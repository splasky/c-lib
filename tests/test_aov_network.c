#include "../include/dbg.h"
#include "../include/graph.h"
#include "../include/queue.h"
#include "unit.h"
#include <limits.h>
#include <stdio.h>

static int int_compare(const void* a, const void* b)
{
    const int x = *((int*)a);
    const int y = *((int*)b);
    return (x > y) - (y > x);
}

static void print_graph_cb(const void* value) { printf("%d\t", *(int*)value); }

static void count_predcessor(
    Queue* queue, int* immediate_predecessor, int num_of_vertices)
{
    for (int i = 1; i < num_of_vertices; i++) {
        if (immediate_predecessor[i] == 0) {
            int* source = malloc(sizeof(int));
            *source = i;
            Queue_send(queue, source);
        }
    }
}

int topological_sort(Graph* graph)
{
    int immediate_predecessor[graph->num_of_vertices + 1];
    for (int i = 0; i < graph->num_of_vertices + 1; i++) {
        immediate_predecessor[i] = 0;
    }

    /* preserve immediate_predecessor[0] */
    immediate_predecessor[0] = INT_MAX;
    int* vertex = 0;
    Queue* queue = New_Queue();

    /* fill the degree of branch */
    for (AdjList* list = graph->adjlist; list != NULL; list = list->next) {
        for (AdjListNode* node = list->head; node != NULL; node = node->next) {
            immediate_predecessor[*(int*)node->dest]++;
        }
    }
    count_predcessor(queue, immediate_predecessor, graph->num_of_vertices + 1);

    while (Queue_count(queue)) {
        vertex = (int*)Queue_recv(queue);
        printf("%d->", *vertex);
        AdjList* list = Graph_find_AdjList(graph, vertex);
        for (AdjListNode* node = list->head; node != NULL; node = node->next) {
            intptr_t* dest = node->dest;
            immediate_predecessor[*dest]--;
            if (immediate_predecessor[*dest] == 0) {
                int* source = malloc(sizeof(int));
                *source = *dest;
                Queue_send(queue, source);
            }
        }
        free(vertex);
    }
    Queue_clear_destory(queue);

    for (int i = 1; i < graph->num_of_vertices + 1; i++) {
        if (immediate_predecessor[i] != 0) {
            return 1;
        }
    }

    return 0;
}

TEST(test_aov_network)
{
    Graph* graph1 = Graph_create(int_compare);
    Graph* graph2 = Graph_create(int_compare);

    int edge[10][3] = { { 3, 2 }, { 2, 1 }, { 2, 5 }, { 2, 6 }, { 1, 4 }, { 5, 4 },
        { 7, 4 }, { 6, 7 }, { 5, 6 }, { 7, 5 } };

    /* create graph1 */
    for (int i = 0; i < 8; ++i) {
        Graph_add_edge(graph1, &edge[i][0], &edge[i][1], 1);
    }
    Graph_print_graph(graph1, print_graph_cb);

    if (topological_sort(graph1)) {
        printf("graph1 has cycle\n");
    } else {
        printf("graph1 doesn't has cycle\n");
    }

    /* create graph 2 */
    for (int i = 0; i < 10; ++i) {
        Graph_add_edge(graph2, &edge[i][0], &edge[i][1], 1);
    }
    Graph_print_graph(graph2, print_graph_cb);

    if (topological_sort(graph2)) {
        printf("graph2 has cycle\n");
    } else {
        printf("graph2 doesn't has cycle\n");
    }

    Graph_destory(graph1);
    Graph_destory(graph2);
    return NULL;
}

TEST(all_tests)
{
    unit_suite_start();
    unit_run_test(test_aov_network);
    return NULL;
}

RUN_TESTS(all_tests)
