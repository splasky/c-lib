#ifndef MYLIB_BASE_H
#define MYLIB_BASE_H

#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MYLIB_VERSION_MAJOR 2
#define MYLIB_VERSION_MINOR 0
#define MYLIB_VERSION_PATCH 0

#if defined(__STDC_VERSION__)
#define MYLIB_C_STANDARD __STDC_VERSION__
#else
#define MYLIB_C_STANDARD 0
#endif

#if MYLIB_C_STANDARD >= 202311L
#define MYLIB_HAVE_C23 1
#else
#define MYLIB_HAVE_C23 0
#endif

#if MYLIB_HAVE_C23
#define MYLIB_NORETURN [[noreturn]]
#define MYLIB_MAYBE_UNUSED [[maybe_unused]]
#define MYLIB_ALIGNED(x) [[align(x)]]
#else
#define MYLIB_NORETURN _Noreturn
#define MYLIB_MAYBE_UNUSED __attribute__((__unused__))
#if defined(__GNUC__)
#define MYLIB_ALIGNED(x) __attribute__((__aligned__(x)))
#else
#define MYLIB_ALIGNED(x)
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MYLIB_LIKELY(x) __builtin_expect(!!(x), 1)
#define MYLIB_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define MYLIB_UNUSED __attribute__((__unused__))
#define MYLIB_INLINE static inline
#define MYLIB_HOT __attribute__((__hot__))
#define MYLIB_COLD __attribute__((__cold__))
#else
#define MYLIB_LIKELY(x) (x)
#define MYLIB_UNLIKELY(x) (x)
#define MYLIB_UNUSED
#define MYLIB_INLINE static inline
#define MYLIB_HOT
#define MYLIB_COLD
#endif

#if defined(__GNUC__)
#define MYLIB_BARRIER() __builtin_barrier()
#define MYLIB_PREFETCH_RW(addr) __builtin_prefetch((addr), 1)
#define MYLIB_PREFETCH_RO(addr) __builtin_prefetch((addr), 0)
#else
#define MYLIB_BARRIER() do {} while (0)
#define MYLIB_PREFETCH_RW(addr) do {} while (0)
#define MYLIB_PREFETCH_RO(addr) do {} while (0)
#endif

#define MYLIB_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define MYLIB_LIST_ENTRY(ptr, type, member) MYLIB_CONTAINER_OF(ptr, type, member)

#if MYLIB_HAVE_C23
#if defined(__STDC_ALLOC_H)
#include <stdckdint.h>
#endif
typedef size_t mylib_size_t;
typedef ssize_t mylib_ssize_t;
#else
typedef size_t mylib_size_t;
typedef ptrdiff_t mylib_ssize_t;
#endif

typedef enum mylib_result {
    MYLIB_SUCCESS = 0,
    MYLIB_ENOMEM = -1,
    MYLIB_EINVAL = -2,
    MYLIB_ENOTFOUND = -3,
    MYLIB_EBOUNDS = -4,
} mylib_result;

MYLIB_INLINE mylib_result mylib_result_from_bool(bool ok)
{
    return ok ? MYLIB_SUCCESS : MYLIB_ENOMEM;
}

#if defined(__GNUC__) || defined(__clang__)
#define mylib_unreachable() __builtin_unreachable()
#else
#define mylib_unreachable() do {} while (1)
#endif

MYLIB_INLINE bool mylib_add_overflow(mylib_size_t a, mylib_size_t b, mylib_size_t *res)
{
    mylib_size_t sum = a + b;
    *res = sum;
    return sum < a;
}

MYLIB_INLINE bool mylib_mul_overflow(mylib_size_t a, mylib_size_t b, mylib_size_t *res)
{
    if (a == 0 || b == 0) {
        *res = 0;
        return false;
    }
    if (a > (mylib_size_t)-1 / b)
        return true;
    *res = a * b;
    return false;
}

#if defined(__GNUC__)
#define mylib_static_assert _Static_assert
#else
#define mylib_static_assert(expr, msg) \
    typedef int mylib_static_assert_##__LINE__[(expr) ? 1 : -1]
#endif

MYLIB_HOT MYLIB_INLINE void *mylib_memcpy(void *dest, const void *src, mylib_size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (mylib_size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

MYLIB_HOT MYLIB_INLINE void *mylib_memset(void *s, int c, mylib_size_t n)
{
    unsigned char *p = (unsigned char *)s;
    unsigned char v = (unsigned char)c;
    for (mylib_size_t i = 0; i < n; i++)
        p[i] = v;
    return s;
}

MYLIB_HOT MYLIB_INLINE int mylib_memcmp(const void *s1, const void *s2, mylib_size_t n)
{
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    for (mylib_size_t i = 0; i < n; i++) {
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    }
    return 0;
}

MYLIB_HOT MYLIB_INLINE int mylib_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*(const unsigned char *)s1) - (*(const unsigned char *)s2);
}

MYLIB_HOT MYLIB_INLINE mylib_size_t mylib_strlen(const char *s)
{
    mylib_size_t len = 0;
    while (s[len])
        len++;
    return len;
}

MYLIB_HOT MYLIB_INLINE void *mylib_memmove(void *dest, const void *src, mylib_size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d < s) {
        for (mylib_size_t i = 0; i < n; i++)
            d[i] = s[i];
    } else {
        for (mylib_size_t i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    return dest;
}

#define MYLIB_SWAP(type, a, b)          \
    do {                                \
        type _tmp = (a);               \
        (a) = (b);                     \
        (b) = _tmp;                    \
    } while (0)

#define MYLIB_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MYLIB_MAX(a, b) (((a) > (b)) ? (a) : (b))

#if MYLIB_HAVE_C23
#define MYLIB_ARRAY_SIZE(arr) (mylib_size_t)(sizeof(arr) / sizeof((arr)[0]))
#else
#define MYLIB_ARRAY_SIZE(arr) sizeof(arr) / sizeof((arr)[0])
#endif

#endif
