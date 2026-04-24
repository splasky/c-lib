#ifndef C_LIB_BASE_H
#define C_LIB_BASE_H

#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define C_LIB_VERSION_MAJOR 3
#define C_LIB_VERSION_MINOR 0
#define C_LIB_VERSION_PATCH 0

#if defined(__STDC_VERSION__)
#define C_LIB_C_STANDARD __STDC_VERSION__
#else
#define C_LIB_C_STANDARD 0
#endif

#if C_LIB_C_STANDARD >= 202311L
#define C_LIB_HAVE_C23 1
#else
#define C_LIB_HAVE_C23 0
#endif

#if C_LIB_HAVE_C23
#define C_LIB_NORETURN [[noreturn]]
#define C_LIB_MAYBE_UNUSED [[maybe_unused]]
#define C_LIB_ALIGNED(x) [[align(x)]]
#define C_LIB_DEPRECATED [[deprecated]]
#else
#define C_LIB_NORETURN _Noreturn
#define C_LIB_MAYBE_UNUSED __attribute__((__unused__))
#define C_LIB_DEPRECATED __attribute__((__deprecated__))
#if defined(__GNUC__)
#define C_LIB_ALIGNED(x) __attribute__((__aligned__(x)))
#else
#define C_LIB_ALIGNED(x)
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define C_LIB_LIKELY(x) __builtin_expect(!!(x), 1)
#define C_LIB_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define C_LIB_UNUSED __attribute__((__unused__))
#define C_LIB_INLINE static inline
#define C_LIB_HOT __attribute__((__hot__))
#define C_LIB_COLD __attribute__((__cold__))
#define C_LIB_MUST_CHECK __attribute__((__warn_unused_result__))
#else
#define C_LIB_LIKELY(x) (x)
#define C_LIB_UNLIKELY(x) (x)
#define C_LIB_UNUSED
#define C_LIB_INLINE static inline
#define C_LIB_HOT
#define C_LIB_COLD
#define C_LIB_MUST_CHECK
#endif

#if defined(__GNUC__)
#define C_LIB_BARRIER() __builtin_barrier()
#define C_LIB_PREFETCH_RW(addr) __builtin_prefetch((addr), 1)
#define C_LIB_PREFETCH_RO(addr) __builtin_prefetch((addr), 0)
#else
#define C_LIB_BARRIER() do {} while (0)
#define C_LIB_PREFETCH_RW(addr) do {} while (0)
#define C_LIB_PREFETCH_RO(addr) do {} while (0)
#endif

#define c_lib_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define c_lib_list_entry(ptr, type, member) c_lib_container_of(ptr, type, member)

#if C_LIB_HAVE_C23
#if defined(__STDC_ALLOC_H)
#include <stdckdint.h>
#endif
#endif
typedef size_t c_lib_size_t;
typedef ssize_t c_lib_ssize_t;

typedef enum c_lib_result {
    C_LIB_SUCCESS = 0,
    C_LIB_ENOMEM = -1,
    C_LIB_EINVAL = -2,
    C_LIB_ENOTFOUND = -3,
    C_LIB_EBOUNDS = -4,
} c_lib_result;

C_LIB_INLINE c_lib_result c_lib_result_from_bool(bool ok)
{
    return ok ? C_LIB_SUCCESS : C_LIB_ENOMEM;
}

#if defined(__GNUC__) || defined(__clang__)
#define c_lib_unreachable() __builtin_unreachable()
#else
#define c_lib_unreachable() do {} while (1)
#endif

C_LIB_INLINE bool c_lib_add_overflow(c_lib_size_t a, c_lib_size_t b, c_lib_size_t *res)
{
    c_lib_size_t sum = a + b;
    *res = sum;
    return sum < a;
}

C_LIB_INLINE bool c_lib_mul_overflow(c_lib_size_t a, c_lib_size_t b, c_lib_size_t *res)
{
    if (a == 0 || b == 0) {
        *res = 0;
        return false;
    }
    if (a > (c_lib_size_t)-1 / b)
        return true;
    *res = a * b;
    return false;
}

#if defined(__GNUC__)
#define c_lib_static_assert _Static_assert
#else
#define c_lib_static_assert(expr, msg) \
    typedef int c_lib_static_assert_##__LINE__[(expr) ? 1 : -1]
#endif

#define c_lib_swap(type, a, b)          \
    do {                                \
        type _tmp = (a);               \
        (a) = (b);                     \
        (b) = _tmp;                    \
    } while (0)

#define c_lib_min(a, b) (((a) < (b)) ? (a) : (b))
#define c_lib_max(a, b) (((a) > (b)) ? (a) : (b))
#define c_lib_clamp(val, min, max) (c_lib_max(c_lib_min(val, max), min))

#define c_lib_array_size(arr) (c_lib_size_t)(sizeof(arr) / sizeof((arr)[0]))

#if C_LIB_HAVE_C23
#define c_lib_defer(...) for (__VA_ARGS__; ; (__VA_ARGS__))
#endif

#define c_lib_concat(a, b) c_lib_concat_impl(a, b)
#define c_lib_concat_impl(a, b) a ## b

#define c_lib_stringify(x) c_lib_stringify_impl(x)
#define c_lib_stringify_impl(x) #x

#define c_lib_typeof __typeof__

typedef enum c_lib_ordering {
    C_LIB_ORDER_LESS = -1,
    C_LIB_ORDER_EQUAL = 0,
    C_LIB_ORDER_GREATER = 1,
} c_lib_ordering;

C_LIB_HOT C_LIB_INLINE void *c_lib_memcpy(void *dest, const void *src, c_lib_size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (c_lib_size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

C_LIB_HOT C_LIB_INLINE void *c_lib_memset(void *s, int c, c_lib_size_t n)
{
    unsigned char *p = (unsigned char *)s;
    unsigned char v = (unsigned char)c;
    for (c_lib_size_t i = 0; i < n; i++)
        p[i] = v;
    return s;
}

C_LIB_HOT C_LIB_INLINE int c_lib_memcmp(const void *s1, const void *s2, c_lib_size_t n)
{
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    for (c_lib_size_t i = 0; i < n; i++) {
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    }
    return 0;
}

C_LIB_HOT C_LIB_INLINE int c_lib_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*(const unsigned char *)s1) - (*(const unsigned char *)s2);
}

C_LIB_HOT C_LIB_INLINE c_lib_size_t c_lib_strlen(const char *s)
{
    c_lib_size_t len = 0;
    while (s[len])
        len++;
    return len;
}

C_LIB_HOT C_LIB_INLINE void *c_lib_memmove(void *dest, const void *src, c_lib_size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d < s) {
        for (c_lib_size_t i = 0; i < n; i++)
            d[i] = s[i];
    } else {
        for (c_lib_size_t i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    return dest;
}

#if C_LIB_HAVE_C23
C_LIB_HOT C_LIB_INLINE void *c_lib_memcpy_fast(void *dest, const void *src, c_lib_size_t n)
{
    return __builtin_memcpy(dest, src, n);
}

C_LIB_HOT C_LIB_INLINE void *c_lib_memmove_fast(void *dest, const void *src, c_lib_size_t n)
{
    return __builtin_memmove(dest, src, n);
}
#endif

#define c_lib_bitcast(dest, src) \
    __builtin_choose_expr(sizeof(*(dest)) == sizeof(unsigned long long), \
        __builtin_memcpy((dest), &(src), sizeof(unsigned long long)), \
        __builtin_memcpy((dest), &(src), sizeof(unsigned long)))

#if C_LIB_HAVE_C23
typedef _BitInt(8) c_lib_i8;
typedef _BitInt(16) c_lib_i16;
typedef _BitInt(32) c_lib_i32;
typedef _BitInt(64) c_lib_i64;
typedef unsigned _BitInt(8) c_lib_u8;
typedef unsigned _BitInt(16) c_lib_u16;
typedef unsigned _BitInt(32) c_lib_u32;
typedef unsigned _BitInt(64) c_lib_u64;
#endif

typedef struct c_lib_slice {
    void *data;
    c_lib_size_t len;
    c_lib_size_t cap;
} c_lib_slice;

typedef struct c_lib_const_slice {
    const void *data;
    c_lib_size_t len;
} c_lib_const_slice;

#define c_lib_slice_len(slice) ((slice)->len)
#define c_lib_slice_cap(slice) ((slice)->cap)
#define c_lib_slice_data(slice, type) ((type *)(slice)->data)

#endif