#include "../include/dbg.h"
#include "../include/linked_list.h"
#include "unit.h"

#define MAX_VERTICES 6

typedef struct edge_t Edge;
struct edge_t {
    int from;
    int to;
    int weight;
};

int vertex[MAX_VERTICES];

void create_edge(LinkedList* list, int len, int* edge);
void minSpanTree(LinkedList* list);
void addSet(int from, int to);
int isSameSet(int from, int to);

void create_edge(LinkedList* list, int len, int* edge)
{
    Edge* newnode = NULL;
    for (int i = 0; i < len; i++) {
        newnode = malloc(sizeof(Edge));
        newnode->from = edge[3 * i];
        newnode->to = edge[3 * i + 1];
        newnode->weight = edge[3 * i + 2];
        LinkedList_push(list, newnode);
    }
}

void addSet(int from, int to)
{
    int to_root = to;

    while (vertex[to_root] > 0) {
        to_root = vertex[to_root];
    }

    vertex[to_root] = from;
}

int isSameSet(int from, int to)
{
    int from_root = from;
    int to_root = to;

    while (vertex[from_root] > 0) {
        from_root = vertex[from_root];
    }

    while (vertex[to_root] > 0) {
        to_root = vertex[to_root];
    }

    if (from_root == to_root) {
        return 1;
    }

    return 0;
}

void minSpanTree(LinkedList* list)
{
    ListNode* ptr = LinkedList_first(list);
    int total = 0;

    /* init graph */
    for (int i = 1; i < MAX_VERTICES; ++i) {
        vertex[i] = -1;
    }

    while (ptr != NULL) {
        if (!isSameSet(((Edge*)ptr->value)->from, ((Edge*)ptr->value)->to)) {
            printf("vertex: %d -> %d cost:%d\n", ((Edge*)ptr->value)->from,
                ((Edge*)ptr->value)->to, ((Edge*)ptr->value)->weight);
            total += ((Edge*)ptr->value)->weight;
            addSet(((Edge*)ptr->value)->from, ((Edge*)ptr->value)->to);
        }
        ptr = ptr->next;
    }

    printf("total:%d\n", total);
}

TEST(test_spanning_tree)
{
    int edge[8][3] = { { 1, 2, 2 }, { 2, 4, 3 }, { 1, 4, 4 }, { 3, 5, 5 }, { 2, 5, 6 },
        { 2, 3, 8 }, { 3, 4, 10 }, { 4, 5, 15 } };

    LinkedList* list = New_LinkedList();
    create_edge(list, 8, &edge[0][0]);
    minSpanTree(list);
    printf("show vertexs\n");
    for (int i = 1; i < MAX_VERTICES; ++i) {
        printf("[%d]", vertex[i]);
    }

    printf("\n");
    return NULL;
}

TEST(all_tests)
{
    unit_suite_start();
    unit_run_test(test_spanning_tree);
    return NULL;
}

RUN_TESTS(all_tests);
