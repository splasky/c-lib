#include "mylib/hashmap.h"
#include "test_helpers.h"

static int test_hashmap_create_destroy(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    TEST_ASSERT_NOT_NULL(map, "Hashmap create failed");
    TEST_ASSERT(mylib_hashmap_is_empty(map), "New hashmap should be empty");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_put_get(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    const char *key1 = "key1";
    const char *key2 = "key2";
    int val1 = 100, val2 = 200;

    TEST_ASSERT(mylib_hashmap_put(map, key1, strlen(key1), &val1), "Put key1 failed");
    TEST_ASSERT(!mylib_hashmap_is_empty(map), "Hashmap should not be empty");

    TEST_ASSERT(mylib_hashmap_put(map, key2, strlen(key2), &val2), "Put key2 failed");

    int *result = mylib_hashmap_get(map, key1, strlen(key1));
    TEST_ASSERT_NOT_NULL(result, "Get key1 failed");
    TEST_ASSERT_EQ(*result, 100, "Value should be 100");

    result = mylib_hashmap_get(map, key2, strlen(key2));
    TEST_ASSERT_NOT_NULL(result, "Get key2 failed");
    TEST_ASSERT_EQ(*result, 200, "Value should be 200");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_update(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    const char *key = "key";
    int val1 = 100, val2 = 200, val3 = 300;

    mylib_hashmap_put(map, key, strlen(key), &val1);
    int *result = mylib_hashmap_get(map, key, strlen(key));
    TEST_ASSERT_EQ(*result, 100, "Initial value should be 100");

    mylib_hashmap_put(map, key, strlen(key), &val2);
    result = mylib_hashmap_get(map, key, strlen(key));
    TEST_ASSERT_EQ(*result, 200, "Value should be updated to 200");

    mylib_hashmap_put(map, key, strlen(key), &val3);
    result = mylib_hashmap_get(map, key, strlen(key));
    TEST_ASSERT_EQ(*result, 300, "Value should be updated to 300");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_remove(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    const char *key = "key";
    int val = 100;

    mylib_hashmap_put(map, key, strlen(key), &val);
    TEST_ASSERT_EQ(mylib_hashmap_count(map), (size_t)1, "Count should be 1");

    void *removed = mylib_hashmap_remove(map, key, strlen(key));
    TEST_ASSERT_NOT_NULL(removed, "Remove should return value");
    TEST_ASSERT_EQ(mylib_hashmap_count(map), (size_t)0, "Count should be 0");

    void *result = mylib_hashmap_get(map, key, strlen(key));
    TEST_ASSERT_NULL(result, "Get after remove should return NULL");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_contains(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    const char *key = "key";
    int val = 100;

    TEST_ASSERT(!mylib_hashmap_contains(map, key, strlen(key)), "Contains should be false");

    mylib_hashmap_put(map, key, strlen(key), &val);
    TEST_ASSERT(mylib_hashmap_contains(map, key, strlen(key)), "Contains should be true");

    mylib_hashmap_remove(map, key, strlen(key));
    TEST_ASSERT(!mylib_hashmap_contains(map, key, strlen(key)), "Contains should be false after remove");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_many_insertions(void)
{
    mylib_hashmap *map = mylib_hashmap_create(32);
    char keys[1000][32];
    int vals[1000];

    for (int i = 0; i < 1000; i++) {
        snprintf(keys[i], sizeof(keys[i]), "key%d", i);
        vals[i] = i * 100;
        TEST_ASSERT(mylib_hashmap_put(map, keys[i], strlen(keys[i]), &vals[i]), "Put failed");
    }

    TEST_ASSERT_EQ(mylib_hashmap_count(map), (size_t)1000, "Count should be 1000");

    for (int i = 0; i < 1000; i++) {
        int *result = mylib_hashmap_get(map, keys[i], strlen(keys[i]));
        if (result == NULL) {
            fprintf(stderr, "FAIL at i=%d: key=%s\n", i, keys[i]);
        }
        TEST_ASSERT_NOT_NULL(result, "Get failed");
        TEST_ASSERT_EQ(*result, vals[i], "Value mismatch");
    }

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_resize(void)
{
    mylib_hashmap *map = mylib_hashmap_create(8);
    char keys[100][32];
    int vals[100];

    for (int i = 0; i < 100; i++) {
        snprintf(keys[i], sizeof(keys[i]), "key%d", i);
        vals[i] = i;
        TEST_ASSERT(mylib_hashmap_put(map, keys[i], strlen(keys[i]), &vals[i]), "Put failed after resize");
    }

    for (int i = 0; i < 100; i++) {
        int *result = mylib_hashmap_get(map, keys[i], strlen(keys[i]));
        TEST_ASSERT_NOT_NULL(result, "Get after resize failed");
        TEST_ASSERT_EQ(*result, vals[i], "Value mismatch after resize");
    }

    mylib_hashmap_destroy(map);
    return 0;
}

static bool visit_count(const void *key, mylib_size_t keylen, void *value, void *userdata)
{
    (void)key;
    (void)keylen;
    (void)value;
    int *count = (int *)userdata;
    (*count)++;
    return true;
}

static int test_hashmap_foreach(void)
{
    mylib_hashmap *map = mylib_hashmap_create(16);
    int visited = 0;

    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", i);
        mylib_hashmap_put(map, key, strlen(key), &i);
    }

    bool ok = mylib_hashmap_foreach(map, visit_count, &visited);
    TEST_ASSERT(ok, "Foreach should succeed");
    TEST_ASSERT_EQ(visited, 10, "Should have visited 10 items");

    mylib_hashmap_destroy(map);
    return 0;
}

static int test_hashmap_null_handling(void)
{
    mylib_hashmap_destroy(NULL);
    mylib_hashmap_clear(NULL);
    mylib_hashmap_count(NULL);
    TEST_ASSERT(mylib_hashmap_is_empty(NULL), "NULL hashmap should be empty");
    TEST_ASSERT_NULL(mylib_hashmap_get(NULL, NULL, 0), "Get NULL should return NULL");
    TEST_ASSERT(!mylib_hashmap_put(NULL, NULL, 0, NULL), "Put NULL should fail");
    TEST_ASSERT_NULL(mylib_hashmap_remove(NULL, NULL, 0), "Remove NULL should fail");
    TEST_ASSERT(!mylib_hashmap_contains(NULL, NULL, 0), "Contains NULL should return false");
    TEST_ASSERT(!mylib_hashmap_foreach(NULL, NULL, NULL), "Foreach NULL should fail");

    {
        mylib_hashmap *tmp = mylib_hashmap_create(0);
        TEST_ASSERT_NOT_NULL(tmp, "Create with 0 capacity should use default");
        mylib_hashmap_destroy(tmp);
    }
    return 0;
}

int main(void)
{
    TEST_SETUP_NAME("Hashmap Tests");

    RUN_TEST(test_hashmap_create_destroy);
    RUN_TEST(test_hashmap_put_get);
    RUN_TEST(test_hashmap_update);
    RUN_TEST(test_hashmap_remove);
    RUN_TEST(test_hashmap_contains);
    RUN_TEST(test_hashmap_many_insertions);
    RUN_TEST(test_hashmap_resize);
    RUN_TEST(test_hashmap_foreach);
    RUN_TEST(test_hashmap_null_handling);

    TEST_SUMMARY();
}
