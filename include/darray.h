#ifndef C_LIB_DARRAY_H
#define C_LIB_DARRAY_H

#include "base.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>

#define C_LIB_DARRAY_DEFAULT_CAPACITY 16
#define C_LIB_DARRAY_GROWTH_FACTOR 2

typedef struct c_lib_darray {
    void **data;
    c_lib_size_t size;
    c_lib_size_t capacity;
} c_lib_darray;

c_lib_darray *c_lib_darray_create(c_lib_size_t initial_capacity);
void c_lib_darray_destroy(c_lib_darray *arr);
void c_lib_darray_destroy_with(c_lib_darray *arr, void (*free_fn)(void *));
void c_lib_darray_clear(c_lib_darray *arr);
void c_lib_darray_clear_with(c_lib_darray *arr, void (*free_fn)(void *));

bool c_lib_darray_push(c_lib_darray *arr, void *value);
void *c_lib_darray_pop(c_lib_darray *arr);
void *c_lib_darray_peek(const c_lib_darray *arr);

bool c_lib_darray_set(c_lib_darray *arr, c_lib_size_t index, void *value);
void *c_lib_darray_get(const c_lib_darray *arr, c_lib_size_t index);
void *c_lib_darray_take(c_lib_darray *arr, c_lib_size_t index);

bool c_lib_darray_insert(c_lib_darray *arr, c_lib_size_t index, void *value);
void *c_lib_darray_remove(c_lib_darray *arr, c_lib_size_t index);
bool c_lib_darray_erase(c_lib_darray *arr, c_lib_size_t index, c_lib_size_t count);

c_lib_size_t c_lib_darray_size(const c_lib_darray *arr);
c_lib_size_t c_lib_darray_capacity(const c_lib_darray *arr);
bool c_lib_darray_is_empty(const c_lib_darray *arr);
bool c_lib_darray_reserve(c_lib_darray *arr, c_lib_size_t capacity);
void c_lib_darray_shrink_to_fit(c_lib_darray *arr);

typedef int (*c_lib_darray_cmp)(const void *a, const void *b);
void c_lib_darray_sort(c_lib_darray *arr, c_lib_darray_cmp cmp);

#define c_lib_darray_foreach(arr, var) \
    for (c_lib_size_t c_lib_concat(_i_, __LINE__) = 0; \
         c_lib_concat(_i_, __LINE__) < c_lib_darray_size(arr) && ((var) = c_lib_darray_get(arr, c_lib_concat(_i_, __LINE__)), 1); \
         c_lib_concat(_i_, __LINE__)++)

#define c_lib_darray_int c_lib_TYPED_DARRAY(int)
#define c_lib_darray_int_create(capacity) c_lib_darray_create(capacity)
#define c_lib_darray_int_push(arr, val) c_lib_darray_push(arr, &(int){val})
#define c_lib_darray_int_pop(arr) (*(int *)c_lib_darray_pop(arr))
#define c_lib_darray_int_get(arr, idx) (*(int *)c_lib_darray_get(arr, idx))
#define c_lib_darray_int_set(arr, idx, val) c_lib_darray_set(arr, idx, &(int){val})

#define c_lib_darray_size_t c_lib_TYPED_DARRAY(size_t)
#define c_lib_darray_size_t_create(capacity) c_lib_darray_create(capacity)
#define c_lib_darray_size_t_push(arr, val) c_lib_darray_push(arr, &(size_t){val})
#define c_lib_darray_size_t_pop(arr) (*(size_t *)c_lib_darray_pop(arr))
#define c_lib_darray_size_t_get(arr, idx) (*(size_t *)c_lib_darray_get(arr, idx))
#define c_lib_darray_size_t_set(arr, idx, val) c_lib_darray_set(arr, idx, &(size_t){val})

#define c_lib_darray_double c_lib_TYPED_DARRAY(double)
#define c_lib_darray_double_create(capacity) c_lib_darray_create(capacity)
#define c_lib_darray_double_push(arr, val) c_lib_darray_push(arr, &(double){val})
#define c_lib_darray_double_pop(arr) (*(double *)c_lib_darray_pop(arr))
#define c_lib_darray_double_get(arr, idx) (*(double *)c_lib_darray_get(arr, idx))
#define c_lib_darray_double_set(arr, idx, val) c_lib_darray_set(arr, idx, &(double){val})

#define c_lib_darray_char c_lib_TYPED_DARRAY(char)
#define c_lib_darray_char_create(capacity) c_lib_darray_create(capacity)
#define c_lib_darray_char_push(arr, val) c_lib_darray_push(arr, &(char){val})
#define c_lib_darray_char_pop(arr) (*(char *)c_lib_darray_pop(arr))
#define c_lib_darray_char_get(arr, idx) (*(char *)c_lib_darray_get(arr, idx))
#define c_lib_darray_char_set(arr, idx, val) c_lib_darray_set(arr, idx, &(char){val})

#define c_lib_darray_void_ptr c_lib_TYPED_DARRAY(void_ptr)
#define c_lib_darray_void_ptr_create(capacity) c_lib_darray_create(capacity)
#define c_lib_darray_void_ptr_push(arr, val) c_lib_darray_push(arr, (void *)(val))
#define c_lib_darray_void_ptr_pop(arr) ((void *)c_lib_darray_pop(arr))
#define c_lib_darray_void_ptr_get(arr, idx) ((void *)c_lib_darray_get(arr, idx))
#define c_lib_darray_void_ptr_set(arr, idx, val) c_lib_darray_set(arr, idx, (void *)(val))

#if defined(__GNUC__) || defined(__clang__)
#define c_lib_darray_new(arr, type) \
    _Generic((arr), \
        c_lib_darray *: c_lib_concat(c_lib_darray_, type##_new)(arr))

#define c_lib_darray_int_new(arr) \
    ((int *)c_lib_darray_peek(arr) ? *(int *)c_lib_darray_peek(arr) : 0)
#endif

#define c_lib_darrayyped(T) struct { \
    T *data; \
    c_lib_size_t size; \
    c_lib_size_t capacity; \
}

#define c_lib_DARRAY(T) c_lib_darray_##T

#define c_lib_DARRAY_CREATE(T, cap) \
    _Generic((T *)0, \
        int: c_lib_darray_int_create(cap), \
        size_t: c_lib_darray_size_t_create(cap), \
        double: c_lib_darray_double_create(cap), \
        char: c_lib_darray_char_create(cap))

#define c_lib_DARRAY_PUSH(arr, val) \
    _Generic((arr), \
        c_lib_darray *: c_lib_darray_push(arr, &(typeof(val)){val}))

#define c_lib_DARRAY_GET(arr, idx) \
    (*(typeof(arr) _Generic((arr), \
        c_lib_darray *: &c_lib_darray_int_get(arr, idx)))[0])

#endif