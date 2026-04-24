#ifndef C_LIB_RBTREE_H
#define C_LIB_RBTREE_H

#include "base.h"

typedef enum c_lib_rbtree_color {
    C_LIB_RBTREE_BLACK = 0,
    C_LIB_RBTREE_RED = 1,
} c_lib_rbtree_color;

typedef struct c_lib_rbtree_node {
    struct c_lib_rbtree_node *left;
    struct c_lib_rbtree_node *right;
    struct c_lib_rbtree_node *parent;
    void *key;
    void *value;
    c_lib_rbtree_color color;
} c_lib_rbtree_node;

typedef struct c_lib_rbtree {
    c_lib_rbtree_node *root;
    c_lib_size_t count;
    int (*cmp)(const void *a, const void *b);
} c_lib_rbtree;

typedef int (*c_lib_rbtree_key_cmp)(const void *a, const void *b);

c_lib_rbtree *c_lib_rbtree_create(c_lib_rbtree_key_cmp cmp);
void c_lib_rbtree_destroy(c_lib_rbtree *tree);
void c_lib_rbtree_clear(c_lib_rbtree *tree);
bool c_lib_rbtree_insert(c_lib_rbtree *tree, void *key, void *value);
bool c_lib_rbtree_delete(c_lib_rbtree *tree, const void *key);
void *c_lib_rbtree_search(const c_lib_rbtree *tree, const void *key);
bool c_lib_rbtree_update(c_lib_rbtree *tree, const void *key, void *value);
c_lib_rbtree_node *c_lib_rbtree_min(c_lib_rbtree_node *node);
c_lib_rbtree_node *c_lib_rbtree_max(c_lib_rbtree_node *node);
c_lib_size_t c_lib_rbtree_count(const c_lib_rbtree *tree);
bool c_lib_rbtree_is_empty(const c_lib_rbtree *tree);

typedef void (*c_lib_rbtree_visit_fn)(const c_lib_rbtree_node *node);
void c_lib_rbtree_inorder(const c_lib_rbtree *tree, c_lib_rbtree_visit_fn visit);
void c_lib_rbtree_preorder(const c_lib_rbtree *tree, c_lib_rbtree_visit_fn visit);
void c_lib_rbtree_postorder(const c_lib_rbtree *tree, c_lib_rbtree_visit_fn visit);

c_lib_rbtree_node *c_lib_rbtree_lower_bound(const c_lib_rbtree *tree, const void *key);
c_lib_rbtree_node *c_lib_rbtree_upper_bound(const c_lib_rbtree *tree, const void *key);

#define c_lib_rbtree_int c_lib_TYPED_RBTREE(int)
#define c_lib_rbtree_int_create(cmp) c_lib_rbtree_create(cmp)
#define c_lib_rbtree_int_insert(tree, key, val) c_lib_rbtree_insert(tree, &(int){key}, &(int){val})
#define c_lib_rbtree_int_search(tree, key) (*(int *)c_lib_rbtree_search(tree, &(key)))

#define c_lib_rbtree_size_t c_lib_TYPED_RBTREE(size_t)
#define c_lib_rbtree_size_t_create(cmp) c_lib_rbtree_create(cmp)
#define c_lib_rbtree_size_t_insert(tree, key, val) c_lib_rbtree_insert(tree, &(size_t){key}, &(size_t){val})
#define c_lib_rbtree_size_t_search(tree, key) (*(size_t *)c_lib_rbtree_search(tree, &(key)))

#define c_lib_rbtree_str_int c_lib_TYPED_RBTREE(str_int)
#define c_lib_rbtree_str_int_create(cmp) c_lib_rbtree_create(cmp)
#define c_lib_rbtree_str_int_insert(tree, key, val) c_lib_rbtree_insert(tree, key, &(int){val})
#define c_lib_rbtree_str_int_search(tree, key) (*(int *)c_lib_rbtree_search(tree, key))

#define c_lib_RBTREE(T) c_lib_concat(c_lib_rbtree_, T)

#define c_lib_RBTREE_CREATE(T, cmp) \
    _Generic((T *)0, \
        int: c_lib_rbtree_int_create(cmp), \
        size_t: c_lib_rbtree_size_t_create(cmp), \
        char *: c_lib_rbtree_str_int_create(cmp))

#define c_lib_RBTREE_INSERT(tree, key, val) \
    _Generic((key), \
        int: c_lib_rbtree_int_insert(tree, key, val), \
        size_t: c_lib_rbtree_size_t_insert(tree, key, val), \
        char *: c_lib_rbtree_str_int_insert(tree, key, val))

#define c_lib_RBTREE_SEARCH(tree, key) \
    (*(typeof(*(typeof(tree))0) *)_Generic((key), \
        int: c_lib_rbtree_search(tree, &(key)), \
        size_t: c_lib_rbtree_search(tree, &(key)), \
        char *: c_lib_rbtree_search(tree, key)))

#endif