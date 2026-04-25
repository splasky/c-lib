#include "../include/bstree.h"
#include "../include/dbg.h"
#include <stdio.h>
#include <stdlib.h>

static int int_compare(const void* restrict a, const void* restrict b)
{
    const int x = *((int*)(a));
    const int y = *((int*)b);
    return (x > y) - (y > x);
}

BSTree* New_BSTree(BSTree_compare bstree_compare)
{
    BSTree* bstree = calloc(1, sizeof(BSTree));
    check_mem(bstree);

    bstree->count = 0;
    bstree->compare = bstree_compare == NULL ? int_compare : bstree_compare;
    bstree->root = NULL;
    return bstree;
error:
    log_err("create bstree failed");
    return NULL;
}

int BSTreeNode_destory(BSTreeNode* node)
{
    if (node == NULL) {
        return 0;
    }

    if (node->key) {
        free(node->key);
        node->key = NULL;
    }

    if (node->data) {
        free(node->data);
        node->data = NULL;
    }

    free(node);
    node = NULL;
    return 0;
}

BSTreeNode* BSTreeNode_create(BSTreeNode* parent, void* key, void* value)
{
    BSTreeNode* node = calloc(1, sizeof(BSTreeNode));
    check_mem(node);
    node->parent = parent;
    node->key = key;
    node->data = value;
    return node;
error:
    log_err("create bstree node error.");
    return NULL;
}

void BSTree_destory(BSTree* bstree)
{
    if (bstree) {
        BSTree_traverse(bstree, BSTreeNode_traverse_postOrder, BSTreeNode_destory);
        free(bstree);
        bstree = NULL;
    }
}

static inline void BSTreeNode_set(
    BSTree* bstree, BSTreeNode* node, void* restrict key, void* restrict data)
{

    if (bstree->compare(node->key, key) <= 0) {
        /* insert left */
        if (node->left) {
            BSTreeNode_set(bstree, node->left, key, data);
        } else {
            node->left = BSTreeNode_create(node, key, data);
        }
    } else {
        if (node->right) {
            BSTreeNode_set(bstree, node->right, key, data);
        } else {
            node->right = BSTreeNode_create(node, key, data);
        }
    }
}

int BSTree_set(BSTree* bstree, void* key, void* data)
{

    if (!bstree->root) {
        BSTreeNode* node = BSTreeNode_create(NULL, key, data);
        check_mem(node);
        bstree->root = node;
        return 0;
    } else {
        BSTreeNode_set(bstree, bstree->root, key, data);
    }

    return 0;
error:
    log_err("set node failed");
    return -1;
}

static inline BSTreeNode* BSTreeNode_get_node(BSTree* bstree, BSTreeNode* node, void* key)
{
    int compare = bstree->compare(node->key, key);
    if (compare == 0) {
        return node;
    } else if (compare > 0 && node->right) {
        return BSTreeNode_get_node(bstree, node->right, key);
    } else if (compare < 0 && node->left) {
        return BSTreeNode_get_node(bstree, node->left, key);
    }
    return NULL;
}

void* BSTree_get(BSTree* bstree, void* key)
{
    if (!bstree->root)
        return NULL;

    BSTreeNode* node = BSTreeNode_get_node(bstree, bstree->root, key);
    return node == NULL ? NULL : node->data;
}

int BSTreeNode_traverse_postOrder(BSTreeNode* node, BSTree_traverse_cb bstree_traverse)
{
    int rc = 0;
    if (node->left) {
        rc = BSTreeNode_traverse_postOrder(node->left, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    if (node->right) {
        rc = BSTreeNode_traverse_postOrder(node->right, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    return bstree_traverse(node);
}

int BSTreeNode_traverse_inOrder(BSTreeNode* node, BSTree_traverse_cb bstree_traverse)
{
    int rc = 0;
    if (node->left) {
        rc = BSTreeNode_traverse_inOrder(node->left, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    rc = bstree_traverse(node);

    if (node->right) {
        rc = BSTreeNode_traverse_inOrder(node->right, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    return rc;
}

int BSTreeNode_traverse_preOrder(BSTreeNode* node, BSTree_traverse_cb bstree_traverse)
{
    int rc = 0;
    rc = bstree_traverse(node);
    if (node->left) {
        rc = BSTreeNode_traverse_preOrder(node->left, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    if (node->right) {
        rc = BSTreeNode_traverse_preOrder(node->right, bstree_traverse);
        if (rc != 0)
            return rc;
    }

    return rc;
}

int BSTree_traverse(
    BSTree* bstree, BSTreeNode_traverse node_traverse, BSTree_traverse_cb bstree_traverse)
{
    if (bstree->root) {
        return node_traverse(bstree->root, bstree_traverse);
    }

    return 0;
}

static inline BSTreeNode* BSTreeNode_find_min(BSTreeNode* node)
{
    while (node->left) {
        BSTreeNode_find_min(node->left);
    }

    return node;
}

static inline void BSTreeNode_swap(BSTreeNode* a, BSTreeNode* b)
{
    void* tmp = NULL;
    tmp = a->key;
    a->key = b->key;
    b->key = tmp;

    tmp = a->data;
    a->data = b->data;
    b->data = tmp;
}

static inline void BSTreeNode_replace_node_in_parent(
    BSTree* bstree, BSTreeNode* node, BSTreeNode* new_node)
{

    if (node->parent) {
        if (node == node->parent->left) {
            node->parent->left = new_node;
        } else {
            node->parent->right = new_node;
        }
    } else {
        bstree->root = new_node;
    }

    if (new_node) {
        new_node->parent = node->parent;
    }
}

static inline void* BSTreeNode_delete(BSTree* bstree, BSTreeNode* node, void* key)
{
    int compare = bstree->compare(node->key, key);

    if (compare < 0) {
        /* scan left */
        if (node->left) {
            return BSTreeNode_delete(bstree, node->left, key);
        } else {
            return NULL;
        }
    }

    if (compare > 0) {
        if (node->right) {
            return BSTreeNode_delete(bstree, node->right, key);
        } else {
            return NULL;
        }
    }

    if (node->left && node->right) {
        BSTreeNode* changed = BSTreeNode_find_min(node->right);
        BSTreeNode_swap(changed, node);
        BSTreeNode_replace_node_in_parent(bstree, changed, changed->right);
        return changed;
    } else if (node->left) {
        BSTreeNode_replace_node_in_parent(bstree, node, node->left);
    } else if (node->right) {
        BSTreeNode_replace_node_in_parent(bstree, node, node->right);
    } else {
        // leaf
        BSTreeNode_replace_node_in_parent(bstree, node, NULL);
    }
    return node;
}

void* BSTree_delete(BSTree* bstree, void* key)
{
    void* data = NULL;
    if (bstree->root) {
        BSTreeNode* tmp_node = BSTreeNode_delete(bstree, bstree->root, key);
        if (tmp_node) {
            data = tmp_node->data;
            free(tmp_node->key);
            free(tmp_node);
            tmp_node = NULL;
        }
    }

    return data;
}
