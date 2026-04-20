#include <stdio.h>
#include <stdlib.h>
#include "unit.h"
/* origin 1:wall,0 path */

#define maze_size (8)
static int maze[maze_size][maze_size] = { { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 1 }, { 1, 0, 1, 0, 0, 1, 0, 1 }, { 1, 0, 1, 0, 0, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 1 }, { 1, 1, 0, 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 0, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 }
};

int findPath(int x, int y)
{
    /* congrulation!
     * finish.
     */

    if (x == 0 && y == 0)
    {
        maze[x][y] = 2;
        return 1;
    }

    else if (x >= 0 && x < maze_size && y >= 0 && y < maze_size && maze[x][y] == 0)
    {
        maze[x][y] = 2;
        if ((findPath(x - 1, y) + findPath(x + 1, y) + findPath(x, y - 1)
                + findPath(x, y + 1))
                > 0)
        {
            return 1;
        }
        else
        {
            /* go back */
            maze[x][y] = 0;
            return 0;
        }
    }

    return 0;
}

void print_maze(void)
{
    for (int i = 0; i < maze_size; i++)
    {
        for (int j = 0; j < maze_size; j++)
        {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }
}

TEST(test_maze)
{
    printf("origin:\n");
    print_maze();
    printf("after:\n");
    findPath(6, 6);
    print_maze();
    return NULL;
}

TEST(all_tests)
{
    unit_suite_start();
    unit_run_test(test_maze);
    return NULL;
}

RUN_TESTS(all_tests);
