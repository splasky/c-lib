#ifndef MYLIB_DARRAY_H
#define MYLIB_DARRAY_H

#include "base.h"
#include <stdlib.h>
#include <string.h>

#define MYLIB_DARRAY_DEFAULT_CAPACITY 16
#define MYLIB_DARRAY_GROWTH_FACTOR 2

typedef struct mylib_darray {
    void **data;
    mylib_size_t size;
    mylib_size_t capacity;
} mylib_darray;

mylib_darray *mylib_darray_create(mylib_size_t initial_capacity);
void mylib_darray_destroy(mylib_darray *arr);
void mylib_darray_destroy_with(mylib_darray *arr, void (*free_fn)(void *));
void mylib_darray_clear(mylib_darray *arr);
void mylib_darray_clear_with(mylib_darray *arr, void (*free_fn)(void *));

bool mylib_darray_push(mylib_darray *arr, void *value);
void *mylib_darray_pop(mylib_darray *arr);
void *mylib_darray_peek(const mylib_darray *arr);

bool mylib_darray_set(mylib_darray *arr, mylib_size_t index, void *value);
void *mylib_darray_get(const mylib_darray *arr, mylib_size_t index);
void *mylib_darray_take(mylib_darray *arr, mylib_size_t index);

bool mylib_darray_insert(mylib_darray *arr, mylib_size_t index, void *value);
void *mylib_darray_remove(mylib_darray *arr, mylib_size_t index);
bool mylib_darray_erase(mylib_darray *arr, mylib_size_t index, mylib_size_t count);

mylib_size_t mylib_darray_size(const mylib_darray *arr);
mylib_size_t mylib_darray_capacity(const mylib_darray *arr);
bool mylib_darray_is_empty(const mylib_darray *arr);
bool mylib_darray_reserve(mylib_darray *arr, mylib_size_t capacity);
void mylib_darray_shrink_to_fit(mylib_darray *arr);

typedef int (*mylib_darray_cmp)(const void *a, const void *b);
void mylib_darray_sort(mylib_darray *arr, mylib_darray_cmp cmp);

#define MYLIB_DARRAY_FOREACH(arr, var) \
    for (mylib_size_t MYLIB_MACRO_CONCATENATE(_i_, __LINE__) = 0; \
         MYLIB_MACRO_CONCATENATE(_i_, __LINE__) < mylib_darray_size(arr) && ((var) = mylib_darray_get(arr, MYLIB_MACRO_CONCATENATE(_i_, __LINE__)), 1); \
         MYLIB_MACRO_CONCATENATE(_i_, __LINE__)++)

#define MYLIB_MACRO_CONCATENATE(a, b) MYLIB_MACRO_CONCATENATE_IMPL(a, b)
#define MYLIB_MACRO_CONCATENATE_IMPL(a, b) a##b

#endif
