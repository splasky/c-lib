#ifndef MYLIB_HASHMAP_H
#define MYLIB_HASHMAP_H

#include "base.h"
#include <stdint.h>
#include <string.h>

typedef struct mylib_hashmap mylib_hashmap;

typedef uint32_t (*mylib_hash_fn)(const void *key, mylib_size_t keylen);
typedef int (*mylib_hashmap_cmp)(const void *a, const void *b, mylib_size_t len);
typedef void (*mylib_hashmap_free_fn)(void *key, void *value);

mylib_hashmap *mylib_hashmap_create(mylib_size_t initial_capacity);
void mylib_hashmap_destroy(mylib_hashmap *map);
void mylib_hashmap_destroy_with(mylib_hashmap *map, mylib_hashmap_free_fn free_fn);
void mylib_hashmap_clear(mylib_hashmap *map);
void mylib_hashmap_clear_with(mylib_hashmap *map, mylib_hashmap_free_fn free_fn);

bool mylib_hashmap_put(mylib_hashmap *map, const void *key, mylib_size_t keylen, void *value);
void *mylib_hashmap_get(const mylib_hashmap *map, const void *key, mylib_size_t keylen);
void *mylib_hashmap_remove(mylib_hashmap *map, const void *key, mylib_size_t keylen);
bool mylib_hashmap_contains(const mylib_hashmap *map, const void *key, mylib_size_t keylen);

mylib_size_t mylib_hashmap_count(const mylib_hashmap *map);
bool mylib_hashmap_is_empty(const mylib_hashmap *map);

typedef bool (*mylib_hashmap_visit_fn)(const void *key, mylib_size_t keylen, void *value, void *userdata);
bool mylib_hashmap_foreach(const mylib_hashmap *map, mylib_hashmap_visit_fn visit, void *userdata);

uint32_t mylib_hashmap_hash_fnv1a(const void *key, mylib_size_t len);
uint32_t mylib_hashmap_hash_djb2(const void *key, mylib_size_t len);
uint32_t mylib_hashmap_hash_sdbm(const void *key, mylib_size_t len);

#endif
