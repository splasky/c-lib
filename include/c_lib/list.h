#ifndef C_LIB_LIST_H
#define C_LIB_LIST_H

#include "base.h"

typedef struct c_lib_list_node {
    struct c_lib_list_node *next;
    struct c_lib_list_node *prev;
    void *data;
} c_lib_list_node;

typedef struct c_lib_list {
    c_lib_list_node *head;
    c_lib_list_node *tail;
    c_lib_size_t count;
} c_lib_list;

typedef int (*c_lib_list_cmp)(const void *a, const void *b);
typedef void (*c_lib_list_free_fn)(void *data);

c_lib_list *c_lib_list_create(void);
void c_lib_list_destroy(c_lib_list *list);
void c_lib_list_destroy_with(c_lib_list *list, c_lib_list_free_fn free_fn);
void c_lib_list_clear(c_lib_list *list);
void c_lib_list_clear_with(c_lib_list *list, c_lib_list_free_fn free_fn);
void c_lib_list_push(c_lib_list *list, void *data);
void c_lib_list_push_front(c_lib_list *list, void *data);
void c_lib_list_push_at(c_lib_list *list, void *data, c_lib_size_t index);
void *c_lib_list_pop(c_lib_list *list);
void *c_lib_list_pop_front(c_lib_list *list);
void *c_lib_list_pop_at(c_lib_list *list, c_lib_size_t index);
void *c_lib_list_get(const c_lib_list *list, c_lib_size_t index);
c_lib_list_node *c_lib_list_find(const c_lib_list *list, const void *data, c_lib_list_cmp cmp);
void c_lib_list_remove_node(c_lib_list *list, c_lib_list_node *node);
bool c_lib_list_remove_data(c_lib_list *list, const void *data, c_lib_list_cmp cmp);
c_lib_size_t c_lib_list_count(const c_lib_list *list);
bool c_lib_list_is_empty(const c_lib_list *list);
void c_lib_list_reverse(c_lib_list *list);
c_lib_list *c_lib_list_clone(const c_lib_list *list);
void c_lib_list_merge(c_lib_list *dst, c_lib_list *src);

#define c_lib_list_foreach(list, node) \
    for (c_lib_list_node *node = (list)->head; node != NULL; node = node->next)

#define c_lib_list_foreach_safe(list, node, tmp) \
    for (c_lib_list_node *node = (list)->head, *tmp = node ? node->next : NULL; \
         node != NULL; \
         node = tmp, tmp = tmp ? tmp->next : NULL)

#define c_lib_list_entry(ptr, type, member) c_lib_container_of(ptr, type, member)

#define c_lib_list_int c_lib_TYPED_LIST(int)
#define c_lib_list_int_create() c_lib_list_create()
#define c_lib_list_int_push(list, val) c_lib_list_push(list, &(int){val})
#define c_lib_list_int_pop(list) (*(int *)c_lib_list_pop(list))
#define c_lib_list_int_get(list, idx) (*(int *)c_lib_list_get(list, idx))

#define c_lib_list_size_t c_lib_TYPED_LIST(size_t)
#define c_lib_list_size_t_create() c_lib_list_create()
#define c_lib_list_size_t_push(list, val) c_lib_list_push(list, &(size_t){val})
#define c_lib_list_size_t_pop(list) (*(size_t *)c_lib_list_pop(list))
#define c_lib_list_size_t_get(list, idx) (*(size_t *)c_lib_list_get(list, idx))

#define c_lib_list_double c_lib_TYPED_LIST(double)
#define c_lib_list_double_create() c_lib_list_create()
#define c_lib_list_double_push(list, val) c_lib_list_push(list, &(double){val})
#define c_lib_list_double_pop(list) (*(double *)c_lib_list_pop(list))
#define c_lib_list_double_get(list, idx) (*(double *)c_lib_list_get(list, idx))

#define c_lib_list_char_ptr c_lib_TYPED_LIST(char_ptr)
#define c_lib_list_char_ptr_create() c_lib_list_create()
#define c_lib_list_char_ptr_push(list, val) c_lib_list_push(list, (val))
#define c_lib_list_char_ptr_pop(list) ((char *)c_lib_list_pop(list))
#define c_lib_list_char_ptr_get(list, idx) ((char *)c_lib_list_get(list, idx))

#define c_lib_LIST(T) c_lib_concat(c_lib_list_, T)

#define c_lib_LIST_CREATE(T, ...) \
    _Generic((T *)0, \
        int: c_lib_list_int_create(), \
        size_t: c_lib_list_size_t_create(), \
        double: c_lib_list_double_create(), \
        char *: c_lib_list_char_ptr_create())

#define c_lib_LIST_PUSH(list, val) \
    _Generic((list), \
        c_lib_list *: c_lib_list_push(list, &(typeof(val)){val}))

#define c_lib_LIST_GET(list, idx) \
    (*(typeof(*(typeof(list))0) *)c_lib_list_get(list, idx))

#endif