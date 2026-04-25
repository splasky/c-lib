#include "../include/linked_list.h"
#include "../include/dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline bool LinkedList_is_Over_Bound(LinkedList* list, const int index);
static inline ListNode* LinkedList_getNodeFromIndex(LinkedList* list, const int index);
static inline int ListNode_item_exists(
    ListNode* node, void* value, LinkedList_Comparator comparator);

static inline ListNode* LinkedList_getNodeFromIndex(LinkedList* list, const int index)
{
    if (LinkedList_is_Over_Bound(list, index))
    {
        goto error;
    }

    ListNode* curr;
    int i = 0;
    for (curr = list->head; curr != NULL; curr = curr->next)
    {
        if (i == index)
            break;
        ++i;
    }

    return curr;
error:
    return NULL;
}

static inline bool LinkedList_is_Over_Bound(LinkedList* list, const int index)
{
    if (index > LinkedList_count(list) || LinkedList_count(list) < 0)
    {
        log_err(
            "Insert into linked list out of bound(linked list length:%d index : %d)\n ",
            LinkedList_count(list), index);
        return true;
    }

    return false;
}

static inline int ListNode_item_exists(
    ListNode* node, void* value, LinkedList_Comparator comparator)
{
    if (node == NULL)
    {
        return 0;
    }

    if (comparator(value, node->value))
    {
        return 1;
    }

    return ListNode_item_exists(node->next, value, comparator);
}

LinkedList* New_LinkedList(void)
{
    LinkedList* list = calloc(1, sizeof(LinkedList));
    check_mem(list);
    return list;
error:
    return NULL;
}

void LinkedList_destory(LinkedList* list)
{
    for (ListNode* curr = list->head; curr != NULL; curr = curr->next)
    {
        if (curr->prev)
        {
            free(curr->prev);
            curr->prev = NULL;
        }
    }

    if (LinkedList_last(list) != NULL)
    {
        free(LinkedList_last(list));
        list->tail = NULL;
    }

    if (list)
    {
        free(list);
        list = NULL;
    }
}

void LinkedList_clear(LinkedList* list)
{
    for (ListNode* curr = list->head; curr != NULL; curr = curr->next)
    {
        if (curr->value)
        {
            free(curr->value);
            curr->value = NULL;
        }
    }
}

void LinkedList_clear_destory(LinkedList* list)
{
    LinkedList_clear(list);
    LinkedList_destory(list);
}

void LinkedList_push(LinkedList* list, void* value)
{
    ListNode* node = calloc(1, sizeof(ListNode));
    check_mem(node);
    node->value = value;

    if (LinkedList_last(list) == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;
error:
    return;
}

void* LinkedList_pop(LinkedList* list)
{
    ListNode* last = LinkedList_last(list);
    return (last != NULL) ? LinkedList_remove(list, last) : NULL;
}

void* LinkedList_shift(LinkedList* list)
{
    return (list->head != NULL) ? LinkedList_remove(list, list->head) : NULL;
}

void LinkedList_addFirst(LinkedList* list, void* value)
{
    ListNode* node = calloc(1, sizeof(ListNode));
    check_mem(node);
    node->value = value;

    if (LinkedList_first(list) == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        node->next = LinkedList_first(list);
        LinkedList_first(list)->prev = node;
        list->head = node;
    }
    list->count++;
error:
    return;
}

void* LinkedList_remove(LinkedList* list, ListNode* node)
{
    void* result = NULL;
    check(LinkedList_first(list) && LinkedList_last(list), "List is empty");
    check(node, "Node can't be NULL");

    if (LinkedList_first(list) == node && LinkedList_last(list) == node)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else if (LinkedList_first(list) == node)
    {
        list->head = list->head->next;
        check(list->head != NULL, "Invalid list, get a first that is a NULL");
        list->head->prev = NULL;
    }
    else if (LinkedList_last(list) == node)
    {
        list->tail = list->tail->prev;
        check(list->tail != NULL, "Invalid list, get a last that is a NULL");
        list->tail->next = NULL;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

    list->count--;
    result = node->value;
    free(node);
    node = NULL;

error:
    return result;
}

void LinkedList_addWithIndex(LinkedList* list, const int index, void* value)
{

    ListNode* index_node = LinkedList_getNodeFromIndex(list, index);
    check(index_node, "get node error");

    if (LinkedList_first(list) == NULL)
    {
        LinkedList_push(list, value);
        return;
    }

    ListNode* node = calloc(1, sizeof(ListNode));
    node->value = value;

    node->prev = index_node->prev;
    node->next = index_node;
    index_node->prev->next = node;
    index_node->prev = node;

    list->count++;
error:
    return;
}

void LinkedList_addALL(LinkedList* list, LinkedList* added)
{
    LinkedList_addALLWithIndex(list, (LinkedList_count(list) - 1), added);
}

void LinkedList_addALLWithIndex(LinkedList* list, const int index, LinkedList* added)
{
    if (added->head == NULL)
    {
        goto error;
    }

    ListNode* index_node = LinkedList_getNodeFromIndex(list, index);
    check(index_node, "Merge two linkedlist failed");

    if (index_node->next == NULL)
    {
        index_node->next = added->head;
        added->head->prev = index_node;
    }
    else if (LinkedList_count(list) > 0 && LinkedList_count(added) > 0)
    {
        index_node->next->prev = added->tail;
        added->tail->next = index_node->next;
        index_node->next = added->head;
        added->head->prev = index_node;
    }
    else if (LinkedList_first(list) == NULL)
    {
        list->head = added->head;
        list->tail = added->tail;
    }

    list->count += added->count;
error:
    return;
}

void* LinkedList_remove_index(LinkedList* list, const int index)
{
    ListNode* index_node = LinkedList_getNodeFromIndex(list, index);
    check(index_node, "Remove from index failed");
    return LinkedList_remove(list, index_node);
error:
    debug("list count:%d ,index:%d", LinkedList_count(list), index);
    return NULL;
}

void* LinkedList_get(LinkedList* list, const int index)
{
    ListNode* node = LinkedList_getNodeFromIndex(list, index);
    check(node, "can't get index in list");
    return node->value;

error:
    return NULL;
}

int LinkedList_item_exists(
    LinkedList* list, void* value, LinkedList_Comparator comparator)
{
    if (list == NULL || list->head == NULL)
    {
        return -1;
    }

    ListNode* curr = LinkedList_first(list);
    return ListNode_item_exists(curr, value, comparator);
}

LinkedList* LinkedList_deep_copy(LinkedList* list)
{
    if (!list)
    {
        return NULL;
    }
    LinkedList* new_list = New_LinkedList();

    for (ListNode* curr = list->head; curr != NULL; curr = curr->next)
    {
        if (curr && curr->value)
        {
            void* value = calloc(1, sizeof(curr->value));
            value = curr->value;
            LinkedList_push(new_list, value);
        }
    }

    return new_list;
}

void* LinkedList_find_value(LinkedList* list, void* key, LinkedList_Comparator comparator)
{
    check(list, "List is NULL.");
    check(key, "Key is NULL.");

    ListNode* node = LinkedList_find_node(list, key, comparator);
    if (node)
    {
        return node->value;
    }

error:
    log_info("Key not found.");
    return NULL;
}

ListNode* LinkedList_find_node(
    LinkedList* list, void* toFind, LinkedList_Comparator comparator)
{
    check(list, "List is NULL.");
    check(toFind, "Key is NULL.");

    ListNode* curr = list->head;
    while (curr != NULL)
    {
        if (comparator(curr->value, toFind) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }

error:
    return NULL;
}

void LinkedList_remove_data(
    LinkedList* list, void* data, LinkedList_Comparator comparator)
{
    ListNode* node = LinkedList_find_node(list, data, comparator);
    if (node)
    {
        LinkedList_remove(list, node);
    }
}

void* LinkedList_Iterate(LinkedList_Iterator* iterator)
{
    check(iterator, "Iterator is NULL.");
    void* data = NULL;

    if (iterator->position && iterator->list->count != 0)
    {
        data = iterator->position->value;
        iterator->position = iterator->position->next;
        iterator->index++;
    }

    return data;
error:
    return NULL;
}

void LinkedList_InitializeIterator(LinkedList_Iterator* iterator, LinkedList* list)
{
    check(iterator != NULL, "Iterator is NULL");
    check(list != NULL, "LinkedList is NULL");

    iterator->index = 0;
    iterator->list = list;
    iterator->position = list->head;
    return;

error:
    log_err("InitializeIterator failed");
}

void LinkedList_ResetIterator(LinkedList_Iterator* iterator)
{
    check(iterator != NULL, "Iterator is NULL");
    check(iterator->list != NULL, "List is NULL");

    iterator->index = 0;
    iterator->position = iterator->list->head;
    return;

error:
    log_err("Reset iterator failed");
}
