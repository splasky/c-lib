#ifndef MYLIB_LIST_H
#define MYLIB_LIST_H

#include "compat.h"

#define MYLIB_LIST_FOREACH(list, node) \
    for (c_lib_list_node *node = (list)->head; node != NULL; node = node->next)

#define MYLIB_LIST_FOREACH_SAFE(list, node, tmp) \
    for (c_lib_list_node *node = (list)->head, *tmp = node ? node->next : NULL; \
         node != NULL; \
         node = tmp, tmp = tmp ? tmp->next : NULL)

#define MYLIB_LIST_ENTRY(ptr, type, member) c_lib_container_of(ptr, type, member)

#endif