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
} MapPoint;

MapPoint
mpoint_init(int x, int y, int width)
{
    return (MapPoint) { x, y, 0, width };
}

int
mpoint_compare_priority(const MapPoint* a, const MapPoint* b)
{
    return a->priorty < b->priorty;
}

int
mpoint_equal(MapPoint* a, MapPoint* b)
{
    return a->x == b->x && a->y == b->y;
}

MapPoint
mpoint_from(cstr maze, const char* c, int width)
{
    int index = cstr_find(maze, c);
    return mpoint_init(index % width, index / width, width);
}

int
mpoint_index(const MapPoint* p)
{
    return p->x + p->width * p->y;
}

int
mpoint_key_compare(const MapPoint* a, const MapPoint* b)
{
    int i = mpoint_index(a);
    int j = mpoint_index(b);
    return (j < i) - (i < j);
}

typedef struct {
    MapPoint point;
    int value;
} PointCost;

typedef struct {
    MapPoint a;
    MapPoint b;
} MapStep;

int
mstep_key_compare(const MapStep* a, const MapStep* b)
{
    return mpoint_key_compare(&a->a, &b->a);
}

int
pcost_key_compare(const PointCost* a, const PointCost* b)
{
    return mpoint_key_compare(&a->point, &b->point);
}

using_cdeq(pt, MapPoint, mpoint_compare_priority);
using_cpque(pt, cdeq_pt, <);
using_csset(ms, MapStep, mstep_key_compare);
using_csset(pc, PointCost, pcost_key_compare);


cdeq_pt
astar(cstr maze, int width)
{
    MapPoint start = mpoint_from(maze, "@", width);
    MapPoint goal = mpoint_from(maze, "!", width);
    cpque_pt frontier = cpque_pt_init();
    cpque_pt_push(&frontier, start);
    csset_ms came_from = csset_ms_init();
    csset_pc cost_so_far = csset_pc_init();
    csset_pc_insert(&cost_so_far, (PointCost) {start, 0});

    while (!cpque_pt_empty(frontier))
    {
        MapPoint current = *cpque_pt_top(&frontier);
        cpque_pt_pop(&frontier);
        if (mpoint_equal(&current, &goal))
            break;
        MapPoint deltas[] = {
            { -1, +1, 0, width }, { 0, +1, 0, width }, { 1, +1, 0, width },
            { -1,  0, 0, width }, /* ~ ~ ~ ~ ~ ~ ~ */  { 1,  0, 0, width },
            { -1, -1, 0, width }, { 0, -1, 0, width }, { 1, -1, 0, width },
        };

        for (size_t i = 0; i < c_arraylen(deltas); i++)
        {
            MapPoint delta = deltas[i];
            MapPoint next = mpoint_init(current.x + delta.x, current.y + delta.y, width);
            int new_cost = csset_pc_find(&cost_so_far, (PointCost) {.point = current})->value;
            if (maze.str[mpoint_index(&next)] != '#')
            {
                csset_pc_value_t* cost = csset_pc_find(&cost_so_far, (PointCost) {.point = next});
                if (!cost || new_cost < cost->value)
                {
                    csset_pc_insert(&cost_so_far, (PointCost) {next, new_cost});
                    next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                    cpque_pt_push(&frontier, next);
                    csset_ms_insert(&came_from, (MapStep) { next, current });
                }
            }
        }
    }
    MapPoint current = goal;
    cdeq_pt path = cdeq_pt_init();

    while(!mpoint_equal(&current, &start))
    {
        cdeq_pt_push_front(&path, current);
        current = csset_ms_find(&came_from, (MapStep) {.a = current})->b;
    }
    cdeq_pt_push_front(&path, start);
    cpque_pt_del(&frontier);
    csset_ms_del(&came_from);
    csset_pc_del(&cost_so_far);
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
    cdeq_pt path = astar(maze, width);

    printf("length: %zu\n", cdeq_pt_size(path));

    c_foreach (it, cdeq_pt, path)
        maze.str[mpoint_index(it.ref)] = 'c';

    printf("%s", maze.str);
    cstr_del(&maze);
    cdeq_pt_del(&path);
}