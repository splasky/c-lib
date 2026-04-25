#ifndef QUEUE_H_HTFODBQ3
#define QUEUE_H_HTFODBQ3

#include "linked_list.h"
typedef LinkedList Queue;

#define New_Queue New_LinkedList
#define Queue_destory LinkedList_destory
#define Queue_clear LinkedList_clear
#define Queue_clear_destory LinkedList_clear_destory

#define Queue_send LinkedList_push
#define Queue_recv LinkedList_shift

#define Queue_peek LinkedList_first
#define Queue_count LinkedList_count
#define Queue_contain LinkedList_item_exists

#define Queue_foreach list_foreach
#endif /* end of include guard: QUEUE_H_HTFODBQ3 */
