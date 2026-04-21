#include "../include/new.h"
#include "../include/mystring.h"
#include "unit.h"

struct String *a, *b;

TEST(test_New_String)
{
    void* c = New(String, "loli good!");
    void* d = New(String, "Hello world");
    unit_assert(c, "test_New_String_failed");
    unit_assert(d, "test_New_String_failed");
    a = c;
    b = d;
    return NULL;
}

TEST(test_Del)
{
    Del(a);
    a = NULL;
    Del(b);
    b = NULL;
    return NULL;
}

TEST(test_Differ)
{
    unit_assert(Differ(a, b) != 0, "test_Differ failed");
    struct String* c = Clone(a);
    unit_assert(Differ(a, c) == 0, "test_Differ failed");
    return NULL;
}

TEST(test_Clone)
{
    struct String* aa = Clone(a);
    unit_assert(Differ(a, aa) == 0, "test_Clone failed");
    return NULL;
}

TEST(test_sizeOf)
{
    unit_assert(sizeOf(a) == sizeof(String), "test_sizeOf failed");
    return NULL;
}

TEST(all_tests)
{
    unit_suite_start();
    unit_run_test(test_New_String);
    unit_run_test(test_Clone);
    unit_run_test(test_Differ);
    unit_run_test(test_sizeOf);
    unit_run_test(test_Del);
    return NULL;
}

RUN_TESTS(all_tests);
