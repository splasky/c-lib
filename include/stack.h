#ifndef STACK_H_LL2XHNA8
#define STACK_H_LL2XHNA8

#include "linked_list.h"
typedef LinkedList Stack;

#define New_Stack New_LinkedList
#define Stack_destory LinkedList_destory
#define Stack_clear LinkedList_clear
#define Stack_clear_destory LinkedList_clear_destory

#define Stack_push LinkedList_push
#define Stack_pop LinkedList_pop
#define Stack_peek LinkedList_last

#define Stack_count LinkedList_count
#define stack_foreach list_foreach
#endif /* end of include guard: STACK_H_LL2XHNA8 */
