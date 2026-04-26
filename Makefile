# c-lib - CPU-optimized Makefile

VERSION := 3.0.0
CPU_TARGET ?= generic
PREFIX ?= /usr/local

# CPU_TARGET selects the toolchain prefix and architecture flags.
#
# Bare-metal Cortex-M4 (no libc, no libm, no syscalls -> no `make test`):
#   arm32       Cortex-M4   soft-float
#   arm32-hf    Cortex-M4F  hard-float (FPv4-SP)
#
# Hosted ARM Linux (full libc -> `make test` runs under qemu-user):
#   arm32-linux     ARMv5+ via gnueabi  (soft-float ABI)
#   arm32-linux-hf  ARMv7-A via gnueabihf (hard-float ABI, VFPv3)
ifeq ($(CPU_TARGET),arm32)
    CROSS_COMPILE := arm-none-eabi-
    ARCH_CFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft \
                   -ffreestanding -fno-builtin -fno-common
    BARE_METAL := 1
else ifeq ($(CPU_TARGET),arm32-hf)
    CROSS_COMPILE := arm-none-eabi-
    ARCH_CFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
                   -ffreestanding -fno-builtin -fno-common
    BARE_METAL := 1
else ifeq ($(CPU_TARGET),arm32-linux)
    CROSS_COMPILE := arm-linux-gnueabi-
else ifeq ($(CPU_TARGET),arm32-linux-hf)
    CROSS_COMPILE := arm-linux-gnueabihf-
endif

# GNU Make predefines CC=cc and AR=ar, so a plain `?=` here would never let
# us swap in a cross-compiler. Treat make's built-in defaults as "unset".
# gcc-ar / gcc-ranlib understand LTO bytecode (.gnu.lto_*) sections in object
# files; plain `ar` silently drops them and breaks LTO for archive consumers.
ifeq ($(origin CC),default)
    CC := $(CROSS_COMPILE)gcc
endif
ifeq ($(origin AR),default)
    AR := $(CROSS_COMPILE)gcc-ar
endif
RANLIB ?= $(CROSS_COMPILE)gcc-ranlib
STRIP ?= $(CROSS_COMPILE)strip
OBJCOPY ?= $(CROSS_COMPILE)objcopy
SIZE ?= $(CROSS_COMPILE)size
RM ?= rm
MKDIR ?= mkdir -p

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build/$(CPU_TARGET)
TEST_DIR := tests

# Library is scoped to these modules only (other src/*.c are unrelated legacy files).
MODULES := darray hashmap list rbtree
SOURCES := $(addprefix $(SRC_DIR)/,$(addsuffix .c,$(MODULES)))
OBJECTS := $(MODULES:%=$(BUILD_DIR)/%.o)
HEADERS := $(addprefix $(INC_DIR)/,$(addsuffix .h,base $(MODULES)))
TEST_SOURCES := $(addprefix $(TEST_DIR)/,$(addsuffix .c,$(MODULES:%=test_%)))

# Build type
BUILD_TYPE ?= release

# CPU_TARGET -> -march / -mtune mapping. Used by BUILD_TYPE=fast.
# -march sets the ISA baseline (allows e.g. SSE4 / AVX2 instructions).
# -mtune steers scheduling. Some -march values (like "x86-64-v2") aren't
# valid -mtune values, so the two are kept independent.
# Override with MARCH=... / MTUNE=... if you need something exotic.
ifeq ($(CPU_TARGET),zen2)
    MARCH ?= znver2
    MTUNE ?= znver2
else ifeq ($(CPU_TARGET),zen3)
    MARCH ?= znver3
    MTUNE ?= znver3
else ifeq ($(CPU_TARGET),native)
    MARCH ?= native
    MTUNE ?= native
else
    MARCH ?= x86-64-v2
    MTUNE ?= generic
endif

ifeq ($(BUILD_TYPE),debug)
    CFLAGS = -O0 -g -std=gnu2x -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined
    LDFLAGS = -lm -fsanitize=address -fsanitize=undefined
    TARGET = c_lib-$(CPU_TARGET)-debug
else ifeq ($(BUILD_TYPE),fast)
    # Maximum-performance build. Trades binary size and IEEE-754 strictness
    # for raw throughput.
    #
    # Compile-time:
    #   -O3                              full optimizer pipeline
    #   -march=$(MARCH) -mtune=$(MARCH)  emit and tune for the target uarch
    #   -funroll-loops                   unroll hot loops
    #   -ftree-vectorize                 enable auto-vectorizer (default at -O3)
    #   -fipa-pta                        whole-program points-to analysis
    #   -fno-semantic-interposition      allow cross-TU inlining of extern fns
    #   -fno-plt                         direct calls instead of through PLT
    #   -fno-stack-protector             skip canary checks
    #   -fomit-frame-pointer             free %rbp as a GP register
    #   -fno-math-errno -fno-trapping-math    skip libm errno + FP trap setup
    #   -fno-signed-zeros -fno-rounding-math  permit reassociation
    #   -ffinite-math-only               assume no NaN / Inf
    #   -flto=auto                       link-time inlining + dead-code elim
    #
    # Link-time:
    #   -Wl,-O3                          maximal linker peephole opts
    #   -Wl,--as-needed                  no DT_NEEDED for unused libs
    #   -Wl,--hash-style=gnu             faster runtime symbol resolution
    #   -Wl,-z,now                       eager bind, no per-call PLT lookup
    CFLAGS = -O3 -pipe -std=gnu2x -Wall -Wextra -Werror \
             -march=$(MARCH) -mtune=$(MTUNE) \
             -funroll-loops -ftree-vectorize \
             -fipa-pta -fno-semantic-interposition \
             -fno-plt -fno-stack-protector \
             -fomit-frame-pointer \
             -fno-math-errno -fno-trapping-math \
             -fno-signed-zeros -fno-rounding-math \
             -ffinite-math-only \
             -flto=auto
    LDFLAGS = -lm -flto=auto \
              -Wl,-O3 -Wl,--as-needed \
              -Wl,--hash-style=gnu -Wl,-z,now
    TARGET = c_lib-$(CPU_TARGET)-fast
else
    # Aggressive size optimization for release builds.
    #
    # Compile-time:
    #   -Os                              size > speed
    #   -ffunction-sections              one section per function ...
    #   -fdata-sections                  ... and per data object, so the linker
    #                                    can drop unreferenced ones with --gc-sections
    #   -fmerge-all-constants            fold identical constants
    #   -fno-asynchronous-unwind-tables  drop .eh_frame_hdr / async unwind data
    #   -fno-unwind-tables               drop .eh_frame entirely
    #   -fno-stack-protector             drop canary thunks (we already validate inputs)
    #   -fno-ident                       no GCC version string in .comment
    #   -fno-plt                         smaller GOT for shared libs
    #   -flto=auto                       cross-file inlining + dead-code elim
    #
    # Link-time:
    #   --gc-sections                    drop sections nothing references
    #   --as-needed                      only DT_NEEDED for libs we actually use
    #   -O1                              linker peephole / hash-section opts
    #   --hash-style=gnu                 smaller, faster than sysv
    #   --build-id=none                  no .note.gnu.build-id
    #   -z,now                           full eager bind, smaller .got.plt
    CFLAGS = -Os -pipe -std=gnu2x -Wall -Wextra -Werror \
             -ffunction-sections -fdata-sections \
             -fmerge-all-constants \
             -fno-asynchronous-unwind-tables -fno-unwind-tables \
             -fno-stack-protector -fno-ident \
             -fno-plt -flto=auto
    LDFLAGS = -lm -flto=auto \
              -Wl,--gc-sections -Wl,--as-needed \
              -Wl,-O1 -Wl,--hash-style=gnu \
              -Wl,--build-id=none -Wl,-z,now
    TARGET = c_lib-$(CPU_TARGET)-release
endif

# Bare-metal targets: append CPU/FPU flags, drop libc-dependent flags.
ifdef BARE_METAL
    CFLAGS += $(ARCH_CFLAGS)
    # No libm, no dynamic loader, no PLT, no eager binding -- bare-metal links
    # are user-supplied with their own linker script.
    LDFLAGS = -nostdlib
endif

# Coverage
ifeq ($(COVERAGE),1)
    CFLAGS += --coverage -g -O0
    LDFLAGS += --coverage
endif

INCLUDES := -I$(INC_DIR)
STATIC_LIB := $(BUILD_DIR)/lib$(TARGET).a
SHARED_LIB := $(BUILD_DIR)/lib$(TARGET).so.$(VERSION)
SHARED_SONAME := lib$(TARGET).so.$(firstword $(subst ., ,$(VERSION)))
SHARED_LINK := $(BUILD_DIR)/lib$(TARGET).so

PIC_OBJECTS := $(MODULES:%=$(BUILD_DIR)/%.pic.o)

$(BUILD_DIR):
	$(MKDIR) -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.pic.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC $(INCLUDES) -c $< -o $@

# `ar D` = deterministic archive (no timestamps/uid/gid) for reproducible /
# slightly smaller output. Release builds also strip debug + non-essential
# locals from each archive member and drop .comment / .note* sections.
$(STATIC_LIB): $(OBJECTS)
	$(AR) Drcs $@ $^
ifneq ($(BUILD_TYPE),debug)
	@$(STRIP) --strip-debug --strip-unneeded $@ 2>/dev/null || true
	@$(OBJCOPY) --remove-section=.comment --remove-section=.note.* $@ 2>/dev/null || true
endif

# Shared lib: --strip-unneeded keeps .dynsym (required for runtime resolution)
# but drops the static .symtab/.strtab and any local symbols.
$(SHARED_LIB): $(PIC_OBJECTS)
	$(CC) -shared -Wl,-soname,$(SHARED_SONAME) -o $@ $^ $(LDFLAGS)
ifneq ($(BUILD_TYPE),debug)
	@$(STRIP) --strip-unneeded $@
	@$(OBJCOPY) --remove-section=.comment --remove-section=.note.* $@ 2>/dev/null || true
endif
	@cd $(BUILD_DIR) && ln -sf $(notdir $(SHARED_LIB)) $(SHARED_SONAME) \
		&& ln -sf $(SHARED_SONAME) $(notdir $(SHARED_LINK))

.PHONY: all help test coverage coverage_html clean info install uninstall static shared _shared_real size

all: info $(STATIC_LIB)

static: info $(STATIC_LIB)
	@echo "  Static : $(STATIC_LIB) ($$(stat -c%s $(STATIC_LIB)) bytes)"

shared:
ifdef BARE_METAL
	@echo "ERROR: 'shared' is not supported for bare-metal CPU_TARGET=$(CPU_TARGET)." >&2
	@echo "       Bare-metal builds have no dynamic loader; use 'static' instead." >&2
	@exit 1
else
	@$(MAKE) --no-print-directory _shared_real
endif

_shared_real: info $(SHARED_LIB)
	@echo "  Shared : $(SHARED_LIB) ($$(stat -c%s $(SHARED_LIB)) bytes)"
	@echo "  SONAME : $(SHARED_SONAME)"
	@echo "  Link   : $(SHARED_LINK)"

# `make size`: build both artifacts (if not already built) and report a concise
# summary. Berkeley-format `size` shows text/data/bss totals per archive member
# and per ELF for the shared lib; full per-section dumps from -ffunction-sections
# are too noisy to be useful.
size: $(STATIC_LIB) $(SHARED_LIB)
	@echo "=== artifact sizes ($(BUILD_TYPE), $(CPU_TARGET)) ==="
	@for f in $(STATIC_LIB) $(SHARED_LIB); do \
		printf "  %-60s %10d bytes\n" "$$f" "$$(stat -c%s $$f)"; \
	done
	@echo ""
	@echo "=== $(notdir $(SHARED_LIB)) sections ==="
	@$(SIZE) $(SHARED_LIB)
	@echo ""
	@echo "=== $(notdir $(SHARED_LIB)) exported symbols ==="
	@printf "  %d public symbol(s) in .dynsym\n" \
		"$$(nm -D --defined-only $(SHARED_LIB) | grep -c ' T ')"
	@echo ""
	@echo "=== $(notdir $(STATIC_LIB)) member totals ==="
	@$(SIZE) --totals $(STATIC_LIB) | tail -n +1

info:
	@echo "c-lib v$(VERSION) - $(CPU_TARGET)"
	@echo "  Output: $(STATIC_LIB)"

# Test/coverage targets operate on the FULL tree (every src/*.c, every tests/test_*.c).
# Strict mode: any compile error, link error, or test failure aborts the build.
ALL_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ALL_TESTS := $(wildcard $(TEST_DIR)/test_*.c)
ALL_OBJECTS := $(ALL_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/all/%.o)
COV_DIR := $(BUILD_DIR)/coverage
COV_OBJECTS := $(ALL_SOURCES:$(SRC_DIR)/%.c=$(COV_DIR)/%.o)
# Permissive flags: legacy code has warnings, so no -Werror/-Wextra.
FULL_CFLAGS := -O0 -g -std=gnu2x -Wall -I$(INC_DIR)

# Each test in `make test` is run under valgrind. Override with `make test VALGRIND=`
# (empty) to skip the leak check.
VALGRIND ?= valgrind --leak-check=full --error-exitcode=1 --errors-for-leak-kinds=all --quiet

$(BUILD_DIR)/all:
	$(MKDIR) -p $(BUILD_DIR)/all

$(COV_DIR):
	$(MKDIR) -p $(COV_DIR)

$(BUILD_DIR)/all/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/all
	$(CC) $(FULL_CFLAGS) -c $< -o $@

$(COV_DIR)/%.o: $(SRC_DIR)/%.c | $(COV_DIR)
	$(CC) $(FULL_CFLAGS) --coverage -c $< -o $@

# `make test`: build every src/*.c, build & run every tests/test_*.c under valgrind.
# Aborts on first compile error, link error, test failure, or memory leak.
test: $(ALL_OBJECTS)
	@if [ -n "$(VALGRIND)" ] && ! command -v $(firstword $(VALGRIND)) >/dev/null 2>&1; then \
		echo "ERROR: valgrind not found in PATH. Install valgrind, or skip the leak check with 'make test VALGRIND='." >&2; \
		exit 1; \
	fi
	@echo "Running all tests in $(TEST_DIR)/$(if $(VALGRIND), under valgrind,)..."
	@set -e; for t in $(ALL_TESTS); do \
		n=$$(basename $$t .c); \
		echo "  [BUILD]    $$n"; \
		$(CC) $(FULL_CFLAGS) $$t $(ALL_OBJECTS) -o $(BUILD_DIR)/all/$$n -lm; \
		echo "  [RUN]      $$n"; \
		$(BUILD_DIR)/all/$$n >/dev/null; \
		if [ -n "$(VALGRIND)" ]; then \
			echo "  [VALGRIND] $$n"; \
			$(VALGRIND) $(BUILD_DIR)/all/$$n >/dev/null; \
		fi; \
	done
	@echo "All $(words $(ALL_TESTS)) tests passed$(if $(VALGRIND), (clean under valgrind),)."

# `make coverage` and `coverage_html`: same as `test` but with --coverage instrumentation.
coverage: $(COV_OBJECTS)
	@echo "Running all tests with coverage instrumentation..."
	@set -e; for t in $(ALL_TESTS); do \
		n=$$(basename $$t .c); \
		echo "  [BUILD] $$n"; \
		$(CC) $(FULL_CFLAGS) --coverage $$t $(COV_OBJECTS) -o $(COV_DIR)/$$n -lm; \
		echo "  [RUN]   $$n"; \
		$(COV_DIR)/$$n >/dev/null; \
	done
	@echo "All $(words $(ALL_TESTS)) tests passed."
	@echo ""
	@echo "=== Coverage Report ==="
	@gcovr --root . --filter '^$(SRC_DIR)/' \
		--gcov-object-directory $(COV_DIR) 2>/dev/null \
		| sed -n '/^File/,/^TOTAL/p'

coverage_html: coverage
	@$(MKDIR) -p coverage_html
	@gcovr --root . --filter '^$(SRC_DIR)/' --html --html-details \
		-o coverage_html/index.html \
		--gcov-object-directory $(COV_DIR) >/dev/null
	@echo "HTML coverage: coverage_html/index.html"

clean:
	$(RM) -rf $(BUILD_DIR) coverage_html
	$(RM) -f *.gcov *.gcda *.gcno
	find . -name '*.gcov' -o -name '*.gcda' -o -name '*.gcno' | xargs -r $(RM) -f

install: $(STATIC_LIB)
	$(MKDIR) -p $(PREFIX)/include $(PREFIX)/lib
	install -m 644 $(HEADERS) $(PREFIX)/include/
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib/
	@if [ -f $(SHARED_LIB) ]; then \
		install -m 755 $(SHARED_LIB) $(PREFIX)/lib/; \
		cd $(PREFIX)/lib && ln -sf $(notdir $(SHARED_LIB)) $(SHARED_SONAME) \
			&& ln -sf $(SHARED_SONAME) $(notdir $(SHARED_LINK)); \
	fi

uninstall:
	$(RM) -f $(addprefix $(PREFIX)/include/,$(notdir $(HEADERS)))
	$(RM) -f $(PREFIX)/lib/libc_lib*.a $(PREFIX)/lib/libc_lib*.so*

help:
	@echo "c-lib v$(VERSION) - Targets:"
	@echo "  make, static, shared, test, clean, info, size"
	@echo "  coverage, coverage_html"
	@echo "  install, uninstall"
	@echo ""
	@echo "  CPU_TARGET=zen3|zen2|generic|native"
	@echo "  BUILD_TYPE=debug|release|fast"
	@echo "    debug   = -O0 -g + ASAN/UBSAN"
	@echo "    release = -Os + LTO + gc-sections + strip   (smallest binary)"
	@echo "    fast    = -O3 + LTO + -march=\$$MARCH       (fastest binary)"
