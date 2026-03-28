#include "mylib/darray.h"
#include "test_helpers.h"

static int int_cmp(const void *a, const void *b)
{
    int ia = **(const int *const *)a;
    int ib = **(const int *const *)b;
    return ia - ib;
}

static int test_darray_create_destroy(void)
{
    mylib_darray *arr = mylib_darray_create(16);
    TEST_ASSERT_NOT_NULL(arr, "Darray create failed");
    TEST_ASSERT(mylib_darray_is_empty(arr), "New darray should be empty");
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)0, "Size should be 0");
    TEST_ASSERT_EQ(mylib_darray_capacity(arr), (size_t)16, "Capacity should be 16");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_push_pop(void)
{
    mylib_darray *arr = mylib_darray_create(4);
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(mylib_darray_push(arr, &vals[i]), "Push failed");
        TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)(i + 1), "Size mismatch");
    }

    TEST_ASSERT_EQ(mylib_darray_capacity(arr), (size_t)8, "Capacity should have grown to 8");

    void *p = mylib_darray_pop(arr);
    TEST_ASSERT_EQ(*(int *)p, 50, "Pop should return 50");
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)4, "Size should be 4");

    p = mylib_darray_pop(arr);
    TEST_ASSERT_EQ(*(int *)p, 40, "Pop should return 40");

    p = mylib_darray_pop(arr);
    TEST_ASSERT_EQ(*(int *)p, 30, "Pop should return 30");

    p = mylib_darray_pop(arr);
    TEST_ASSERT_EQ(*(int *)p, 20, "Pop should return 20");

    p = mylib_darray_pop(arr);
    TEST_ASSERT_EQ(*(int *)p, 10, "Pop should return 10");

    p = mylib_darray_pop(arr);
    TEST_ASSERT_NULL(p, "Pop from empty should return NULL");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_get_set(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++)
        mylib_darray_push(arr, &vals[i]);

    for (int i = 0; i < 5; i++) {
        int *v = mylib_darray_get(arr, (size_t)i);
        TEST_ASSERT_EQ(*v, vals[i], "Get returned wrong value");
    }

    TEST_ASSERT_NULL(mylib_darray_get(arr, 100), "Get out of bounds should return NULL");

    int new_val = 999;
    TEST_ASSERT(mylib_darray_set(arr, 2, &new_val), "Set should succeed");
    int *v = mylib_darray_get(arr, 2);
    TEST_ASSERT_EQ(*v, 999, "Set should update value");

    TEST_ASSERT(!mylib_darray_set(arr, 100, &new_val), "Set out of bounds should fail");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_insert(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 3; i++)
        mylib_darray_push(arr, &vals[i]);

    int new_val = 25;
    TEST_ASSERT(mylib_darray_insert(arr, 1, &new_val), "Insert should succeed");
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)4, "Size should be 4");

    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 0), 10, "Index 0 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 1), 25, "Index 1 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 2), 20, "Index 2 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 3), 30, "Index 3 wrong");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_remove(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++)
        mylib_darray_push(arr, &vals[i]);

    void *removed = mylib_darray_remove(arr, 2);
    TEST_ASSERT_EQ(*(int *)removed, 30, "Removed value should be 30");
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)4, "Size should be 4");

    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 0), 10, "Index 0 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 1), 20, "Index 1 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 2), 40, "Index 2 wrong (was 40)");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 3), 50, "Index 3 wrong");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_peek(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int val = 42;

    TEST_ASSERT_NULL(mylib_darray_peek(arr), "Peek empty should return NULL");

    mylib_darray_push(arr, &val);
    int *p = mylib_darray_peek(arr);
    TEST_ASSERT_EQ(*p, 42, "Peek should return 42");

    mylib_darray_push(arr, &val);
    p = mylib_darray_peek(arr);
    TEST_ASSERT_EQ(*p, 42, "Peek should still return 42");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_reserve(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int vals[4];

    for (int i = 0; i < 4; i++) {
        vals[i] = 10 + i;
        mylib_darray_push(arr, &vals[i]);
    }

    TEST_ASSERT(mylib_darray_reserve(arr, 32), "Reserve should succeed");
    TEST_ASSERT_EQ(mylib_darray_capacity(arr), (size_t)32, "Capacity should be 32");
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)4, "Size should be unchanged");

    TEST_ASSERT(mylib_darray_reserve(arr, 16), "Reserve to smaller should be no-op");
    TEST_ASSERT_EQ(mylib_darray_capacity(arr), (size_t)32, "Capacity unchanged");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_shrink(void)
{
    mylib_darray *arr = mylib_darray_create(32);
    int vals[8];

    for (int i = 0; i < 8; i++) {
        vals[i] = 10 + i;
        mylib_darray_push(arr, &vals[i]);
    }

    mylib_darray_shrink_to_fit(arr);
    TEST_ASSERT_EQ(mylib_darray_capacity(arr), (size_t)8, "Capacity should shrink to size");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_clear(void)
{
    mylib_darray *arr = mylib_darray_create(8);

    for (int i = 0; i < 5; i++)
        mylib_darray_push(arr, test_malloc(100));

    mylib_darray_clear_with(arr, free);
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)0, "Size should be 0 after clear");

    mylib_darray_push(arr, test_malloc(100));
    TEST_ASSERT_EQ(mylib_darray_size(arr), (size_t)1, "Push after clear should work");

    mylib_darray_destroy_with(arr, free);
    return 0;
}

static int test_darray_sort(void)
{
    mylib_darray *arr = mylib_darray_create(8);
    int vals[5] = {30, 10, 50, 20, 40};

    for (int i = 0; i < 5; i++)
        mylib_darray_push(arr, &vals[i]);

    mylib_darray_sort(arr, (mylib_darray_cmp)int_cmp);

    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 0), 10, "Sorted[0] should be 10");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 1), 20, "Sorted[1] should be 20");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 2), 30, "Sorted[2] should be 30");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 3), 40, "Sorted[3] should be 40");
    TEST_ASSERT_EQ(*(int *)mylib_darray_get(arr, 4), 50, "Sorted[4] should be 50");

    mylib_darray_destroy(arr);
    return 0;
}

static int test_darray_null_handling(void)
{
    mylib_darray_destroy(NULL);
    mylib_darray_destroy_with(NULL, free);
    mylib_darray_clear(NULL);
    mylib_darray_push(NULL, NULL);
    mylib_darray_pop(NULL);
    mylib_darray_peek(NULL);
    mylib_darray_get(NULL, 0);
    mylib_darray_set(NULL, 0, NULL);
    mylib_darray_remove(NULL, 0);
    mylib_darray_insert(NULL, 0, NULL);
    mylib_darray_size(NULL);
    mylib_darray_capacity(NULL);
    TEST_ASSERT(mylib_darray_is_empty(NULL), "NULL darray should be empty");
    TEST_ASSERT(!mylib_darray_reserve(NULL, 16), "Reserve NULL should fail");
    mylib_darray_shrink_to_fit(NULL);
    mylib_darray_sort(NULL, NULL);

    {
        mylib_darray *tmp = mylib_darray_create(0);
        TEST_ASSERT_NOT_NULL(tmp, "Create with 0 capacity should use default");
        mylib_darray_destroy(tmp);
    }
    return 0;
}

int main(void)
{
    TEST_SETUP_NAME("Dynamic Array Tests");

    RUN_TEST(test_darray_create_destroy);
    RUN_TEST(test_darray_push_pop);
    RUN_TEST(test_darray_get_set);
    RUN_TEST(test_darray_insert);
    RUN_TEST(test_darray_remove);
    RUN_TEST(test_darray_peek);
    RUN_TEST(test_darray_reserve);
    RUN_TEST(test_darray_shrink);
    RUN_TEST(test_darray_clear);
    RUN_TEST(test_darray_sort);
    RUN_TEST(test_darray_null_handling);

    TEST_SUMMARY();
}
