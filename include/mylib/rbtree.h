#ifndef MYLIB_RBTREE_H
#define MYLIB_RBTREE_H

#include "base.h"

typedef enum mylib_rbtree_color {
    MYLIB_RBTREE_BLACK = 0,
    MYLIB_RBTREE_RED = 1,
} mylib_rbtree_color;

typedef struct mylib_rbtree_node {
    struct mylib_rbtree_node *left;
    struct mylib_rbtree_node *right;
    struct mylib_rbtree_node *parent;
    void *key;
    void *value;
    mylib_rbtree_color color;
} mylib_rbtree_node;

typedef struct mylib_rbtree {
    mylib_rbtree_node *root;
    mylib_size_t count;
    int (*cmp)(const void *a, const void *b);
} mylib_rbtree;

typedef int (*mylib_rbtree_key_cmp)(const void *a, const void *b);

mylib_rbtree *mylib_rbtree_create(mylib_rbtree_key_cmp cmp);
void mylib_rbtree_destroy(mylib_rbtree *tree);
void mylib_rbtree_clear(mylib_rbtree *tree);
bool mylib_rbtree_insert(mylib_rbtree *tree, void *key, void *value);
bool mylib_rbtree_delete(mylib_rbtree *tree, const void *key);
void *mylib_rbtree_search(const mylib_rbtree *tree, const void *key);
bool mylib_rbtree_update(mylib_rbtree *tree, const void *key, void *value);
mylib_rbtree_node *mylib_rbtree_min(mylib_rbtree_node *node);
mylib_rbtree_node *mylib_rbtree_max(mylib_rbtree_node *node);
mylib_size_t mylib_rbtree_count(const mylib_rbtree *tree);
bool mylib_rbtree_is_empty(const mylib_rbtree *tree);

typedef void (*mylib_rbtree_visit_fn)(const mylib_rbtree_node *node);
void mylib_rbtree_inorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit);
void mylib_rbtree_preorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit);
void mylib_rbtree_postorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit);

mylib_rbtree_node *mylib_rbtree_lower_bound(const mylib_rbtree *tree, const void *key);
mylib_rbtree_node *mylib_rbtree_upper_bound(const mylib_rbtree *tree, const void *key);

#endif
