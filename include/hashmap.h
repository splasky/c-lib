#ifndef C_LIB_HASHMAP_H
#define C_LIB_HASHMAP_H

#include "base.h"
#include <stdint.h>
#include <string.h>

typedef struct c_lib_hashmap c_lib_hashmap;

typedef uint32_t (*c_lib_hash_fn)(const void *key, c_lib_size_t keylen);
typedef int (*c_lib_hashmap_cmp)(const void *a, const void *b, c_lib_size_t len);
typedef void (*c_lib_hashmap_free_fn)(void *key, void *value);

c_lib_hashmap *c_lib_hashmap_create(c_lib_size_t initial_capacity);
void c_lib_hashmap_destroy(c_lib_hashmap *map);
void c_lib_hashmap_destroy_with(c_lib_hashmap *map, c_lib_hashmap_free_fn free_fn);
void c_lib_hashmap_clear(c_lib_hashmap *map);
void c_lib_hashmap_clear_with(c_lib_hashmap *map, c_lib_hashmap_free_fn free_fn);

bool c_lib_hashmap_put(c_lib_hashmap *map, const void *key, c_lib_size_t keylen, void *value);
void *c_lib_hashmap_get(const c_lib_hashmap *map, const void *key, c_lib_size_t keylen);
void *c_lib_hashmap_remove(c_lib_hashmap *map, const void *key, c_lib_size_t keylen);
bool c_lib_hashmap_contains(const c_lib_hashmap *map, const void *key, c_lib_size_t keylen);

c_lib_size_t c_lib_hashmap_count(const c_lib_hashmap *map);
bool c_lib_hashmap_is_empty(const c_lib_hashmap *map);

typedef bool (*c_lib_hashmap_visit_fn)(const void *key, c_lib_size_t keylen, void *value, void *userdata);
bool c_lib_hashmap_foreach(const c_lib_hashmap *map, c_lib_hashmap_visit_fn visit, void *userdata);

uint32_t c_lib_hashmap_hash_fnv1a(const void *key, c_lib_size_t len);
uint32_t c_lib_hashmap_hash_djb2(const void *key, c_lib_size_t len);
uint32_t c_lib_hashmap_hash_sdbm(const void *key, c_lib_size_t len);

#define c_lib_hashmap_int_int c_lib_TYPED_HASHMAP(int, int)
#define c_lib_hashmap_int_int_create(capacity) c_lib_hashmap_create(capacity)
#define c_lib_hashmap_int_int_put(map, key, val) c_lib_hashmap_put(map, &(int){key}, sizeof(int), &(int){val})
#define c_lib_hashmap_int_int_get(map, key) (*(int *)c_lib_hashmap_get(map, &(key), sizeof(int)))

#define c_lib_hashmap_str_int c_lib_TYPED_HASHMAP(str, int)
#define c_lib_hashmap_str_int_create(capacity) c_lib_hashmap_create(capacity)
#define c_lib_hashmap_str_int_put(map, key, val) c_lib_hashmap_put(map, key, c_lib_strlen(key), &(int){val})
#define c_lib_hashmap_str_int_get(map, key) (*(int *)c_lib_hashmap_get(map, key, c_lib_strlen(key)))

#define c_lib_hashmap_str_void_ptr c_lib_TYPED_HASHMAP(str, void_ptr)
#define c_lib_hashmap_str_void_ptr_create(capacity) c_lib_hashmap_create(capacity)
#define c_lib_hashmap_str_void_ptr_put(map, key, val) c_lib_hashmap_put(map, key, c_lib_strlen(key), (void *)(val))
#define c_lib_hashmap_str_void_ptr_get(map, key) ((void *)c_lib_hashmap_get(map, key, c_lib_strlen(key)))

#define c_lib_HASHMAP(K, V) c_lib_concat3(c_lib_hashmap_, K, _ ## V)

#define c_lib_HASHMAP_CREATE(K, V, cap) \
    _Generic((K *)0, \
        char *: c_lib_hashmap_str_##V##_create(cap))

#define c_lib_HASHMAP_PUT(map, key, val) \
    _Generic((key), \
        char *: c_lib_hashmap_put(map, key, c_lib_strlen(key), &(typeof(val)){val}), \
        default: c_lib_hashmap_put(map, &(typeof(key)){key}, sizeof(typeof(key)), &(typeof(val)){val}))

#define c_lib_HASHMAP_GET(map, key) \
    (*(typeof(*(typeof(map))0) *)_Generic((key), \
        char *: c_lib_hashmap_get(map, key, c_lib_strlen(key)), \
        default: c_lib_hashmap_get(map, &(typeof(key)){key}, sizeof(typeof(key)))))

#endif