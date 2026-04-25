#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct ListNode ListNode;
typedef struct LinkedList_t LinkedList;

struct ListNode
{
    void* value;
    ListNode* prev;
    ListNode* next;
};

struct LinkedList_t
{
    int count;
    ListNode* tail;
    ListNode* head;
};

typedef struct LinkedList_Iterator_t
{
    LinkedList* list;
    ListNode* position;
    uint64_t index;
} LinkedList_Iterator;

#define LinkedList_count(L) ((L)->count)
#define LinkedList_first(L) (((L)->head != NULL) ? (L)->head : NULL)
#define LinkedList_last(L) (((L)->tail) != NULL ? (L)->tail : NULL)

LinkedList* New_LinkedList(void);
void LinkedList_destory(LinkedList* list);
void LinkedList_clear(LinkedList* list);
void LinkedList_clear_destory(LinkedList* list);
void LinkedList_push(LinkedList* list, void* value);
void* LinkedList_pop(LinkedList* list);
void* LinkedList_shift(LinkedList* list);
void* LinkedList_remove(LinkedList* list, ListNode* node);
void* LinkedList_remove_index(LinkedList* list, const int index);
void LinkedList_addFirst(LinkedList* list, void* value);
void LinkedList_addWithIndex(LinkedList* list, const int index, void* value);
void LinkedList_addALL(LinkedList* list, LinkedList* added);
void LinkedList_addALLWithIndex(LinkedList* list, const int index, LinkedList* listadd);
void* LinkedList_get(LinkedList* list, const int index);
/* TODO:need test */
typedef int (*LinkedList_Comparator)(const void* a, const void* b);

int LinkedList_item_exists(
    LinkedList* list, void* value, LinkedList_Comparator comparator);
LinkedList* LinkedList_deep_copy(LinkedList* list);
void* LinkedList_find_value(
    LinkedList* list, void* key, LinkedList_Comparator comparator);
ListNode* LinkedList_find_node(
    LinkedList* list, void* toFind, LinkedList_Comparator comparator);
void LinkedList_remove_data(
    LinkedList* list, void* data, LinkedList_Comparator comparator);
/* need test */

void* LinkedList_Iterate(LinkedList_Iterator* iterator);
void LinkedList_InitializeIterator(LinkedList_Iterator* iterator, LinkedList* list);
void LinkedList_ResetIterator(LinkedList_Iterator* iterator);

/* L is list , pos is current node name
 * retrive data by post->data
 * */
#define list_foreach(L,pos)\
    for(ListNode* pos=L->head;pos!=NULL;pos=pos->next)

#endif /* ifndef LIST_H */
