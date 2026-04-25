#include "../include/avltree.h"
#include "unit.h"

static int compare(const void* a, const void* b)
{
    if (*(int*)a > *(int*)b) {
        return 1;
    } else if (*(int*)a == *(int*)b) {
        return 0;
    }
    return -1;
}

static void print(const AVLTreeNode* root) { printf("%d\n", *(int*)root->data); }

TEST(test_avltree)
{
    const int n = 10;
    const int randRange = 1000;
    const int randTrials = 10000;
    AVLTreeNode* root = TREE_EMPTY;

    int storage[1000];
    for (int i = 0; i < 1000; ++i) {
        storage[i] = i;
    }

    for (int i = 0; i < n; ++i) {
        unit_assert(!avltreeContains(root, &storage[i]), "Test not contains failed");
        avltreeInsert(&root, &storage[i], &storage[i], compare);
        unit_assert(avltreeContains(root, &storage[i]), "Test not contains failed");
        avltreeSanityCheck(root);
        avltreePrint(root, print);
        printf("---\n");
    }

    /* delete everything */
    for (int i = 0; i < n; ++i) {
        unit_assert(avltreeContains(root, &storage[i]), "Test contains failed");
        avltreeDelete(&root, &storage[i]);
        unit_assert(!avltreeContains(root, &storage[i]), "Test delete failed");
        avltreeSanityCheck(root);
        avltreePrint(root, print);
        printf("---\n");
    }

    avltreeSanityCheck(root);
    avltreeDestroy(&root);

    srand(1);
    for (int i = 0; i < randTrials; i++) {
        int r = rand() % randRange;
        avltreeInsert(&root, &storage[r], &storage[r], compare);
        avltreeDelete(&root, &storage[r]);
    }

    avltreeSanityCheck(root);
    avltreeDestroy(&root);

    return NULL;
}

TEST(all_tests)
{
    unit_suite_start();
    unit_run_test(test_avltree);
    return NULL;
}

RUN_TESTS(all_tests);
