#include "list.h"
#include "test_helpers.h"

static int test_list_create_destroy(void)
{
    c_lib_list *list = c_lib_list_create();
    TEST_ASSERT_NOT_NULL(list, "List create failed");
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)0, "Count should be 0");
    c_lib_list_destroy(list);
    return 0;
}

static int test_list_push_pop(void)
{
    c_lib_list *list = c_lib_list_create();
    TEST_ASSERT_NOT_NULL(list, "List create failed");

    int a = 1, b = 2, c = 3;

    c_lib_list_push(list, &a);
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)1, "Count should be 1");

    c_lib_list_push(list, &b);
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)2, "Count should be 2");

    c_lib_list_push(list, &c);
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)3, "Count should be 3");

    int *val = c_lib_list_pop(list);
    TEST_ASSERT_EQ(*val, 3, "Pop should return 3");
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)2, "Count should be 2");

    val = c_lib_list_pop(list);
    TEST_ASSERT_EQ(*val, 2, "Pop should return 2");

    val = c_lib_list_pop(list);
    TEST_ASSERT_EQ(*val, 1, "Pop should return 1");

    val = c_lib_list_pop(list);
    TEST_ASSERT_NULL(val, "Pop from empty should return NULL");

    c_lib_list_destroy(list);
    return 0;
}

static int test_list_push_front(void)
{
    c_lib_list *list = c_lib_list_create();
    int a = 1, b = 2, c = 3;

    c_lib_list_push_front(list, &a);
    c_lib_list_push_front(list, &b);
    c_lib_list_push_front(list, &c);

    int *val = c_lib_list_pop(list);
    TEST_ASSERT_EQ(*val, 1, "Pop should return 1 (c was pushed last)");

    c_lib_list_destroy(list);
    return 0;
}

static int test_list_get(void)
{
    c_lib_list *list = c_lib_list_create();
    int vals[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++)
        c_lib_list_push(list, &vals[i]);

    for (int i = 0; i < 5; i++) {
        int *v = c_lib_list_get(list, (size_t)i);
        TEST_ASSERT_EQ(*v, vals[i], "Get returned wrong value");
    }

    void *v = c_lib_list_get(list, 100);
    TEST_ASSERT_NULL(v, "Get out of bounds should return NULL");

    c_lib_list_destroy(list);
    return 0;
}

static int test_list_insert_at(void)
{
    c_lib_list *list = c_lib_list_create();
    int a = 1, b = 2, c = 3, d = 4;

    c_lib_list_push(list, &a);
    c_lib_list_push(list, &c);
    c_lib_list_push(list, &d);

    c_lib_list_push_at(list, &b, 1);

    TEST_ASSERT_EQ(*(int *)c_lib_list_get(list, 0), 1, "Index 0 wrong");
    TEST_ASSERT_EQ(*(int *)c_lib_list_get(list, 1), 2, "Index 1 wrong");
    TEST_ASSERT_EQ(*(int *)c_lib_list_get(list, 2), 3, "Index 2 wrong");
    TEST_ASSERT_EQ(*(int *)c_lib_list_get(list, 3), 4, "Index 3 wrong");

    c_lib_list_destroy(list);
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
    c_lib_list *list = c_lib_list_create();
    int vals[3] = {1, 2, 3};

    c_lib_list_push(list, &vals[0]);
    c_lib_list_push(list, &vals[1]);
    c_lib_list_push(list, &vals[2]);

    c_lib_list_node *node = c_lib_list_find(list, &vals[1], int_ptr_cmp);
    TEST_ASSERT_NOT_NULL(node, "Find should work");

    c_lib_list_remove_node(list, node);
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)2, "Count should be 2");

    c_lib_list_destroy(list);
    return 0;
}

static int test_list_reverse(void)
{
    c_lib_list *list = c_lib_list_create();
    int vals[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
        c_lib_list_push(list, &vals[i]);

    c_lib_list_reverse(list);

    for (int i = 0; i < 5; i++) {
        int *v = c_lib_list_get(list, (size_t)i);
        TEST_ASSERT_EQ(*v, vals[4 - i], "Reversed order wrong");
    }

    c_lib_list_destroy(list);
    return 0;
}

static int test_list_clone(void)
{
    c_lib_list *list = c_lib_list_create();
    int vals[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++)
        c_lib_list_push(list, &vals[i]);

    c_lib_list *clone = c_lib_list_clone(list);
    TEST_ASSERT_NOT_NULL(clone, "Clone failed");
    TEST_ASSERT_EQ(c_lib_list_count(clone), c_lib_list_count(list), "Clone count mismatch");

    for (size_t i = 0; i < c_lib_list_count(list); i++) {
        int *v1 = c_lib_list_get(list, i);
        int *v2 = c_lib_list_get(clone, i);
        TEST_ASSERT_EQ(*v1, *v2, "Clone values mismatch");
    }

    c_lib_list_destroy(list);
    c_lib_list_destroy(clone);
    return 0;
}

static int test_list_clear(void)
{
    c_lib_list *list = c_lib_list_create();
    c_lib_list_push(list, test_malloc(100));
    c_lib_list_push(list, test_malloc(100));
    c_lib_list_push(list, test_malloc(100));

    c_lib_list_clear_with(list, free);
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)0, "Clear should set count to 0");
    TEST_ASSERT(c_lib_list_is_empty(list), "List should be empty");

    c_lib_list_push(list, test_malloc(100));
    TEST_ASSERT_EQ(c_lib_list_count(list), (size_t)1, "Push after clear should work");

    c_lib_list_destroy_with(list, free);
    return 0;
}

static int test_list_null_handling(void)
{
    c_lib_list_destroy(NULL);
    c_lib_list_clear(NULL);
    c_lib_list_push(NULL, NULL);
    c_lib_list_pop(NULL);
    c_lib_list_count(NULL);
    TEST_ASSERT(c_lib_list_is_empty(NULL), "NULL list should be empty");
    TEST_ASSERT_NULL(c_lib_list_get(NULL, 0), "Get from NULL should return NULL");
    TEST_ASSERT_NULL(c_lib_list_find(NULL, NULL, NULL), "Find in NULL should return NULL");
    c_lib_list_reverse(NULL);
    c_lib_list_merge(NULL, NULL);
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
