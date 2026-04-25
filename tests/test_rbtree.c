#include "rbtree.h"
#include "test_helpers.h"

static int int_cmp(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return ia - ib;
}

static int test_rbtree_create_destroy(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    TEST_ASSERT_NOT_NULL(tree, "Tree create failed");
    TEST_ASSERT(c_lib_rbtree_is_empty(tree), "New tree should be empty");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_insert(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int a = 10, b = 5, c = 15;

    TEST_ASSERT(c_lib_rbtree_insert(tree, &a, &a), "Insert 10 failed");
    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)1, "Count should be 1");

    TEST_ASSERT(c_lib_rbtree_insert(tree, &b, &b), "Insert 5 failed");
    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)2, "Count should be 2");

    TEST_ASSERT(c_lib_rbtree_insert(tree, &c, &c), "Insert 15 failed");
    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)3, "Count should be 3");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_search(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int vals[5] = {10, 5, 15, 3, 7};

    for (int i = 0; i < 5; i++)
        c_lib_rbtree_insert(tree, &vals[i], &vals[i]);

    int key = 5;
    void *result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_NOT_NULL(result, "Search should find 5");
    TEST_ASSERT_EQ(*(int *)result, 5, "Result should be 5");

    key = 99;
    result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_NULL(result, "Search should not find 99");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_update(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int key = 10;
    int val1 = 100, val2 = 200;

    c_lib_rbtree_insert(tree, &key, &val1);
    void *result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_EQ(*(int *)result, 100, "Initial value should be 100");

    c_lib_rbtree_update(tree, &key, &val2);
    result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_EQ(*(int *)result, 200, "Updated value should be 200");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_delete(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int vals[5] = {10, 5, 15, 3, 7};

    for (int i = 0; i < 5; i++)
        c_lib_rbtree_insert(tree, &vals[i], &vals[i]);

    int key = 5;
    TEST_ASSERT(c_lib_rbtree_delete(tree, &key), "Delete should succeed");
    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)4, "Count should be 4");

    void *result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_NULL(result, "Deleted key should not be found");

    TEST_ASSERT(!c_lib_rbtree_delete(tree, &key), "Delete non-existent should fail");
    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)4, "Count unchanged after failed delete");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_min_max(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int vals[5] = {10, 5, 15, 3, 7};

    for (int i = 0; i < 5; i++)
        c_lib_rbtree_insert(tree, &vals[i], &vals[i]);

    c_lib_rbtree_node *min = c_lib_rbtree_min(tree->root);
    TEST_ASSERT_NOT_NULL(min, "Min should not be NULL");
    TEST_ASSERT_EQ(*(int *)min->key, 3, "Min should be 3");

    c_lib_rbtree_node *max = c_lib_rbtree_max(tree->root);
    TEST_ASSERT_NOT_NULL(max, "Max should not be NULL");
    TEST_ASSERT_EQ(*(int *)max->key, 15, "Max should be 15");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_duplicate_update(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int key = 10;
    int val1 = 100, val2 = 200, val3 = 300;

    c_lib_rbtree_insert(tree, &key, &val1);
    c_lib_rbtree_insert(tree, &key, &val2);
    c_lib_rbtree_insert(tree, &key, &val3);

    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)1, "Duplicate keys should update, not insert");

    void *result = c_lib_rbtree_search(tree, &key);
    TEST_ASSERT_EQ(*(int *)result, 300, "Should have latest value");

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_many_insertions(void)
{
    c_lib_rbtree *tree = c_lib_rbtree_create(int_cmp);
    int keys[100];
    int vals[100];

    for (int i = 0; i < 100; i++) {
        keys[i] = i + 1;
        vals[i] = (i + 1) * 10;
    }

    for (int i = 99; i >= 0; i--)
        TEST_ASSERT(c_lib_rbtree_insert(tree, &keys[i], &vals[i]), "Insert failed");

    TEST_ASSERT_EQ(c_lib_rbtree_count(tree), (size_t)100, "Count should be 100");

    for (int i = 0; i < 100; i++) {
        void *result = c_lib_rbtree_search(tree, &keys[i]);
        TEST_ASSERT_NOT_NULL(result, "Search failed for key");
        TEST_ASSERT_EQ(*(int *)result, vals[i], "Value mismatch");
    }

    c_lib_rbtree_destroy(tree);
    return 0;
}

static int test_rbtree_null_handling(void)
{
    c_lib_rbtree_destroy(NULL);
    c_lib_rbtree_clear(NULL);
    c_lib_rbtree_count(NULL);
    TEST_ASSERT(c_lib_rbtree_is_empty(NULL), "NULL tree should be empty");
    TEST_ASSERT_NULL(c_lib_rbtree_search(NULL, NULL), "Search NULL should return NULL");
    TEST_ASSERT(!c_lib_rbtree_insert(NULL, NULL, NULL), "Insert NULL should fail");
    TEST_ASSERT(!c_lib_rbtree_delete(NULL, NULL), "Delete NULL should fail");
    TEST_ASSERT(!c_lib_rbtree_update(NULL, NULL, NULL), "Update NULL should fail");
    c_lib_rbtree_min(NULL);
    c_lib_rbtree_max(NULL);

    TEST_ASSERT_NULL(c_lib_rbtree_create(NULL), "Create with NULL cmp should fail");
    return 0;
}

int main(void)
{
    TEST_SETUP_NAME("Red-Black Tree Tests");

    RUN_TEST(test_rbtree_create_destroy);
    RUN_TEST(test_rbtree_insert);
    RUN_TEST(test_rbtree_search);
    RUN_TEST(test_rbtree_update);
    RUN_TEST(test_rbtree_delete);
    RUN_TEST(test_rbtree_min_max);
    RUN_TEST(test_rbtree_duplicate_update);
    RUN_TEST(test_rbtree_many_insertions);
    RUN_TEST(test_rbtree_null_handling);

    TEST_SUMMARY();
}
