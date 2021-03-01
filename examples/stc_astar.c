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
mpoint_init(int x, int y, int width)
{
    MazePoint p = { x, y, 0, width }; return p;
}

int
mpoint_compare_priority(const MazePoint* a, const MazePoint* b)
{
    //return 0; // NB! gives 14 steps shorter path!? hmm..
    return (a->priorty > b->priorty) - (a->priorty < b->priorty);
}

int
mpoint_equal(MazePoint* a, MazePoint* b)
{
    return a->x == b->x && a->y == b->y;
}

MazePoint
mpoint_from(cstr maze, const char* c, int width)
{
    int index = cstr_find(maze, c);
    return mpoint_init(index % width, index / width, width);
}

int
mpoint_index(const MazePoint* p)
{
    return p->x + p->width * p->y;
}

int
mpoint_key_compare(const MazePoint* a, const MazePoint* b)
{
    int i = mpoint_index(a);
    int j = mpoint_index(b);
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

using_cdeq(mp, MazePoint, mpoint_compare_priority);
using_cpque(mp, cdeq_mp, >);
using_csmap(ms, MazePoint, MazePoint, mpoint_key_compare); // step
using_csmap(mc, MazePoint, int, mpoint_key_compare); // cost


cdeq_mp
astar(cstr maze, int width)
{
    MazePoint start = mpoint_from(maze, "@", width);
    MazePoint goal = mpoint_from(maze, "!", width);
    cpque_mp frontier = cpque_mp_init();
    csmap_ms came_from = csmap_ms_init();
    csmap_mc cost_so_far = csmap_mc_init();

    cpque_mp_push(&frontier, start);
    csmap_mc_emplace(&cost_so_far, start, 0);

    while (!cpque_mp_empty(frontier))
    {
        MazePoint current = *cpque_mp_top(&frontier);
        cpque_mp_pop(&frontier);
        if (mpoint_equal(&current, &goal))
            break;
        MazePoint deltas[] = {
            { -1, +1, 0, width }, { 0, +1, 0, width }, { 1, +1, 0, width },
            { -1,  0, 0, width }, /* ~ ~ ~ ~ ~ ~ ~ */  { 1,  0, 0, width },
            { -1, -1, 0, width }, { 0, -1, 0, width }, { 1, -1, 0, width },
        };

        c_forrange (i, c_arraylen(deltas))
        {
            MazePoint delta = deltas[i];
            MazePoint next = mpoint_init(current.x + delta.x, current.y + delta.y, width);
            int new_cost = *csmap_mc_at(&cost_so_far, current);
            if (maze.str[mpoint_index(&next)] != '#')
            {
                csmap_mc_value_t* cost = csmap_mc_find(&cost_so_far, next).ref;
                if (!cost || new_cost < cost->second)
                {
                    csmap_mc_emplace_or_assign(&cost_so_far, next, new_cost); // update (put)
                    next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                    cpque_mp_push(&frontier, next);
                    csmap_ms_emplace_or_assign(&came_from, next, current);
                }
            }
        }
    }
    MazePoint current = goal;
    cdeq_mp path = cdeq_mp_init();

    while(!mpoint_equal(&current, &start))
    {
        cdeq_mp_push_front(&path, current);
        current = *csmap_ms_at(&came_from, current);
    }
    cdeq_mp_push_front(&path, start);
    cpque_mp_del(&frontier);
    csmap_ms_del(&came_from);
    csmap_mc_del(&cost_so_far);
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
    cdeq_mp path = astar(maze, width);

    printf("length: %zu\n", cdeq_mp_size(path));

    c_foreach (it, cdeq_mp, path)
        maze.str[mpoint_index(it.ref)] = 'o';

    printf("%s", maze.str);
    cstr_del(&maze);
    cdeq_mp_del(&path);
}