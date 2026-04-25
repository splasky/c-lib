#include "../include/trie.h"
#include "../include/dbg.h"
#include <stdlib.h>

#define BITS_PER_BYTE (8)
#define GET_BIT(x, n)                                                                    \
    ((((x)[(n) / BITS_PER_BYTE]) & (0x1 << (BITS_PER_BYTE - 1 - (n) % BITS_PER_BYTE)))   \
        != 0)

#define TRIE_BASE (2)

struct trie_node {
    char* key;
    struct trie_node* kids[TRIE_BASE];
};

#define IsLeaf(t) ((t)->kids[0] == 0 && (t)->kids[1] == 0)

int trie_contains(Trie* trie, const char* target)
{

    for (int bit = 0; trie && !IsLeaf(trie); bit++) {
        trie = trie->kids[GET_BIT(target, bit)];
    }

    if (trie == 0) {
        return 0;
    } else {
        return !strcmp(trie->key, target);
    }
}

static char* trie_strdup(const char* s)
{
    char* s2;

    s2 = malloc(strlen(s) + 1);
    check_mem(s2);
    strcpy(s2, s);
    return s2;
error:
    return NULL;
}

static Trie* make_trie_node(const char* key)
{
    Trie* trie;

    trie = malloc(sizeof(Trie));

    if (key) {
        trie->key = trie_strdup(key);
        check_mem(trie->key);
    } else {
        trie->key = 0;
    }

    for (int i = 0; i < TRIE_BASE; i++) {
        trie->kids[i] = 0;
    }

    return trie;
error:
    return NULL;
}

/* add a new key to a trie
 * and return the new trie
 */
Trie* trie_insert(Trie* trie, const char* key)
{
    int bit;
    int bit_value;

    Trie* t;
    Trie* kid;
    const char* old_key;

    if (trie == 0) {
        return make_trie_node(key);
    }

    for (t = trie, bit = 0; !IsLeaf(t); bit++, t = kid) {
        kid = t->kids[bit_value = GET_BIT(key, bit)];
        if (kid == 0) {
            t->kids[bit_value] = make_trie_node(key);
            return trie;
        }
    }

    if (!strcmp(t->key, key)) {
        return trie;
    }

    /* have to extend the trie */
    old_key = t->key;
    t->key = 0;

    /* walk the common prefix */
    while (GET_BIT(old_key, bit) == (bit_value = GET_BIT(key, bit))) {
        kid = make_trie_node(0);
        t->kids[bit_value] = kid;
        bit++;
        t = kid;
    }

    /* split */
    t->kids[bit_value] = make_trie_node(key);
    t->kids[!bit_value] = make_trie_node(old_key);
    free((char*)old_key);
    return trie;
}

void trie_destroy(Trie* trie)
{

    if (trie) {
        for (int i = 0; i < TRIE_BASE; ++i) {
            trie_destroy(trie->kids[i]);
        }

        if (IsLeaf(trie)) {
            free(trie->key);
        }

        free(trie);
    }
}

static void trie_print_internal(Trie* t, int bit)
{

    if (t) {
        if (IsLeaf(t)) {
            for (int i = 0; i < bit; ++i) {
                printf(" ");
            }
            printf("%s\n", t->key);
        } else {
            for (int kid = 0; kid < TRIE_BASE; kid++) {
                trie_print_internal(t->kids[kid], bit + 1);
            }
        }
    }
}

void trie_print(Trie* t) { trie_print_internal(t, 0); }
