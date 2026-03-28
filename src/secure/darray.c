#include "mylib/darray.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

mylib_darray *mylib_darray_create(mylib_size_t initial_capacity)
{
    if (initial_capacity == 0)
        initial_capacity = MYLIB_DARRAY_DEFAULT_CAPACITY;

    mylib_darray *arr = calloc(1, sizeof(*arr));
    if (MYLIB_UNLIKELY(arr == NULL))
        return NULL;

    arr->data = calloc(initial_capacity, sizeof(*arr->data));
    if (MYLIB_UNLIKELY(arr->data == NULL)) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;

    return arr;
}

void mylib_darray_destroy(mylib_darray *arr)
{
    if (arr == NULL)
        return;
    free(arr->data);
    free(arr);
}

void mylib_darray_destroy_with(mylib_darray *arr, void (*free_fn)(void *))
{
    if (arr == NULL)
        return;
    if (free_fn != NULL) {
        for (mylib_size_t i = 0; i < arr->size; i++) {
            if (arr->data[i] != NULL)
                free_fn(arr->data[i]);
        }
    }
    free(arr->data);
    free(arr);
}

void mylib_darray_clear(mylib_darray *arr)
{
    if (arr == NULL)
        return;
    memset(arr->data, 0, arr->size * sizeof(*arr->data));
    arr->size = 0;
}

void mylib_darray_clear_with(mylib_darray *arr, void (*free_fn)(void *))
{
    if (arr == NULL)
        return;

    if (free_fn != NULL) {
        for (mylib_size_t i = 0; i < arr->size; i++) {
            if (arr->data[i] != NULL)
                free_fn(arr->data[i]);
        }
    }

    memset(arr->data, 0, arr->size * sizeof(*arr->data));
    arr->size = 0;
}

static bool darray_grow(mylib_darray *arr)
{
    mylib_size_t new_capacity = arr->capacity * MYLIB_DARRAY_GROWTH_FACTOR;

    if (new_capacity < arr->capacity)
        return false;

    void **new_data = realloc(arr->data, new_capacity * sizeof(*arr->data));
    if (MYLIB_UNLIKELY(new_data == NULL))
        return false;

    memset(new_data + arr->capacity, 0,
           (new_capacity - arr->capacity) * sizeof(*new_data));

    arr->data = new_data;
    arr->capacity = new_capacity;

    return true;
}

bool mylib_darray_push(mylib_darray *arr, void *value)
{
    if (MYLIB_UNLIKELY(arr == NULL))
        return false;

    if (arr->size >= arr->capacity) {
        if (!darray_grow(arr))
            return false;
    }

    arr->data[arr->size++] = value;
    return true;
}

void *mylib_darray_pop(mylib_darray *arr)
{
    if (MYLIB_UNLIKELY(arr == NULL || arr->size == 0))
        return NULL;

    return arr->data[--arr->size];
}

void *mylib_darray_peek(const mylib_darray *arr)
{
    if (MYLIB_UNLIKELY(arr == NULL || arr->size == 0))
        return NULL;

    return arr->data[arr->size - 1];
}

bool mylib_darray_set(mylib_darray *arr, mylib_size_t index, void *value)
{
    if (MYLIB_UNLIKELY(arr == NULL || index >= arr->size))
        return false;

    arr->data[index] = value;
    return true;
}

void *mylib_darray_get(const mylib_darray *arr, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(arr == NULL || index >= arr->size))
        return NULL;

    return arr->data[index];
}

void *mylib_darray_take(mylib_darray *arr, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(arr == NULL || index >= arr->size))
        return NULL;

    void *value = arr->data[index];
    arr->data[index] = NULL;
    return value;
}

bool mylib_darray_insert(mylib_darray *arr, mylib_size_t index, void *value)
{
    if (MYLIB_UNLIKELY(arr == NULL || index > arr->size))
        return false;

    if (arr->size >= arr->capacity) {
        if (!darray_grow(arr))
            return false;
    }

    if (index < arr->size) {
        memmove(arr->data + index + 1, arr->data + index,
                (arr->size - index) * sizeof(*arr->data));
    }

    arr->data[index] = value;
    arr->size++;
    return true;
}

void *mylib_darray_remove(mylib_darray *arr, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(arr == NULL || index >= arr->size))
        return NULL;

    void *value = arr->data[index];

    if (index < arr->size - 1) {
        memmove(arr->data + index, arr->data + index + 1,
                (arr->size - index - 1) * sizeof(*arr->data));
    }

    arr->size--;
    arr->data[arr->size] = NULL;

    return value;
}

bool mylib_darray_erase(mylib_darray *arr, mylib_size_t index, mylib_size_t count)
{
    if (MYLIB_UNLIKELY(arr == NULL || index >= arr->size))
        return false;

    if (index + count > arr->size)
        count = arr->size - index;

    if (index + count < arr->size) {
        memmove(arr->data + index, arr->data + index + count,
                (arr->size - index - count) * sizeof(*arr->data));
    }

    memset(arr->data + arr->size - count, 0, count * sizeof(*arr->data));
    arr->size -= count;

    return true;
}

mylib_size_t mylib_darray_size(const mylib_darray *arr)
{
    return arr != NULL ? arr->size : 0;
}

mylib_size_t mylib_darray_capacity(const mylib_darray *arr)
{
    return arr != NULL ? arr->capacity : 0;
}

bool mylib_darray_is_empty(const mylib_darray *arr)
{
    return arr == NULL || arr->size == 0;
}

bool mylib_darray_reserve(mylib_darray *arr, mylib_size_t capacity)
{
    if (MYLIB_UNLIKELY(arr == NULL))
        return false;

    if (capacity <= arr->capacity)
        return true;

    void **new_data = realloc(arr->data, capacity * sizeof(*arr->data));
    if (MYLIB_UNLIKELY(new_data == NULL))
        return false;

    memset(new_data + arr->capacity, 0,
           (capacity - arr->capacity) * sizeof(*new_data));

    arr->data = new_data;
    arr->capacity = capacity;

    return true;
}

void mylib_darray_shrink_to_fit(mylib_darray *arr)
{
    if (MYLIB_UNLIKELY(arr == NULL || arr->size == arr->capacity))
        return;

    void **new_data = realloc(arr->data, arr->size * sizeof(*arr->data));
    if (MYLIB_UNLIKELY(new_data == NULL))
        return;

    arr->data = new_data;
    arr->capacity = arr->size;
}

void mylib_darray_sort(mylib_darray *arr, mylib_darray_cmp cmp)
{
    if (MYLIB_UNLIKELY(arr == NULL || cmp == NULL || arr->size <= 1))
        return;

    qsort(arr->data, arr->size, sizeof(*arr->data), cmp);
}
