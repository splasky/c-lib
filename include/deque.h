#ifndef deque_H_X6402
#define deque_H_X6402
#include "linked_list.h"

typedef LinkedList Deque;

#define New_Deque New_LinkedList
#define Deque_destory LinkedList_destory
#define Deque_clear LinkedList_clear
#define Deque_clear_destory LinkedList_clear_destory

#define Deque_push_back LinkedList_push
#define Deque_push_front LinkedList_addFirst
#define Deque_pop_back LinkedList_pop
#define Deque_pop_front LinkedList_shift

#define Deque_peek_front(L) LinkedList_first(L)->value
#define Deque_peek_back(L) LinkedList_last(L)->value

#define Deque_count LinkedList_count

#endif /* ifndef deque_H_X6402 */
