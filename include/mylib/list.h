#ifndef MYLIB_LIST_H
#define MYLIB_LIST_H

#include "base.h"

typedef struct mylib_list_node {
    struct mylib_list_node *next;
    struct mylib_list_node *prev;
    void *data;
} mylib_list_node;

typedef struct mylib_list {
    mylib_list_node *head;
    mylib_list_node *tail;
    mylib_size_t count;
} mylib_list;

typedef int (*mylib_list_cmp)(const void *a, const void *b);
typedef void (*mylib_list_free_fn)(void *data);

mylib_list *mylib_list_create(void);
void mylib_list_destroy(mylib_list *list);
void mylib_list_destroy_with(mylib_list *list, mylib_list_free_fn free_fn);
void mylib_list_clear(mylib_list *list);
void mylib_list_clear_with(mylib_list *list, mylib_list_free_fn free_fn);
void mylib_list_push(mylib_list *list, void *data);
void mylib_list_push_front(mylib_list *list, void *data);
void mylib_list_push_at(mylib_list *list, void *data, mylib_size_t index);
void *mylib_list_pop(mylib_list *list);
void *mylib_list_pop_front(mylib_list *list);
void *mylib_list_pop_at(mylib_list *list, mylib_size_t index);
void *mylib_list_get(const mylib_list *list, mylib_size_t index);
mylib_list_node *mylib_list_find(const mylib_list *list, const void *data, mylib_list_cmp cmp);
void mylib_list_remove_node(mylib_list *list, mylib_list_node *node);
bool mylib_list_remove_data(mylib_list *list, const void *data, mylib_list_cmp cmp);
mylib_size_t mylib_list_count(const mylib_list *list);
bool mylib_list_is_empty(const mylib_list *list);
void mylib_list_reverse(mylib_list *list);
mylib_list *mylib_list_clone(const mylib_list *list);
void mylib_list_merge(mylib_list *dst, mylib_list *src);

#define MYLIB_LIST_FOREACH(list, node) \
    for (mylib_list_node *node = (list)->head; node != NULL; node = node->next)

#define MYLIB_LIST_FOREACH_SAFE(list, node, tmp) \
    for (mylib_list_node *node = (list)->head, *tmp = node ? node->next : NULL; \
         node != NULL; \
         node = tmp, tmp = tmp ? tmp->next : NULL)

#define MYLIB_LIST_ENTRY(ptr, type, member) MYLIB_CONTAINER_OF(ptr, type, member)

#endif
