#include "mylib/list.h"
#include <stdlib.h>
#include <string.h>

mylib_static_assert(sizeof(mylib_list_node) % sizeof(void *) == 0,
                    "mylib_list_node must be cache-aligned");

mylib_list *mylib_list_create(void)
{
    mylib_list *list = calloc(1, sizeof(*list));
    return list;
}

static void list_node_free(mylib_list_node *node)
{
    if (node == NULL)
        return;
    free(node);
}

static mylib_list_node *list_node_create(void *data)
{
    mylib_list_node *node = calloc(1, sizeof(*node));
    if (MYLIB_UNLIKELY(node == NULL))
        return NULL;
    node->data = data;
    return node;
}

void mylib_list_destroy(mylib_list *list)
{
    if (list == NULL)
        return;
    mylib_list_clear(list);
    free(list);
}

void mylib_list_destroy_with(mylib_list *list, mylib_list_free_fn free_fn)
{
    if (list == NULL)
        return;
    mylib_list_clear_with(list, free_fn);
    free(list);
}

void mylib_list_clear(mylib_list *list)
{
    if (list == NULL)
        return;

    mylib_list_node *curr = list->head;
    while (curr != NULL) {
        mylib_list_node *next = curr->next;
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void mylib_list_clear_with(mylib_list *list, mylib_list_free_fn free_fn)
{
    if (list == NULL)
        return;

    mylib_list_node *curr = list->head;
    while (curr != NULL) {
        mylib_list_node *next = curr->next;
        if (free_fn != NULL && curr->data != NULL)
            free_fn(curr->data);
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void mylib_list_push(mylib_list *list, void *data)
{
    if (MYLIB_UNLIKELY(list == NULL))
        return;

    mylib_list_node *node = list_node_create(data);
    if (MYLIB_UNLIKELY(node == NULL))
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

void mylib_list_push_front(mylib_list *list, void *data)
{
    if (MYLIB_UNLIKELY(list == NULL))
        return;

    mylib_list_node *node = list_node_create(data);
    if (MYLIB_UNLIKELY(node == NULL))
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

void mylib_list_push_at(mylib_list *list, void *data, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(list == NULL))
        return;

    if (index >= list->count) {
        mylib_list_push(list, data);
        return;
    }

    if (index == 0) {
        mylib_list_push_front(list, data);
        return;
    }

    mylib_list_node *node = list_node_create(data);
    if (MYLIB_UNLIKELY(node == NULL))
        return;

    mylib_list_node *curr = list->head;
    for (mylib_size_t i = 0; i < index - 1; i++)
        curr = curr->next;

    node->next = curr->next;
    node->prev = curr;
    curr->next->prev = node;
    curr->next = node;
    list->count++;
}

void *mylib_list_pop(mylib_list *list)
{
    if (MYLIB_UNLIKELY(list == NULL || list->tail == NULL))
        return NULL;

    mylib_list_node *node = list->tail;
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

void *mylib_list_pop_front(mylib_list *list)
{
    if (MYLIB_UNLIKELY(list == NULL || list->head == NULL))
        return NULL;

    mylib_list_node *node = list->head;
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

void *mylib_list_pop_at(mylib_list *list, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(list == NULL || index >= list->count))
        return NULL;

    mylib_list_node *node = list->head;
    for (mylib_size_t i = 0; i < index; i++)
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

void *mylib_list_get(const mylib_list *list, mylib_size_t index)
{
    if (MYLIB_UNLIKELY(list == NULL || index >= list->count))
        return NULL;

    mylib_list_node *node = list->head;
    for (mylib_size_t i = 0; i < index; i++)
        node = node->next;

    return node->data;
}

mylib_list_node *mylib_list_find(const mylib_list *list, const void *data, mylib_list_cmp cmp)
{
    if (MYLIB_UNLIKELY(list == NULL || cmp == NULL))
        return NULL;

    mylib_list_node *node = list->head;
    while (node != NULL) {
        if (cmp(node->data, data) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

void mylib_list_remove_node(mylib_list *list, mylib_list_node *node)
{
    if (MYLIB_UNLIKELY(list == NULL || node == NULL))
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

bool mylib_list_remove_data(mylib_list *list, const void *data, mylib_list_cmp cmp)
{
    if (MYLIB_UNLIKELY(list == NULL || cmp == NULL))
        return false;

    mylib_list_node *node = mylib_list_find(list, data, cmp);
    if (node == NULL)
        return false;

    mylib_list_remove_node(list, node);
    return true;
}

mylib_size_t mylib_list_count(const mylib_list *list)
{
    return list != NULL ? list->count : 0;
}

bool mylib_list_is_empty(const mylib_list *list)
{
    return list == NULL || list->count == 0;
}

void mylib_list_reverse(mylib_list *list)
{
    if (MYLIB_UNLIKELY(list == NULL || list->count <= 1))
        return;

    mylib_list_node *curr = list->head;
    mylib_list_node *tmp = NULL;

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

mylib_list *mylib_list_clone(const mylib_list *list)
{
    if (MYLIB_UNLIKELY(list == NULL))
        return NULL;

    mylib_list *new_list = mylib_list_create();
    if (MYLIB_UNLIKELY(new_list == NULL))
        return NULL;

    mylib_list_node *node = list->head;
    while (node != NULL) {
        mylib_list_push(new_list, node->data);
        node = node->next;
    }

    return new_list;
}

void mylib_list_merge(mylib_list *dst, mylib_list *src)
{
    if (MYLIB_UNLIKELY(dst == NULL || src == NULL))
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
