#ifndef MYLIB_TEST_HELPERS_H
#define MYLIB_TEST_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, msg); \
        return 1; \
    } \
} while (0)

#define TEST_ASSERT_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        fprintf(stderr, "FAIL: %s:%d: %s (expected %ld, got %ld)\n", \
                __FILE__, __LINE__, msg, (long)(b), (long)(a)); \
        return 1; \
    } \
} while (0)

#define TEST_ASSERT_NE(a, b, msg) do { \
    if ((a) == (b)) { \
        fprintf(stderr, "FAIL: %s:%d: %s (should not be %ld)\n", \
                __FILE__, __LINE__, msg, (long)(b)); \
        return 1; \
    } \
} while (0)

#define TEST_ASSERT_NULL(ptr, msg) TEST_ASSERT((ptr) == NULL, msg)
#define TEST_ASSERT_NOT_NULL(ptr, msg) TEST_ASSERT((ptr) != NULL, msg)

#define TEST_ASSERT_STR_EQ(a, b, msg) do { \
    if (strcmp((a), (b)) != 0) { \
        fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, msg); \
        return 1; \
    } \
} while (0)

#define RUN_TEST(fn) do { \
    int result = fn(); \
    if (result != 0) { \
        failures++; \
        printf("  [FAIL] %s\n", #fn); \
    } else { \
        printf("  [PASS] %s\n", #fn); \
    } \
    total++; \
} while (0)

#define TEST_SUMMARY() do { \
    printf("\n%s: %d/%d tests passed\n", \
           failures == 0 ? "SUCCESS" : "FAILED", \
           total - failures, total); \
    return failures > 0 ? 1 : 0; \
} while (0)

#define TEST_SETUP() int failures = 0; int total = 0
#define TEST_SETUP_NAME(name) int failures = 0; int total = 0; printf("\n=== %s ===\n", name)

static inline void *test_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    return ptr;
}

static inline void *test_calloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    return ptr;
}

static inline char *test_strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *copy = (char *)test_malloc(len);
    memcpy(copy, s, len);
    return copy;
}

static inline void test_free(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
}

static inline unsigned long test_rand(void)
{
    static unsigned long state = 12345;
    state = state * 1103515245 + 12345;
    return (state >> 16) & 0x7FFFFFFF;
}

static inline void test_srand(unsigned long seed)
{
    (void)seed;
}

#endif
