#ifndef C_LIB_TYPES_H
#define C_LIB_TYPES_H

#include "base.h"

#define C_LIB_TYPED_DARRAY(T) C_LIB_CAT(c_lib_darray_, T)
#define C_LIB_TYPED_DARRAY_CREATE(T) C_LIB_CAT(c_lib_darray_, T ## _create)
#define C_LIB_TYPED_DARRAY_DESTROY(T) C_LIB_CAT(c_lib_darray_, T ## _destroy)
#define C_LIB_TYPED_DARRAY_PUSH(T) C_LIB_CAT(c_lib_darray_, T ## _push)
#define C_LIB_TYPED_DARRAY_POP(T) C_LIB_CAT(c_lib_darray_, T ## _pop)
#define C_LIB_TYPED_DARRAY_GET(T) C_LIB_CAT(c_lib_darray_, T ## _get)
#define C_LIB_TYPED_DARRAY_SET(T) C_LIB_CAT(c_lib_darray_, T ## _set)

#define C_LIB_TYPED_LIST(T) C_LIB_CAT(c_lib_list_, T)
#define C_LIB_TYPED_LIST_CREATE(T) C_LIB_CAT(c_lib_list_, T ## _create)
#define C_LIB_TYPED_LIST_DESTROY(T) C_LIB_CAT(c_lib_list_, T ## _destroy)
#define C_LIB_TYPED_LIST_PUSH(T) C_LIB_CAT(c_lib_list_, T ## _push)
#define C_LIB_TYPED_LIST_POP(T) C_LIB_CAT(c_lib_list_, T ## _pop)
#define C_LIB_TYPED_LIST_GET(T) C_LIB_CAT(c_lib_list_, T ## _get)

#define C_LIB_TYPED_HASHMAP(K, V) C_LIB_CAT3(c_lib_hashmap_, K, _ ## V)
#define C_LIB_TYPED_HASHMAP_CREATE(K, V) C_LIB_CAT3(c_lib_hashmap_, K, _ ## V ## _create)
#define C_LIB_TYPED_HASHMAP_PUT(K, V) C_LIB_CAT3(c_lib_hashmap_, K, _ ## V ## _put)
#define C_LIB_TYPED_HASHMAP_GET(K, V) C_LIB_CAT3(c_lib_hashmap_, K, _ ## V ## _get)

#define C_LIB_TYPED_RBTREE(T) C_LIB_CAT(c_lib_rbtree_, T)
#define C_LIB_TYPED_RBTREE_CREATE(T) C_LIB_CAT(c_lib_rbtree_, T ## _create)
#define C_LIB_TYPED_RBTREE_INSERT(T) C_LIB_CAT(c_lib_rbtree_, T ## _insert)
#define C_LIB_TYPED_RBTREE_SEARCH(T) C_LIB_CAT(c_lib_rbtree_, T ## _search)

#define C_LIB_CAT(a, b) C_LIB_CAT_IMPL(a, b)
#define C_LIB_CAT_IMPL(a, b) a ## b

#define C_LIB_CAT3(a, b, c) C_LIB_CAT3_IMPL(a, b, c)
#define C_LIB_CAT3_IMPL(a, b, c) a ## b ## c

#define c_lib_typed(T, ...) _Generic((T), __VA_ARGS__)

#define c_lib_as_ptr(val) ((void *)&(val))
#define c_lib_as_const_ptr(val) ((const void *)&(val))

#define c_lib_deref_as(val, type) (*(type *)(val))

#define c_lib_is_same_type(a, b) __builtin_is_same(typeof(a), typeof(b))

#define c_lib_align_of(type) _Alignof(type)

#define c_lib_size_of(type) sizeof(type)
#define c_lib_size_of_val(expr) sizeof(expr)

#define c_lib_new(T) ((T *)malloc(sizeof(T)))
#define c_lib_new_n(T, n) ((T *)malloc((n) * sizeof(T)))
#define c_lib_new_with(T, ...) ((T *)malloc(sizeof(T)), __VA_ARGS__, (T *)(-1))
#define c_lib_delete(ptr) free(ptr)

#define c_lib_emplace(ptr, T, ...) (*(ptr) = (T)__VA_ARGS__)

#define c_lib_optional(T) struct { bool valid; T value; }

#define c_lib_result(T) struct { c_lib_result status; T value; }

#define c_lib_box(T) struct { T data; }

#define c_lib_unbox(box) ((box).data)

#define c_lib_into_box(val) ((c_lib_box(typeof(val))){ .data = (val) })

#define c_lib_unwrap(opt) ((opt).valid ? (opt).value : (c_lib_unreachable(), (typeof((opt).value))0))

#define c_lib_unwrap_or(opt, fallback) ((opt).valid ? (opt).value : (fallback))

#define c_lib_some(val) ((typeof(val)){ .valid = true, .value = (val) })
#define c_lib_none(T) ((typeof(T)){ .valid = false })

#define c_lib_variant(...) __VA_ARGS__

#define c_lib_match(val, ...) _Generic((val), __VA_ARGS__)

#define c_lib_let(var, expr) __builtin_choose_expr( \
    __builtin_is_same(typeof(var), typeof(expr)), \
    (void)((var) = (expr)), (void)0)

#define c_lib_for(var, from, to) for (typeof(from) var = (from); (var) < (to); (var)++)

#define c_lib_foreach(ptr, arr, len) for (typeof(arr) ptr = (arr); ptr < (arr) + (len); ptr++)

#define c_lib_zip(a, b, len) for (size_t i = 0; i < (len); i++)

#if C_LIB_HAVE_C23
#define c_lib_any(T, ...) _Any(T, __VA_ARGS__)
#define c_lib_all(T, ...) _All(T, __VA_ARGS__)

#define c_lib_autotype(expr) typeof(expr)
#define c_lib_decltype(expr) typeof(expr)

#define c_lib_constexpr if (0) {} else

#define c_lib_static_assert_array(arr, n) static_assert(sizeof(arr) == (n) * sizeof((arr)[0]), "array size mismatch")
#endif

typedef struct c_lib_pair {
    void *first;
    void *second;
} c_lib_pair;

typedef struct c_lib_triple {
    void *first;
    void *second;
    void *third;
} c_lib_triple;

#define c_lib_pair_new(a, b) ((c_lib_pair){ .first = (void *)(a), .second = (void *)(b) })
#define c_lib_triple_new(a, b, c) ((c_lib_triple){ .first = (void *)(a), .second = (void *)(b), .third = (void *)(c) })

#define c_lib_make_pair(type_a, type_b, a, b) \
    ((c_lib_pair){ .first = (void *)(type_a *)(a), .second = (void *)(type_b *)(b) })

#endif