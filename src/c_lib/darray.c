#include "c_lib/darray.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

c_lib_darray *c_lib_darray_create(c_lib_size_t initial_capacity)
{
    if (initial_capacity == 0)
        initial_capacity = C_LIB_DARRAY_DEFAULT_CAPACITY;

    c_lib_darray *arr = calloc(1, sizeof(*arr));
    if (C_LIB_UNLIKELY(arr == NULL))
        return NULL;

    arr->data = calloc(initial_capacity, sizeof(*arr->data));
    if (C_LIB_UNLIKELY(arr->data == NULL)) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;

    return arr;
}

void c_lib_darray_destroy(c_lib_darray *arr)
{
    if (arr == NULL)
        return;
    free(arr->data);
    free(arr);
}

void c_lib_darray_destroy_with(c_lib_darray *arr, void (*free_fn)(void *))
{
    if (arr == NULL)
        return;
    if (free_fn != NULL) {
        for (c_lib_size_t i = 0; i < arr->size; i++) {
            if (arr->data[i] != NULL)
                free_fn(arr->data[i]);
        }
    }
    free(arr->data);
    free(arr);
}

void c_lib_darray_clear(c_lib_darray *arr)
{
    if (arr == NULL)
        return;
    memset(arr->data, 0, arr->size * sizeof(*arr->data));
    arr->size = 0;
}

void c_lib_darray_clear_with(c_lib_darray *arr, void (*free_fn)(void *))
{
    if (arr == NULL)
        return;

    if (free_fn != NULL) {
        for (c_lib_size_t i = 0; i < arr->size; i++) {
            if (arr->data[i] != NULL)
                free_fn(arr->data[i]);
        }
    }

    memset(arr->data, 0, arr->size * sizeof(*arr->data));
    arr->size = 0;
}

static bool darray_grow(c_lib_darray *arr)
{
    c_lib_size_t new_capacity = arr->capacity * C_LIB_DARRAY_GROWTH_FACTOR;

    if (new_capacity < arr->capacity)
        return false;

    void **new_data = realloc(arr->data, new_capacity * sizeof(*arr->data));
    if (C_LIB_UNLIKELY(new_data == NULL))
        return false;

    memset(new_data + arr->capacity, 0,
           (new_capacity - arr->capacity) * sizeof(*new_data));

    arr->data = new_data;
    arr->capacity = new_capacity;

    return true;
}

bool c_lib_darray_push(c_lib_darray *arr, void *value)
{
    if (C_LIB_UNLIKELY(arr == NULL))
        return false;

    if (arr->size >= arr->capacity) {
        if (!darray_grow(arr))
            return false;
    }

    arr->data[arr->size++] = value;
    return true;
}

void *c_lib_darray_pop(c_lib_darray *arr)
{
    if (C_LIB_UNLIKELY(arr == NULL || arr->size == 0))
        return NULL;

    return arr->data[--arr->size];
}

void *c_lib_darray_peek(const c_lib_darray *arr)
{
    if (C_LIB_UNLIKELY(arr == NULL || arr->size == 0))
        return NULL;

    return arr->data[arr->size - 1];
}

bool c_lib_darray_set(c_lib_darray *arr, c_lib_size_t index, void *value)
{
    if (C_LIB_UNLIKELY(arr == NULL || index >= arr->size))
        return false;

    arr->data[index] = value;
    return true;
}

void *c_lib_darray_get(const c_lib_darray *arr, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(arr == NULL || index >= arr->size))
        return NULL;

    return arr->data[index];
}

void *c_lib_darray_take(c_lib_darray *arr, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(arr == NULL || index >= arr->size))
        return NULL;

    void *value = arr->data[index];
    arr->data[index] = NULL;
    return value;
}

bool c_lib_darray_insert(c_lib_darray *arr, c_lib_size_t index, void *value)
{
    if (C_LIB_UNLIKELY(arr == NULL || index > arr->size))
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

void *c_lib_darray_remove(c_lib_darray *arr, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(arr == NULL || index >= arr->size))
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

bool c_lib_darray_erase(c_lib_darray *arr, c_lib_size_t index, c_lib_size_t count)
{
    if (C_LIB_UNLIKELY(arr == NULL || index >= arr->size))
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

c_lib_size_t c_lib_darray_size(const c_lib_darray *arr)
{
    return arr != NULL ? arr->size : 0;
}

c_lib_size_t c_lib_darray_capacity(const c_lib_darray *arr)
{
    return arr != NULL ? arr->capacity : 0;
}

bool c_lib_darray_is_empty(const c_lib_darray *arr)
{
    return arr == NULL || arr->size == 0;
}

bool c_lib_darray_reserve(c_lib_darray *arr, c_lib_size_t capacity)
{
    if (C_LIB_UNLIKELY(arr == NULL))
        return false;

    if (capacity <= arr->capacity)
        return true;

    void **new_data = realloc(arr->data, capacity * sizeof(*arr->data));
    if (C_LIB_UNLIKELY(new_data == NULL))
        return false;

    memset(new_data + arr->capacity, 0,
           (capacity - arr->capacity) * sizeof(*new_data));

    arr->data = new_data;
    arr->capacity = capacity;

    return true;
}

void c_lib_darray_shrink_to_fit(c_lib_darray *arr)
{
    if (C_LIB_UNLIKELY(arr == NULL || arr->size == arr->capacity))
        return;

    void **new_data = realloc(arr->data, arr->size * sizeof(*arr->data));
    if (C_LIB_UNLIKELY(new_data == NULL))
        return;

    arr->data = new_data;
    arr->capacity = arr->size;
}

void c_lib_darray_sort(c_lib_darray *arr, c_lib_darray_cmp cmp)
{
    if (C_LIB_UNLIKELY(arr == NULL || cmp == NULL || arr->size <= 1))
        return;

    qsort(arr->data, arr->size, sizeof(*arr->data), cmp);
}