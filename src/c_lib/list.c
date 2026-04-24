#include "c_lib/list.h"
#include <stdlib.h>
#include <string.h>

c_lib_static_assert(sizeof(c_lib_list_node) % sizeof(void *) == 0,
                    "c_lib_list_node must be cache-aligned");

c_lib_list *c_lib_list_create(void)
{
    c_lib_list *list = calloc(1, sizeof(*list));
    return list;
}

static c_lib_list_node *list_node_create(void *data)
{
    c_lib_list_node *node = calloc(1, sizeof(*node));
    if (C_LIB_UNLIKELY(node == NULL))
        return NULL;
    node->data = data;
    return node;
}

void c_lib_list_destroy(c_lib_list *list)
{
    if (list == NULL)
        return;
    c_lib_list_clear(list);
    free(list);
}

void c_lib_list_destroy_with(c_lib_list *list, c_lib_list_free_fn free_fn)
{
    if (list == NULL)
        return;
    c_lib_list_clear_with(list, free_fn);
    free(list);
}

void c_lib_list_clear(c_lib_list *list)
{
    if (list == NULL)
        return;

    c_lib_list_node *curr = list->head;
    while (curr != NULL) {
        c_lib_list_node *next = curr->next;
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void c_lib_list_clear_with(c_lib_list *list, c_lib_list_free_fn free_fn)
{
    if (list == NULL)
        return;

    c_lib_list_node *curr = list->head;
    while (curr != NULL) {
        c_lib_list_node *next = curr->next;
        if (free_fn != NULL && curr->data != NULL)
            free_fn(curr->data);
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void c_lib_list_push(c_lib_list *list, void *data)
{
    if (C_LIB_UNLIKELY(list == NULL))
        return;

    c_lib_list_node *node = list_node_create(data);
    if (C_LIB_UNLIKELY(node == NULL))
        return;

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;
}

void c_lib_list_push_front(c_lib_list *list, void *data)
{
    if (C_LIB_UNLIKELY(list == NULL))
        return;

    c_lib_list_node *node = list_node_create(data);
    if (C_LIB_UNLIKELY(node == NULL))
        return;

    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->count++;
}

void c_lib_list_push_at(c_lib_list *list, void *data, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(list == NULL))
        return;

    if (index >= list->count) {
        c_lib_list_push(list, data);
        return;
    }

    if (index == 0) {
        c_lib_list_push_front(list, data);
        return;
    }

    c_lib_list_node *node = list_node_create(data);
    if (C_LIB_UNLIKELY(node == NULL))
        return;

    c_lib_list_node *curr = list->head;
    for (c_lib_size_t i = 0; i < index - 1; i++)
        curr = curr->next;

    node->next = curr->next;
    node->prev = curr;
    curr->next->prev = node;
    curr->next = node;
    list->count++;
}

void *c_lib_list_pop(c_lib_list *list)
{
    if (C_LIB_UNLIKELY(list == NULL || list->tail == NULL))
        return NULL;

    c_lib_list_node *node = list->tail;
    void *data = node->data;

    if (node->prev != NULL) {
        node->prev->next = NULL;
        list->tail = node->prev;
    } else {
        list->head = NULL;
        list->tail = NULL;
    }

    list->count--;
    free(node);
    return data;
}

void *c_lib_list_pop_front(c_lib_list *list)
{
    if (C_LIB_UNLIKELY(list == NULL || list->head == NULL))
        return NULL;

    c_lib_list_node *node = list->head;
    void *data = node->data;

    if (node->next != NULL) {
        node->next->prev = NULL;
        list->head = node->next;
    } else {
        list->head = NULL;
        list->tail = NULL;
    }

    list->count--;
    free(node);
    return data;
}

void *c_lib_list_pop_at(c_lib_list *list, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(list == NULL || index >= list->count))
        return NULL;

    c_lib_list_node *node = list->head;
    for (c_lib_size_t i = 0; i < index; i++)
        node = node->next;

    void *data = node->data;

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    list->count--;
    free(node);
    return data;
}

void *c_lib_list_get(const c_lib_list *list, c_lib_size_t index)
{
    if (C_LIB_UNLIKELY(list == NULL || index >= list->count))
        return NULL;

    c_lib_list_node *node = list->head;
    for (c_lib_size_t i = 0; i < index; i++)
        node = node->next;

    return node->data;
}

c_lib_list_node *c_lib_list_find(const c_lib_list *list, const void *data, c_lib_list_cmp cmp)
{
    if (C_LIB_UNLIKELY(list == NULL || cmp == NULL))
        return NULL;

    c_lib_list_node *node = list->head;
    while (node != NULL) {
        if (cmp(node->data, data) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

void c_lib_list_remove_node(c_lib_list *list, c_lib_list_node *node)
{
    if (C_LIB_UNLIKELY(list == NULL || node == NULL))
        return;

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    list->count--;
    free(node);
}

bool c_lib_list_remove_data(c_lib_list *list, const void *data, c_lib_list_cmp cmp)
{
    if (C_LIB_UNLIKELY(list == NULL || cmp == NULL))
        return false;

    c_lib_list_node *node = c_lib_list_find(list, data, cmp);
    if (node == NULL)
        return false;

    c_lib_list_remove_node(list, node);
    return true;
}

c_lib_size_t c_lib_list_count(const c_lib_list *list)
{
    return list != NULL ? list->count : 0;
}

bool c_lib_list_is_empty(const c_lib_list *list)
{
    return list == NULL || list->count == 0;
}

void c_lib_list_reverse(c_lib_list *list)
{
    if (C_LIB_UNLIKELY(list == NULL || list->count <= 1))
        return;

    c_lib_list_node *curr = list->head;
    c_lib_list_node *tmp = NULL;

    list->tail = list->head;

    while (curr != NULL) {
        tmp = curr->prev;
        curr->prev = curr->next;
        curr->next = tmp;
        curr = curr->prev;
    }

    if (tmp != NULL)
        list->head = tmp->prev;
}

c_lib_list *c_lib_list_clone(const c_lib_list *list)
{
    if (C_LIB_UNLIKELY(list == NULL))
        return NULL;

    c_lib_list *new_list = c_lib_list_create();
    if (C_LIB_UNLIKELY(new_list == NULL))
        return NULL;

    c_lib_list_node *node = list->head;
    while (node != NULL) {
        c_lib_list_push(new_list, node->data);
        node = node->next;
    }

    return new_list;
}

void c_lib_list_merge(c_lib_list *dst, c_lib_list *src)
{
    if (C_LIB_UNLIKELY(dst == NULL || src == NULL))
        return;

    if (src->count == 0)
        return;

    if (dst->tail != NULL) {
        dst->tail->next = src->head;
        src->head->prev = dst->tail;
        dst->tail = src->tail;
    } else {
        dst->head = src->head;
        dst->tail = src->tail;
    }

    dst->count += src->count;

    src->head = NULL;
    src->tail = NULL;
    src->count = 0;
}