#ifndef C_LIB_COMPAT_MYLIB_H
#define C_LIB_COMPAT_MYLIB_H

#include "../c_lib/base.h"
#include "../c_lib/darray.h"
#include "../c_lib/list.h"
#include "../c_lib/hashmap.h"
#include "../c_lib/rbtree.h"

#ifndef MYLIB_BASE_H
#define MYLIB_BASE_H
#endif

#ifndef MYLIB_DARRAY_H
#define MYLIB_DARRAY_H
#endif

#ifndef MYLIB_LIST_H
#define MYLIB_LIST_H
#endif

#ifndef MYLIB_HASHMAP_H
#define MYLIB_HASHMAP_H
#endif

#ifndef MYLIB_RBTREE_H
#define MYLIB_RBTREE_H
#endif

typedef c_lib_size_t mylib_size_t;
typedef c_lib_ssize_t mylib_ssize_t;

#define mylib_size_t c_lib_size_t
#define mylib_ssize_t c_lib_ssize_t

#define mylib_darray c_lib_darray
#define mylib_darray_create c_lib_darray_create
#define mylib_darray_destroy c_lib_darray_destroy
#define mylib_darray_destroy_with c_lib_darray_destroy_with
#define mylib_darray_clear c_lib_darray_clear
#define mylib_darray_clear_with c_lib_darray_clear_with
#define mylib_darray_push c_lib_darray_push
#define mylib_darray_pop c_lib_darray_pop
#define mylib_darray_peek c_lib_darray_peek
#define mylib_darray_set c_lib_darray_set
#define mylib_darray_get c_lib_darray_get
#define mylib_darray_take c_lib_darray_take
#define mylib_darray_insert c_lib_darray_insert
#define mylib_darray_remove c_lib_darray_remove
#define mylib_darray_erase c_lib_darray_erase
#define mylib_darray_size c_lib_darray_size
#define mylib_darray_capacity c_lib_darray_capacity
#define mylib_darray_is_empty c_lib_darray_is_empty
#define mylib_darray_reserve c_lib_darray_reserve
#define mylib_darray_shrink_to_fit c_lib_darray_shrink_to_fit
#define mylib_darray_sort c_lib_darray_sort
#define mylib_darray_cmp c_lib_darray_cmp
#define mylib_darray_foreach c_lib_darray_foreach

#define MYLIB_DARRAY_DEFAULT_CAPACITY C_LIB_DARRAY_DEFAULT_CAPACITY
#define MYLIB_DARRAY_GROWTH_FACTOR C_LIB_DARRAY_GROWTH_FACTOR
#define MYLIB_DARRAY_FOREACH c_lib_darray_foreach

#define mylib_list c_lib_list
#define mylib_list_node c_lib_list_node
#define mylib_list_create c_lib_list_create
#define mylib_list_destroy c_lib_list_destroy
#define mylib_list_destroy_with c_lib_list_destroy_with
#define mylib_list_clear c_lib_list_clear
#define mylib_list_clear_with c_lib_list_clear_with
#define mylib_list_push c_lib_list_push
#define mylib_list_push_front c_lib_list_push_front
#define mylib_list_push_at c_lib_list_push_at
#define mylib_list_pop c_lib_list_pop
#define mylib_list_pop_front c_lib_list_pop_front
#define mylib_list_pop_at c_lib_list_pop_at
#define mylib_list_get c_lib_list_get
#define mylib_list_find c_lib_list_find
#define mylib_list_remove_node c_lib_list_remove_node
#define mylib_list_remove_data c_lib_list_remove_data
#define mylib_list_count c_lib_list_count
#define mylib_list_is_empty c_lib_list_is_empty
#define mylib_list_reverse c_lib_list_reverse
#define mylib_list_clone c_lib_list_clone
#define mylib_list_merge c_lib_list_merge

#define mylib_list_cmp c_lib_list_cmp
#define mylib_list_free_fn c_lib_list_free_fn
#define MYLIB_LIST_FOREACH c_lib_list_foreach
#define MYLIB_LIST_FOREACH_SAFE c_lib_list_foreach_safe

#define mylib_hashmap c_lib_hashmap
#define mylib_hashmap_create c_lib_hashmap_create
#define mylib_hashmap_destroy c_lib_hashmap_destroy
#define mylib_hashmap_destroy_with c_lib_hashmap_destroy_with
#define mylib_hashmap_clear c_lib_hashmap_clear
#define mylib_hashmap_clear_with c_lib_hashmap_clear_with
#define mylib_hashmap_put c_lib_hashmap_put
#define mylib_hashmap_get c_lib_hashmap_get
#define mylib_hashmap_remove c_lib_hashmap_remove
#define mylib_hashmap_contains c_lib_hashmap_contains
#define mylib_hashmap_count c_lib_hashmap_count
#define mylib_hashmap_is_empty c_lib_hashmap_is_empty
#define mylib_hashmap_foreach c_lib_hashmap_foreach
#define mylib_hashmap_hash_fnv1a c_lib_hashmap_hash_fnv1a
#define mylib_hashmap_hash_djb2 c_lib_hashmap_hash_djb2
#define mylib_hashmap_hash_sdbm c_lib_hashmap_hash_sdbm

#define mylib_hash_fn c_lib_hash_fn
#define mylib_hashmap_cmp c_lib_hashmap_cmp
#define mylib_hashmap_free_fn c_lib_hashmap_free_fn
#define mylib_hashmap_visit_fn c_lib_hashmap_visit_fn

#define mylib_rbtree c_lib_rbtree
#define mylib_rbtree_node c_lib_rbtree_node
#define mylib_rbtree_create c_lib_rbtree_create
#define mylib_rbtree_destroy c_lib_rbtree_destroy
#define mylib_rbtree_clear c_lib_rbtree_clear
#define mylib_rbtree_insert c_lib_rbtree_insert
#define mylib_rbtree_delete c_lib_rbtree_delete
#define mylib_rbtree_search c_lib_rbtree_search
#define mylib_rbtree_update c_lib_rbtree_update
#define mylib_rbtree_min c_lib_rbtree_min
#define mylib_rbtree_max c_lib_rbtree_max
#define mylib_rbtree_count c_lib_rbtree_count
#define mylib_rbtree_is_empty c_lib_rbtree_is_empty
#define mylib_rbtree_inorder c_lib_rbtree_inorder
#define mylib_rbtree_preorder c_lib_rbtree_preorder
#define mylib_rbtree_postorder c_lib_rbtree_postorder
#define mylib_rbtree_lower_bound c_lib_rbtree_lower_bound
#define mylib_rbtree_upper_bound c_lib_rbtree_upper_bound

#define mylib_rbtree_color c_lib_rbtree_color
#define MYLIB_RBTREE_BLACK C_LIB_RBTREE_BLACK
#define MYLIB_RBTREE_RED C_LIB_RBTREE_RED
#define mylib_rbtree_key_cmp c_lib_rbtree_key_cmp
#define mylib_rbtree_visit_fn c_lib_rbtree_visit_fn

#define MYLIB_CONTAINER_OF c_lib_container_of
#define MYLIB_LIST_ENTRY c_lib_list_entry

#define mylib_swap c_lib_swap
#define mylib_min c_lib_min
#define mylib_max c_lib_max
#define mylib_array_size c_lib_array_size

#define MYLIB_LIKELY C_LIB_LIKELY
#define MYLIB_UNLIKELY C_LIB_UNLIKELY
#define MYLIB_UNUSED C_LIB_UNUSED
#define MYLIB_INLINE C_LIB_INLINE
#define MYLIB_HOT C_LIB_HOT
#define MYLIB_COLD C_LIB_COLD

#define MYLIB_HAVE_C23 C_LIB_HAVE_C23
#define MYLIB_NORETURN C_LIB_NORETURN
#define MYLIB_MAYBE_UNUSED C_LIB_MAYBE_UNUSED
#define MYLIB_ALIGNED C_LIB_ALIGNED

#define mylib_static_assert c_lib_static_assert
#define mylib_unreachable c_lib_unreachable
#define mylib_add_overflow c_lib_add_overflow
#define mylib_mul_overflow c_lib_mul_overflow
#define mylib_memcpy c_lib_memcpy
#define mylib_memset c_lib_memset
#define mylib_memcmp c_lib_memcmp
#define mylib_strcmp c_lib_strcmp
#define mylib_strlen c_lib_strlen
#define mylib_memmove c_lib_memmove

#define MYLIB_VERSION_MAJOR C_LIB_VERSION_MAJOR
#define MYLIB_VERSION_MINOR C_LIB_VERSION_MINOR
#define MYLIB_VERSION_PATCH C_LIB_VERSION_PATCH

typedef enum mylib_result {
    MYLIB_SUCCESS = C_LIB_SUCCESS,
    MYLIB_ENOMEM = C_LIB_ENOMEM,
    MYLIB_EINVAL = C_LIB_EINVAL,
    MYLIB_ENOTFOUND = C_LIB_ENOTFOUND,
    MYLIB_EBOUNDS = C_LIB_EBOUNDS,
} mylib_result;

#define mylib_result_from_bool c_lib_result_from_bool

#endif