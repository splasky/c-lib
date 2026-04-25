#include "../include/dbg.h"
#include "../include/my_malloc.h"
#include "unit.h"
#include <stdbool.h>


TEST(test_malloc)
{
    /* my_malloc / my_free use sbrk and a custom header; my_realloc / my_calloc
     * use libc malloc internally. The two allocators are not interoperable, so
     * each allocation is exercised and freed in isolation through its own path. */
    int* a = my_malloc(10 * sizeof(int));
    unit_assert(a, "Test my_malloc failed!");
    a[0] = 10;
    a[9] = 99;
    unit_assert(a[0] == 10 && a[9] == 99, "Test my_malloc rw failed!");
    my_free(a);

    int* b = my_calloc(10, sizeof(int));
    unit_assert(b, "Test my_calloc failed!");
    unit_assert(b[0] == 0 && b[9] == 0, "Test my_calloc zero-init failed!");
    b[9] = 42;
    unit_assert(b[9] == 42, "Test my_calloc rw failed!");
    free(b);

    int* c = my_realloc(NULL, 5 * sizeof(int));
    unit_assert(c, "Test my_realloc(NULL) failed!");
    c[4] = 7;
    unit_assert(c[4] == 7, "Test my_realloc rw failed!");
    free(c);

    return NULL;
}

TEST(all_tests)
{

    unit_suite_start();
    unit_run_test(test_malloc);
    return NULL;
}

RUN_TESTS(all_tests);
