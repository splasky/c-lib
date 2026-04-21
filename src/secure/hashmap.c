#include "mylib/hashmap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MYLIB_HASHMAP_LOAD_FACTOR 75
#define MYLIB_HASHMAP_MIN_CAPACITY 16
#define MYLIB_HASHMAP_GROWTH_FACTOR 2

typedef struct mylib_hashmap_entry {
    const void *key;
    void *value;
    mylib_size_t keylen;
    uint32_t hash;
    struct mylib_hashmap_entry *next;
    bool occupied;
} mylib_hashmap_entry;

static inline void *mylib_hashmap_key_as_mutable(const void *key)
{
    union { const void *cp; void *p; } u = { .cp = key };
    return u.p;
}

struct mylib_hashmap {
    mylib_hashmap_entry *entries;
    mylib_size_t capacity;
    mylib_size_t count;
    mylib_size_t max_load;
    mylib_hash_fn hash_fn;
    mylib_hashmap_cmp cmp_fn;
};

static mylib_hashmap_entry *entry_create(const void *key, mylib_size_t keylen,
                                         void *value, uint32_t hash)
{
    mylib_hashmap_entry *entry = calloc(1, sizeof(*entry));
    if (MYLIB_UNLIKELY(entry == NULL))
        return NULL;

    entry->key = key;
    entry->value = value;
    entry->keylen = keylen;
    entry->hash = hash;
    entry->occupied = true;
    return entry;
}

mylib_hashmap *mylib_hashmap_create(mylib_size_t initial_capacity)
{
    if (initial_capacity < MYLIB_HASHMAP_MIN_CAPACITY)
        initial_capacity = MYLIB_HASHMAP_MIN_CAPACITY;

    mylib_hashmap *map = calloc(1, sizeof(*map));
    if (MYLIB_UNLIKELY(map == NULL))
        return NULL;

    map->entries = calloc(initial_capacity, sizeof(*map->entries));
    if (MYLIB_UNLIKELY(map->entries == NULL)) {
        free(map);
        return NULL;
    }

    map->capacity = initial_capacity;
    map->max_load = (initial_capacity * MYLIB_HASHMAP_LOAD_FACTOR) / 100;
    map->hash_fn = mylib_hashmap_hash_fnv1a;
    map->cmp_fn = (mylib_hashmap_cmp)memcmp;

    return map;
}

void mylib_hashmap_destroy(mylib_hashmap *map)
{
    if (map == NULL)
        return;
    mylib_hashmap_clear(map);
    free(map->entries);
    free(map);
}

void mylib_hashmap_destroy_with(mylib_hashmap *map, mylib_hashmap_free_fn free_fn)
{
    if (map == NULL)
        return;
    mylib_hashmap_clear_with(map, free_fn);
    free(map->entries);
    free(map);
}

void mylib_hashmap_clear(mylib_hashmap *map)
{
    if (map == NULL)
        return;

    for (mylib_size_t i = 0; i < map->capacity; i++) {
        mylib_hashmap_entry *entry = &map->entries[i];
        if (entry->occupied && entry->next != NULL) {
            mylib_hashmap_entry *curr = entry->next;
            while (curr != NULL) {
                mylib_hashmap_entry *next = curr->next;
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

void mylib_hashmap_clear_with(mylib_hashmap *map, mylib_hashmap_free_fn free_fn)
{
    if (map == NULL)
        return;

    for (mylib_size_t i = 0; i < map->capacity; i++) {
        mylib_hashmap_entry *entry = &map->entries[i];
        if (entry->occupied) {
            if (free_fn != NULL) {
                free_fn(mylib_hashmap_key_as_mutable(entry->key), entry->value);
            }
            mylib_hashmap_entry *curr = entry->next;
            while (curr != NULL) {
                mylib_hashmap_entry *next = curr->next;
                if (free_fn != NULL) {
                    free_fn(mylib_hashmap_key_as_mutable(curr->key), curr->value);
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

static bool hashmap_resize(mylib_hashmap *map, mylib_size_t new_capacity)
{
    mylib_hashmap_entry *new_entries = calloc(new_capacity, sizeof(*new_entries));
    if (MYLIB_UNLIKELY(new_entries == NULL))
        return false;

    for (mylib_size_t i = 0; i < map->capacity; i++) {
        mylib_hashmap_entry *curr = &map->entries[i];
        while (curr != NULL && !curr->occupied)
            curr = curr->next;

        while (curr != NULL && curr->occupied) {
            mylib_size_t idx = curr->hash % new_capacity;
            mylib_hashmap_entry *slot = &new_entries[idx];

            mylib_hashmap_entry *copy = calloc(1, sizeof(*copy));
            if (copy == NULL) {
                for (mylib_size_t j = 0; j < new_capacity; j++)
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

    for (mylib_size_t i = 0; i < map->capacity; i++) {
        mylib_hashmap_entry *curr = map->entries[i].next;
        while (curr != NULL) {
            mylib_hashmap_entry *next = curr->next;
            free(curr);
            curr = next;
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
    map->max_load = (new_capacity * MYLIB_HASHMAP_LOAD_FACTOR) / 100;

    return true;
}

bool mylib_hashmap_put(mylib_hashmap *map, const void *key, mylib_size_t keylen, void *value)
{
    if (MYLIB_UNLIKELY(map == NULL || key == NULL))
        return false;

    if (map->count >= map->max_load) {
        if (!hashmap_resize(map, map->capacity * MYLIB_HASHMAP_GROWTH_FACTOR))
            return false;
    }

    uint32_t hash = map->hash_fn(key, keylen);
    mylib_size_t idx = hash % map->capacity;
    mylib_hashmap_entry *entry = &map->entries[idx];

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

        mylib_hashmap_entry *new_entry = entry_create(key, keylen, value, hash);
        if (MYLIB_UNLIKELY(new_entry == NULL))
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

void *mylib_hashmap_get(const mylib_hashmap *map, const void *key, mylib_size_t keylen)
{
    if (MYLIB_UNLIKELY(map == NULL || key == NULL))
        return NULL;

    uint32_t hash = map->hash_fn(key, keylen);
    mylib_size_t idx = hash % map->capacity;
    const mylib_hashmap_entry *entry = &map->entries[idx];

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

void *mylib_hashmap_remove(mylib_hashmap *map, const void *key, mylib_size_t keylen)
{
    if (MYLIB_UNLIKELY(map == NULL || key == NULL))
        return NULL;

    uint32_t hash = map->hash_fn(key, keylen);
    mylib_size_t idx = hash % map->capacity;
    mylib_hashmap_entry *entry = &map->entries[idx];
    mylib_hashmap_entry *prev = NULL;

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
                mylib_hashmap_entry *next = entry->next;
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

bool mylib_hashmap_contains(const mylib_hashmap *map, const void *key, mylib_size_t keylen)
{
    return mylib_hashmap_get(map, key, keylen) != NULL;
}

mylib_size_t mylib_hashmap_count(const mylib_hashmap *map)
{
    return map != NULL ? map->count : 0;
}

bool mylib_hashmap_is_empty(const mylib_hashmap *map)
{
    return map == NULL || map->count == 0;
}

bool mylib_hashmap_foreach(const mylib_hashmap *map, mylib_hashmap_visit_fn visit, void *userdata)
{
    if (MYLIB_UNLIKELY(map == NULL || visit == NULL))
        return false;

    for (mylib_size_t i = 0; i < map->capacity; i++) {
        const mylib_hashmap_entry *entry = &map->entries[i];
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

uint32_t mylib_hashmap_hash_fnv1a(const void *key, mylib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 2166136261U;

    for (mylib_size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619U;
    }

    return hash;
}

uint32_t mylib_hashmap_hash_djb2(const void *key, mylib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 5381;

    for (mylib_size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

uint32_t mylib_hashmap_hash_sdbm(const void *key, mylib_size_t len)
{
    const unsigned char *data = (const unsigned char *)key;
    uint32_t hash = 0;

    for (mylib_size_t i = 0; i < len; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
