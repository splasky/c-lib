#include "mylib/rbtree.h"
#include <stdlib.h>
#include <string.h>

static mylib_rbtree_node *rbtree_node_create(void *key, void *value)
{
    mylib_rbtree_node *node = calloc(1, sizeof(*node));
    if (MYLIB_UNLIKELY(node == NULL))
        return NULL;

    node->key = key;
    node->value = value;
    node->color = MYLIB_RBTREE_RED;
    return node;
}

static void rbtree_left_rotate(mylib_rbtree_node **root, mylib_rbtree_node *x)
{
    mylib_rbtree_node *y = x->right;

    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

static void rbtree_right_rotate(mylib_rbtree_node **root, mylib_rbtree_node *y)
{
    mylib_rbtree_node *x = y->left;

    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;

    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

static void rbtree_insert_fixup(mylib_rbtree_node **root, mylib_rbtree_node *node)
{
    mylib_rbtree_node *parent, *gparent, *uncle;

    while ((parent = node->parent) != NULL && parent->color == MYLIB_RBTREE_RED) {
        gparent = parent->parent;

        if (gparent == NULL)
            break;

        if (parent == gparent->left) {
            uncle = gparent->right;
            if (uncle != NULL && uncle->color == MYLIB_RBTREE_RED) {
                parent->color = MYLIB_RBTREE_BLACK;
                uncle->color = MYLIB_RBTREE_BLACK;
                gparent->color = MYLIB_RBTREE_RED;
                node = gparent;
                continue;
            }

            if (parent->right == node) {
                rbtree_left_rotate(root, parent);
                MYLIB_SWAP(mylib_rbtree_node *, parent, node);
            }

            parent->color = MYLIB_RBTREE_BLACK;
            gparent->color = MYLIB_RBTREE_RED;
            rbtree_right_rotate(root, gparent);
        } else {
            uncle = gparent->left;
            if (uncle != NULL && uncle->color == MYLIB_RBTREE_RED) {
                parent->color = MYLIB_RBTREE_BLACK;
                uncle->color = MYLIB_RBTREE_BLACK;
                gparent->color = MYLIB_RBTREE_RED;
                node = gparent;
                continue;
            }

            if (parent->left == node) {
                rbtree_right_rotate(root, parent);
                MYLIB_SWAP(mylib_rbtree_node *, parent, node);
            }

            parent->color = MYLIB_RBTREE_BLACK;
            gparent->color = MYLIB_RBTREE_RED;
            rbtree_left_rotate(root, gparent);
        }
    }

    (*root)->color = MYLIB_RBTREE_BLACK;
}

static void rbtree_transplant(mylib_rbtree_node **root,
                               mylib_rbtree_node *u, mylib_rbtree_node *v)
{
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL)
        v->parent = u->parent;
}

static void rbtree_erase_fixup(mylib_rbtree_node **root,
                                mylib_rbtree_node *x, mylib_rbtree_node *x_parent)
{
    mylib_rbtree_node *w;

    while (x == NULL || (x->color == MYLIB_RBTREE_BLACK && x != *root)) {
        if (x == x_parent->left) {
            w = x_parent->right;
            if (w != NULL && w->color == MYLIB_RBTREE_RED) {
                w->color = MYLIB_RBTREE_BLACK;
                x_parent->color = MYLIB_RBTREE_RED;
                rbtree_left_rotate(root, x_parent);
                w = x_parent->right;
            }

            if ((w->left == NULL || w->left->color == MYLIB_RBTREE_BLACK) &&
                (w->right == NULL || w->right->color == MYLIB_RBTREE_BLACK)) {
                if (w != NULL)
                    w->color = MYLIB_RBTREE_RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->right == NULL || w->right->color == MYLIB_RBTREE_BLACK) {
                    if (w->left != NULL)
                        w->left->color = MYLIB_RBTREE_BLACK;
                    if (w != NULL)
                        w->color = MYLIB_RBTREE_RED;
                    rbtree_right_rotate(root, w);
                    w = x_parent->right;
                }
                if (w != NULL)
                    w->color = x_parent->color;
                x_parent->color = MYLIB_RBTREE_BLACK;
                if (w->right != NULL)
                    w->right->color = MYLIB_RBTREE_BLACK;
                rbtree_left_rotate(root, x_parent);
                x = *root;
            }
        } else {
            w = x_parent->left;
            if (w != NULL && w->color == MYLIB_RBTREE_RED) {
                w->color = MYLIB_RBTREE_BLACK;
                x_parent->color = MYLIB_RBTREE_RED;
                rbtree_right_rotate(root, x_parent);
                w = x_parent->left;
            }

            if ((w->right == NULL || w->right->color == MYLIB_RBTREE_BLACK) &&
                (w->left == NULL || w->left->color == MYLIB_RBTREE_BLACK)) {
                if (w != NULL)
                    w->color = MYLIB_RBTREE_RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                if (w->left == NULL || w->left->color == MYLIB_RBTREE_BLACK) {
                    if (w->right != NULL)
                        w->right->color = MYLIB_RBTREE_BLACK;
                    if (w != NULL)
                        w->color = MYLIB_RBTREE_RED;
                    rbtree_left_rotate(root, w);
                    w = x_parent->left;
                }
                if (w != NULL)
                    w->color = x_parent->color;
                x_parent->color = MYLIB_RBTREE_BLACK;
                if (w->left != NULL)
                    w->left->color = MYLIB_RBTREE_BLACK;
                rbtree_right_rotate(root, x_parent);
                x = *root;
            }
        }

        if (x != NULL)
            x_parent = x->parent;
    }

    if (x != NULL)
        x->color = MYLIB_RBTREE_BLACK;
}

static mylib_rbtree_node *rbtree_minimum(mylib_rbtree_node *node)
{
    while (node != NULL && node->left != NULL)
        node = node->left;
    return node;
}

mylib_rbtree *mylib_rbtree_create(mylib_rbtree_key_cmp cmp)
{
    if (MYLIB_UNLIKELY(cmp == NULL))
        return NULL;

    mylib_rbtree *tree = calloc(1, sizeof(*tree));
    if (MYLIB_UNLIKELY(tree == NULL))
        return NULL;

    tree->cmp = cmp;
    return tree;
}

void mylib_rbtree_destroy(mylib_rbtree *tree)
{
    if (tree == NULL)
        return;
    mylib_rbtree_clear(tree);
    free(tree);
}

void mylib_rbtree_clear(mylib_rbtree *tree)
{
    if (tree == NULL)
        return;

    mylib_rbtree_node *stack[64];
    mylib_rbtree_node **sp = stack;
    mylib_rbtree_node *node = tree->root;

    while (node != NULL || sp > stack) {
        while (node != NULL) {
            *sp++ = node;
            node = node->left;
        }
        node = *--sp;
        mylib_rbtree_node *right = node->right;
        free(node);
        node = right;
    }

    tree->root = NULL;
    tree->count = 0;
}

bool mylib_rbtree_insert(mylib_rbtree *tree, void *key, void *value)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return false;

    mylib_rbtree_node *node = rbtree_node_create(key, value);
    if (MYLIB_UNLIKELY(node == NULL))
        return false;

    mylib_rbtree_node *parent = NULL;
    mylib_rbtree_node *curr = tree->root;

    while (curr != NULL) {
        parent = curr;
        int cmp = tree->cmp(key, curr->key);
        if (cmp < 0) {
            curr = curr->left;
        } else if (cmp > 0) {
            curr = curr->right;
        } else {
            curr->value = value;
            free(node);
            return true;
        }
    }

    node->parent = parent;
    if (parent == NULL) {
        tree->root = node;
    } else if (tree->cmp(key, parent->key) < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    tree->count++;
    rbtree_insert_fixup(&tree->root, node);
    return true;
}

bool mylib_rbtree_delete(mylib_rbtree *tree, const void *key)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return false;

    mylib_rbtree_node *node = tree->root;
    while (node != NULL) {
        int cmp = tree->cmp(key, node->key);
        if (cmp == 0)
            break;
        node = cmp < 0 ? node->left : node->right;
    }

    if (node == NULL)
        return false;

    mylib_rbtree_node *y = node;
    mylib_rbtree_node *x;
    mylib_rbtree_node *x_parent;
    mylib_rbtree_color y_original_color = y->color;

    if (node->left == NULL) {
        x = node->right;
        x_parent = node->parent;
        rbtree_transplant(&tree->root, node, node->right);
    } else if (node->right == NULL) {
        x = node->left;
        x_parent = node->parent;
        rbtree_transplant(&tree->root, node, node->left);
    } else {
        y = rbtree_minimum(node->right);
        y_original_color = y->color;
        x = y->right;
        x_parent = y;

        if (y->parent == node) {
            if (x != NULL)
                x->parent = y;
        } else {
            rbtree_transplant(&tree->root, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }

        rbtree_transplant(&tree->root, node, y);
        y->left = node->left;
        y->left->parent = y;
        y->color = node->color;
    }

    free(node);
    tree->count--;

    if (y_original_color == MYLIB_RBTREE_BLACK)
        rbtree_erase_fixup(&tree->root, x, x_parent);

    return true;
}

void *mylib_rbtree_search(const mylib_rbtree *tree, const void *key)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return NULL;

    mylib_rbtree_node *node = tree->root;
    while (node != NULL) {
        int cmp = tree->cmp(key, node->key);
        if (cmp == 0)
            return node->value;
        node = cmp < 0 ? node->left : node->right;
    }
    return NULL;
}

bool mylib_rbtree_update(mylib_rbtree *tree, const void *key, void *value)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return false;

    mylib_rbtree_node *node = tree->root;
    while (node != NULL) {
        int cmp = tree->cmp(key, node->key);
        if (cmp == 0) {
            node->value = value;
            return true;
        }
        node = cmp < 0 ? node->left : node->right;
    }
    return false;
}

mylib_rbtree_node *mylib_rbtree_min(const mylib_rbtree_node *node)
{
    if (node == NULL)
        return NULL;
    while (node->left != NULL)
        node = node->left;
    return (mylib_rbtree_node *)node;
}

mylib_rbtree_node *mylib_rbtree_max(const mylib_rbtree_node *node)
{
    if (node == NULL)
        return NULL;
    while (node->right != NULL)
        node = node->right;
    return (mylib_rbtree_node *)node;
}

mylib_size_t mylib_rbtree_count(const mylib_rbtree *tree)
{
    return tree != NULL ? tree->count : 0;
}

bool mylib_rbtree_is_empty(const mylib_rbtree *tree)
{
    return tree == NULL || tree->count == 0;
}

static void inorder_helper(mylib_rbtree_node *node, mylib_rbtree_visit_fn visit)
{
    if (node == NULL)
        return;
    inorder_helper(node->left, visit);
    visit(node);
    inorder_helper(node->right, visit);
}

static void preorder_helper(mylib_rbtree_node *node, mylib_rbtree_visit_fn visit)
{
    if (node == NULL)
        return;
    visit(node);
    preorder_helper(node->left, visit);
    preorder_helper(node->right, visit);
}

static void postorder_helper(mylib_rbtree_node *node, mylib_rbtree_visit_fn visit)
{
    if (node == NULL)
        return;
    postorder_helper(node->left, visit);
    postorder_helper(node->right, visit);
    visit(node);
}

void mylib_rbtree_inorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit)
{
    if (tree == NULL || visit == NULL)
        return;
    inorder_helper(tree->root, visit);
}

void mylib_rbtree_preorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit)
{
    if (tree == NULL || visit == NULL)
        return;
    preorder_helper(tree->root, visit);
}

void mylib_rbtree_postorder(const mylib_rbtree *tree, mylib_rbtree_visit_fn visit)
{
    if (tree == NULL || visit == NULL)
        return;
    postorder_helper(tree->root, visit);
}

mylib_rbtree_node *mylib_rbtree_lower_bound(const mylib_rbtree *tree, const void *key)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return NULL;

    mylib_rbtree_node *node = tree->root;
    mylib_rbtree_node *result = NULL;

    while (node != NULL) {
        if (tree->cmp(key, node->key) <= 0) {
            result = node;
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return result;
}

mylib_rbtree_node *mylib_rbtree_upper_bound(const mylib_rbtree *tree, const void *key)
{
    if (MYLIB_UNLIKELY(tree == NULL || key == NULL))
        return NULL;

    mylib_rbtree_node *node = tree->root;
    mylib_rbtree_node *result = NULL;

    while (node != NULL) {
        if (tree->cmp(key, node->key) < 0) {
            result = node;
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return result;
}
