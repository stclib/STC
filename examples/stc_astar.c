//
// -- An A* pathfinder inspired by the excellent tutorial at Red Blob Games --
//
// See:
//     https://www.redblobgames.com/pathfinding/a-star/introduction.html

#include <stc/cstr.h>
#include <stc/cpque.h>
#include <stc/cdeq.h>
#include <stc/csmap.h>

#include <stdio.h>

typedef struct {
    int x, y;
    int priorty, width;
} MazePoint;

MazePoint
mpnt_init(int x, int y, int width)
{
    return c_make(MazePoint){ x, y, 0, width };
}

int
mpnt_compare_priority(const MazePoint* a, const MazePoint* b)
{
    //return 0; // NB! gives 14 steps shorter path!? hmm..
    return c_default_compare(&b->priorty, &a->priorty); // note: high priority is low cost
}

int
mpnt_equal(MazePoint* a, MazePoint* b)
{
    return a->x == b->x && a->y == b->y;
}

MazePoint
mpnt_from(cstr maze, const char* c, int width)
{
    int index = cstr_find(maze, c);
    return mpnt_init(index % width, index / width, width);
}

int
mpnt_index(const MazePoint* p)
{
    return p->x + p->width * p->y;
}

int
mpnt_key_compare(const MazePoint* a, const MazePoint* b)
{
    int i = mpnt_index(a);
    int j = mpnt_index(b);
    return (i > j) - (i < j);
}

typedef struct {
    MazePoint point;
    int value;
} MazeCost;

typedef struct {
    MazePoint a;
    MazePoint b;
} MazeStep;

using_cdeq(pnt, MazePoint, c_no_compare);
using_cpque(pnt, cdeq_pnt, mpnt_compare_priority);
using_csmap(step, MazePoint, MazePoint, mpnt_key_compare);
using_csmap(cost, MazePoint, int, mpnt_key_compare);


cdeq_pnt
astar(cstr maze, int width)
{
    MazePoint start = mpnt_from(maze, "@", width);
    MazePoint goal = mpnt_from(maze, "!", width);

    cdeq_pnt path = cdeq_pnt_init(); // returned
    
    cpque_pnt frontier = cpque_pnt_init();
    csmap_step came_from = csmap_step_init();
    csmap_cost cost_so_far = csmap_cost_init();
    c_fordefer (cpque_pnt_del(&frontier),
                csmap_step_del(&came_from),
                csmap_cost_del(&cost_so_far))
    {
        cpque_pnt_push(&frontier, start);
        csmap_cost_insert(&cost_so_far, start, 0);

        while (!cpque_pnt_empty(frontier))
        {
            MazePoint current = *cpque_pnt_top(&frontier);
            cpque_pnt_pop(&frontier);
            if (mpnt_equal(&current, &goal))
                break;
            MazePoint deltas[] = {
                { -1, +1, 0, width }, { 0, +1, 0, width }, { 1, +1, 0, width },
                { -1,  0, 0, width }, /* ~ ~ ~ ~ ~ ~ ~ */  { 1,  0, 0, width },
                { -1, -1, 0, width }, { 0, -1, 0, width }, { 1, -1, 0, width },
            };

            c_forrange (i, c_arraylen(deltas))
            {
                MazePoint delta = deltas[i];
                MazePoint next = mpnt_init(current.x + delta.x, current.y + delta.y, width);
                int new_cost = *csmap_cost_at(&cost_so_far, current);
                if (maze.str[mpnt_index(&next)] != '#')
                {
                    csmap_cost_value_t* cost = csmap_cost_find(&cost_so_far, next).ref;
                    if (!cost || new_cost < cost->second)
                    {
                        csmap_cost_put(&cost_so_far, next, new_cost); // update
                        next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                        cpque_pnt_push(&frontier, next);
                        csmap_step_put(&came_from, next, current);
                    }
                }
            }
        }
        MazePoint current = goal;
        while(!mpnt_equal(&current, &start))
        {
            cdeq_pnt_push_front(&path, current);
            current = *csmap_step_at(&came_from, current);
        }
        cdeq_pnt_push_front(&path, start);
    }
    return path;
}

int
main(void)
{
    cstr maze = cstr_from(
        "#########################################################################\n"
        "#   #               #               #           #                   #   #\n"
        "#   #   #########   #   #####   #########   #####   #####   #####   # ! #\n"
        "#               #       #   #           #           #   #   #       #   #\n"
        "#########   #   #########   #########   #####   #   #   #   #########   #\n"
        "#       #   #               #           #   #   #   #   #           #   #\n"
        "#   #   #############   #   #   #########   #####   #   #########   #   #\n"
        "#   #               #   #   #       #           #           #       #   #\n"
        "#   #############   #####   #####   #   #####   #########   #   #####   #\n"
        "#           #       #   #       #   #       #           #   #           #\n"
        "#   #####   #####   #   #####   #   #########   #   #   #   #############\n"
        "#       #       #   #   #       #       #       #   #   #       #       #\n"
        "#############   #   #   #   #########   #   #####   #   #####   #####   #\n"
        "#           #   #           #       #   #       #   #       #           #\n"
        "#   #####   #   #########   #####   #   #####   #####   #############   #\n"
        "#   #       #           #           #       #   #   #               #   #\n"
        "#   #   #########   #   #####   #########   #   #   #############   #   #\n"
        "#   #           #   #   #   #   #           #               #   #       #\n"
        "#   #########   #   #   #   #####   #########   #########   #   #########\n"
        "#   #       #   #   #           #           #   #       #               #\n"
        "# @ #   #####   #####   #####   #########   #####   #   #########   #   #\n"
        "#   #                   #           #               #               #   #\n"
        "#########################################################################\n");
    int width = cstr_find(maze, "\n") + 1;
    cdeq_pnt path = astar(maze, width);

    printf("length: %zu\n", cdeq_pnt_size(path));

    c_foreach (it, cdeq_pnt, path)
        maze.str[mpnt_index(it.ref)] = 'x';

    printf("%s", maze.str);
    cstr_del(&maze);
    cdeq_pnt_del(&path);
}