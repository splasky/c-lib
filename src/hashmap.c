#include "hashmap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define C_LIB_HASHMAP_LOAD_FACTOR 75
#define C_LIB_HASHMAP_MIN_CAPACITY 16
#define C_LIB_HASHMAP_GROWTH_FACTOR 2

typedef struct c_lib_hashmap_entry {
    const void *key;
    void *value;
    c_lib_size_t keylen;
    uint32_t hash;
    struct c_lib_hashmap_entry *next;
    bool occupied;
} c_lib_hashmap_entry;

static inline void *c_lib_hashmap_key_as_mutable(const void *key)
{
    union { const void *cp; void *p; } u = { .cp = key };
    return u.p;
}

struct c_lib_hashmap {
    c_lib_hashmap_entry *entries;
    c_lib_size_t capacity;
    c_lib_size_t count;
    c_lib_size_t max_load;
    c_lib_hash_fn hash_fn;
    c_lib_hashmap_cmp cmp_fn;
};

static c_lib_hashmap_entry *entry_create(const void *key, c_lib_size_t keylen,
                                         void *value, uint32_t hash)
{
    c_lib_hashmap_entry *entry = calloc(1, sizeof(*entry));
    if (C_LIB_UNLIKELY(entry == NULL))
        return NULL;

    entry->key = key;
    entry->value = value;
    entry->keylen = keylen;
    entry->hash = hash;
    entry->occupied = true;
    return entry;
}

c_lib_hashmap *c_lib_hashmap_create(c_lib_size_t initial_capacity)
{
    if (initial_capacity < C_LIB_HASHMAP_MIN_CAPACITY)
        initial_capacity = C_LIB_HASHMAP_MIN_CAPACITY;

    c_lib_hashmap *map = calloc(1, sizeof(*map));
    if (C_LIB_UNLIKELY(map == NULL))
        return NULL;

    map->entries = calloc(initial_capacity, sizeof(*map->entries));
    if (C_LIB_UNLIKELY(map->entries == NULL)) {
        free(map);
        return NULL;
    }

    map->capacity = initial_capacity;
    map->max_load = (initial_capacity * C_LIB_HASHMAP_LOAD_FACTOR) / 100;
    map->hash_fn = c_lib_hashmap_hash_fnv1a;
    map->cmp_fn = (c_lib_hashmap_cmp)memcmp;

    return map;
}

void c_lib_hashmap_destroy(c_lib_hashmap *map)
{
    if (map == NULL)
        return;
    c_lib_hashmap_clear(map);
    free(map->entries);
    free(map);
}

void c_lib_hashmap_destroy_with(c_lib_hashmap *map, c_lib_hashmap_free_fn free_fn)
{
    if (map == NULL)
        return;
    c_lib_hashmap_clear_with(map, free_fn);
    free(map->entries);
    free(map);
}

void c_lib_hashmap_clear(c_lib_hashmap *map)
{
    if (map == NULL)
        return;

    for (c_lib_size_t i = 0; i < map->capacity; i++) {
        c_lib_hashmap_entry *entry = &map->entries[i];
        if (entry->occupied && entry->next != NULL) {
            c_lib_hashmap_entry *curr = entry->next;
            while (curr != NULL) {
                c_lib_hashmap_entry *next = curr->next;
                free(curr);
                curr = next;
            }
            entry->next = NULL;
        }
        entry->occupied = false;
        entry->key = NULL;
        entry->value = NULL;
        entry->keylen = 0;
        entry->hash = 0;
    }
    map->count = 0;
}

void c_lib_hashmap_clear_with(c_lib_hashmap *map, c_lib_hashmap_free_fn free_fn)
{
    if (map == NULL)
        return;

    for (c_lib_size_t i = 0; i < map->capacity; i++) {
        c_lib_hashmap_entry *entry = &map->entries[i];
        if (entry->occupied) {
            if (free_fn != NULL) {
                free_fn(c_lib_hashmap_key_as_mutable(entry->key), entry->value);
            }
            c_lib_hashmap_entry *curr = entry->next;
            while (curr != NULL) {
                c_lib_hashmap_entry *next = curr->next;
                if (free_fn != NULL) {
                    free_fn(c_lib_hashmap_key_as_mutable(curr->key), curr->value);
                }
                free(curr);
                curr = next;
            }
        }
        entry->occupied = false;
        entry->next = NULL;
        entry->key = NULL;
        entry->value = NULL;
        entry->keylen = 0;
        entry->hash = 0;
    }
    map->count = 0;
}

static bool hashmap_resize(c_lib_hashmap *map, c_lib_size_t new_capacity)
{
    c_lib_hashmap_entry *new_entries = calloc(new_capacity, sizeof(*new_entries));
    if (C_LIB_UNLIKELY(new_entries == NULL))
        return false;

    for (c_lib_size_t i = 0; i < map->capacity; i++) {
        c_lib_hashmap_entry *curr = &map->entries[i];
        while (curr != NULL && !curr->occupied)
            curr = curr->next;

        while (curr != NULL && curr->occupied) {
            c_lib_size_t idx = curr->hash % new_capacity;
            c_lib_hashmap_entry *slot = &new_entries[idx];

            c_lib_hashmap_entry *copy = calloc(1, sizeof(*copy));
            if (copy == NULL) {
                for (c_lib_size_t j = 0; j < new_capacity; j++)
                    free(new_entries[j].next);
                free(new_entries);
                return false;
            }
            *copy = *curr;
            copy->next = NULL;

            if (!slot->occupied) {
                *slot = *copy;
                free(copy);
            } else {
                while (slot->next != NULL)
                    slot = slot->next;
                slot->next = copy;
            }

            curr = curr->next;
        }
    }

    for (c_lib_size_t i = 0; i < map->capacity; i++) {
        c_lib_hashmap_entry *curr = map->entries[i].next;
        while (curr != NULL) {
            c_lib_hashmap_entry *next = curr->next;
            free(curr);
            curr = next;
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
    map->max_load = (new_capacity * C_LIB_HASHMAP_LOAD_FACTOR) / 100;

    return true;
}

bool c_lib_hashmap_put(c_lib_hashmap *map, const void *key, c_lib_size_t keylen, void *value)
{
    if (C_LIB_UNLIKELY(map == NULL || key == NULL))
        return false;

    if (map->count >= map->max_load) {
        if (!hashmap_resize(map, map->capacity * C_LIB_HASHMAP_GROWTH_FACTOR))
            return false;
    }

    uint32_t hash = map->hash_fn(key, keylen);
    c_lib_size_t idx = hash % map->capacity;
    c_lib_hashmap_entry *entry = &map->entries[idx];

    if (entry->occupied) {
        if (entry->hash == hash && entry->keylen == keylen &&
            map->cmp_fn(entry->key, key, keylen) == 0) {
            entry->value = value;
            return true;
        }

        while (entry->next != NULL) {
            entry = entry->next;
            if (entry->hash == hash && entry->keylen == keylen &&
                map->cmp_fn(entry->key, key, keylen) == 0) {
                entry->value = value;
                return true;
            }
        }

        c_lib_hashmap_entry *new_entry = entry_create(key, keylen, value, hash);
        if (C_LIB_UNLIKELY(new_entry == NULL))
            return false;
        entry->next = new_entry;
    } else {
        entry->key = key;
        entry->value = value;
        entry->keylen = keylen;
        entry->hash = hash;
        entry->occupied = true;
    }

    map->count++;
    return true;
}

void *c_lib_hashmap_get(const c_lib_hashmap *map, const void *key, c_lib_size_t keylen)
{
    if (C_LIB_UNLIKELY(map == NULL || key == NULL))
        return NULL;

    uint32_t hash = map->hash_fn(key, keylen);
    c_lib_size_t idx = hash % map->capacity;
    const c_lib_hashmap_entry *entry = &map->entries[idx];

    while (entry != NULL) {
        if (entry->occupied &&
            entry->hash == hash &&
            entry->keylen == keylen &&
            map->cmp_fn(entry->key, key, keylen) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

void *c_lib_hashmap_remove(c_lib_hashmap *map, const void *key, c_lib_size_t keylen)
{
    if (C_LIB_UNLIKELY(map == NULL || key == NULL))
        return NULL;

    uint32_t hash = map->hash_fn(key, keylen);
    c_lib_size_t idx = hash % map->capacity;
    c_lib_hashmap_entry *entry = &map->entries[idx];
    c_lib_hashmap_entry *prev = NULL;

    while (entry != NULL) {
        if (entry->occupied &&
            entry->hash == hash &&
            entry->keylen == keylen &&
            map->cmp_fn(entry->key, key, keylen) == 0) {

            void *value = entry->value;

            if (prev != NULL) {
                prev->next = entry->next;
                free(entry);
            } else if (entry->next != NULL) {
                c_lib_hashmap_entry *next = entry->next;
                *entry = *next;
                free(next);
            } else {
                entry->occupied = false;
                entry->key = NULL;
                entry->value = NULL;
                entry->keylen = 0;
                entry->hash = 0;
            }

            map->count--;
            return value;
        }
        prev = entry;
        entry = entry->next;
    }

    return NULL;
}

bool c_lib_hashmap_contains(const c_lib_hashmap *map, const void *key, c_lib_size_t keylen)
{
    return c_lib_hashmap_get(map, key, keylen) != NULL;
}

c_lib_size_t c_lib_hashmap_count(const c_lib_hashmap *map)
{
    return map != NULL ? map->count : 0;
}

bool c_lib_hashmap_is_empty(const c_lib_hashmap *map)
{
    return map == NULL || map->count == 0;
}

bool c_lib_hashmap_foreach(const c_lib_hashmap *map, c_lib_hashmap_visit_fn visit, void *userdata)
{
    if (C_LIB_UNLIKELY(map == NULL || visit == NULL))
        return false;

    for (c_lib_size_t i = 0; i < map->capacity; i++) {
        const c_lib_hashmap_entry *entry = &map->entries[i];
        while (entry != NULL) {
            if (entry->occupied) {
                if (!visit(entry->key, entry->keylen, entry->value, userdata))
                    return false;
            }
            entry = entry->next;
        }
    }
    return true;
}

uint32_t c_lib_hashmap_hash_fnv1a(const void *key, c_lib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 2166136261U;

    for (c_lib_size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619U;
    }

    return hash;
}

uint32_t c_lib_hashmap_hash_djb2(const void *key, c_lib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 5381;

    for (c_lib_size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

uint32_t c_lib_hashmap_hash_sdbm(const void *key, c_lib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 0;

    for (c_lib_size_t i = 0; i < len; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}