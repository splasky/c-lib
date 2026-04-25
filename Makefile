# c-lib - CPU-optimized Makefile

CC ?= gcc
AR ?= ar
RM ?= rm
MKDIR ?= mkdir -p

VERSION := 3.0.0
CPU_TARGET ?= generic
PREFIX ?= /usr/local

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
ifeq ($(BUILD_TYPE),debug)
    CFLAGS = -O0 -g -std=gnu2x -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined
    LDFLAGS = -lm -fsanitize=address -fsanitize=undefined
    TARGET = c_lib-$(CPU_TARGET)-debug
else
    CFLAGS = -O3 -pipe -std=gnu2x -Wall -Wextra -Werror
    LDFLAGS = -lm
    TARGET = c_lib-$(CPU_TARGET)-release
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

$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $^

$(SHARED_LIB): $(PIC_OBJECTS)
	$(CC) -shared -Wl,-soname,$(SHARED_SONAME) -o $@ $^ $(LDFLAGS)
	@cd $(BUILD_DIR) && ln -sf $(notdir $(SHARED_LIB)) $(SHARED_SONAME) \
		&& ln -sf $(SHARED_SONAME) $(notdir $(SHARED_LINK))

.PHONY: all help test coverage coverage_html clean info install uninstall static shared

all: info $(STATIC_LIB)

static: info $(STATIC_LIB)
	@echo "  Static : $(STATIC_LIB)"

shared: info $(SHARED_LIB)
	@echo "  Shared : $(SHARED_LIB)"
	@echo "  SONAME : $(SHARED_SONAME)"
	@echo "  Link   : $(SHARED_LINK)"

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
	@echo "  make, static, shared, test, clean, info"
	@echo "  coverage, coverage_html"
	@echo "  install, uninstall"
	@echo ""
	@echo "  CPU_TARGET=zen3|zen2|generic"
	@echo "  BUILD_TYPE=debug|release"
