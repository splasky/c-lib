#include "mylib/list.h"
#include "test_helpers.h"

static int test_list_create_destroy(void)
{
    mylib_list *list = mylib_list_create();
    TEST_ASSERT_NOT_NULL(list, "List create failed");
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)0, "Count should be 0");
    mylib_list_destroy(list);
    return 0;
}

static int test_list_push_pop(void)
{
    mylib_list *list = mylib_list_create();
    TEST_ASSERT_NOT_NULL(list, "List create failed");

    int a = 1, b = 2, c = 3;

    mylib_list_push(list, &a);
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)1, "Count should be 1");

    mylib_list_push(list, &b);
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)2, "Count should be 2");

    mylib_list_push(list, &c);
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)3, "Count should be 3");

    int *val = mylib_list_pop(list);
    TEST_ASSERT_EQ(*val, 3, "Pop should return 3");
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)2, "Count should be 2");

    val = mylib_list_pop(list);
    TEST_ASSERT_EQ(*val, 2, "Pop should return 2");

    val = mylib_list_pop(list);
    TEST_ASSERT_EQ(*val, 1, "Pop should return 1");

    val = mylib_list_pop(list);
    TEST_ASSERT_NULL(val, "Pop from empty should return NULL");

    mylib_list_destroy(list);
    return 0;
}

static int test_list_push_front(void)
{
    mylib_list *list = mylib_list_create();
    int a = 1, b = 2, c = 3;

    mylib_list_push_front(list, &a);
    mylib_list_push_front(list, &b);
    mylib_list_push_front(list, &c);

    int *val = mylib_list_pop(list);
    TEST_ASSERT_EQ(*val, 1, "Pop should return 1 (c was pushed last)");

    mylib_list_destroy(list);
    return 0;
}

static int test_list_get(void)
{
    mylib_list *list = mylib_list_create();
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++)
        mylib_list_push(list, &vals[i]);

    for (int i = 0; i < 5; i++) {
        int *v = mylib_list_get(list, (size_t)i);
        TEST_ASSERT_EQ(*v, vals[i], "Get returned wrong value");
    }

    void *v = mylib_list_get(list, 100);
    TEST_ASSERT_NULL(v, "Get out of bounds should return NULL");

    mylib_list_destroy(list);
    return 0;
}

static int test_list_insert_at(void)
{
    mylib_list *list = mylib_list_create();
    int a = 1, b = 2, c = 3, d = 4;

    mylib_list_push(list, &a);
    mylib_list_push(list, &c);
    mylib_list_push(list, &d);

    mylib_list_push_at(list, &b, 1);

    TEST_ASSERT_EQ(*(int *)mylib_list_get(list, 0), 1, "Index 0 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_list_get(list, 1), 2, "Index 1 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_list_get(list, 2), 3, "Index 2 wrong");
    TEST_ASSERT_EQ(*(int *)mylib_list_get(list, 3), 4, "Index 3 wrong");

    mylib_list_destroy(list);
    return 0;
}

static int int_ptr_cmp(const void *a, const void *b)
{
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

static int test_list_remove_node(void)
{
    mylib_list *list = mylib_list_create();
    int vals[3] = {1, 2, 3};

    mylib_list_push(list, &vals[0]);
    mylib_list_push(list, &vals[1]);
    mylib_list_push(list, &vals[2]);

    mylib_list_node *node = mylib_list_find(list, &vals[1], int_ptr_cmp);
    TEST_ASSERT_NOT_NULL(node, "Find should work");

    mylib_list_remove_node(list, node);
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)2, "Count should be 2");

    mylib_list_destroy(list);
    return 0;
}

static int test_list_reverse(void)
{
    mylib_list *list = mylib_list_create();
    int vals[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
        mylib_list_push(list, &vals[i]);

    mylib_list_reverse(list);

    for (int i = 0; i < 5; i++) {
        int *v = mylib_list_get(list, (size_t)i);
        TEST_ASSERT_EQ(*v, vals[4 - i], "Reversed order wrong");
    }

    mylib_list_destroy(list);
    return 0;
}

static int test_list_clone(void)
{
    mylib_list *list = mylib_list_create();
    int vals[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++)
        mylib_list_push(list, &vals[i]);

    mylib_list *clone = mylib_list_clone(list);
    TEST_ASSERT_NOT_NULL(clone, "Clone failed");
    TEST_ASSERT_EQ(mylib_list_count(clone), mylib_list_count(list), "Clone count mismatch");

    for (size_t i = 0; i < mylib_list_count(list); i++) {
        int *v1 = mylib_list_get(list, i);
        int *v2 = mylib_list_get(clone, i);
        TEST_ASSERT_EQ(*v1, *v2, "Clone values mismatch");
    }

    mylib_list_destroy(list);
    mylib_list_destroy(clone);
    return 0;
}

static int test_list_clear(void)
{
    mylib_list *list = mylib_list_create();
    mylib_list_push(list, test_malloc(100));
    mylib_list_push(list, test_malloc(100));
    mylib_list_push(list, test_malloc(100));

    mylib_list_clear_with(list, free);
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)0, "Clear should set count to 0");
    TEST_ASSERT(mylib_list_is_empty(list), "List should be empty");

    mylib_list_push(list, test_malloc(100));
    TEST_ASSERT_EQ(mylib_list_count(list), (size_t)1, "Push after clear should work");

    mylib_list_destroy_with(list, free);
    return 0;
}

static int test_list_null_handling(void)
{
    mylib_list_destroy(NULL);
    mylib_list_clear(NULL);
    mylib_list_push(NULL, NULL);
    mylib_list_pop(NULL);
    mylib_list_count(NULL);
    TEST_ASSERT(mylib_list_is_empty(NULL), "NULL list should be empty");
    TEST_ASSERT_NULL(mylib_list_get(NULL, 0), "Get from NULL should return NULL");
    TEST_ASSERT_NULL(mylib_list_find(NULL, NULL, NULL), "Find in NULL should return NULL");
    mylib_list_reverse(NULL);
    mylib_list_merge(NULL, NULL);
    return 0;
}

int main(void)
{
    TEST_SETUP_NAME("List Tests");

    RUN_TEST(test_list_create_destroy);
    RUN_TEST(test_list_push_pop);
    RUN_TEST(test_list_push_front);
    RUN_TEST(test_list_get);
    RUN_TEST(test_list_insert_at);
    RUN_TEST(test_list_remove_node);
    RUN_TEST(test_list_reverse);
    RUN_TEST(test_list_clone);
    RUN_TEST(test_list_clear);
    RUN_TEST(test_list_null_handling);

    TEST_SUMMARY();
}
