//
// -- An A* pathfinder inspired by the excellent tutorial at Red Blob Games --
//
// This is a reimplementation of the CTL example to STC:
//     https://github.com/glouw/ctl/blob/master/examples/astar.c
//     https://www.redblobgames.com/pathfinding/a-star/introduction.html
#define i_implement
#include "stc/cstr.h"
#include <stdio.h>

typedef struct
{
    int x;
    int y;
    int priorty;
    int width;
}
point;

point
point_init(int x, int y, int width)
{
    return c_LITERAL(point){ x, y, 0, width };
}

int
point_cmp_priority(const point* a, const point* b)
{
    return c_default_cmp(&a->priorty, &b->priorty);
}

int
point_equal(const point* a, const point* b)
{
    return a->x == b->x && a->y == b->y;
}

point
point_from(const cstr* maze, const char* c, int width)
{
    int index = (int)cstr_find(maze, c);
    return point_init(index % width, index / width, width);
}

int
point_index(const point* p)
{
    return p->x + p->width * p->y;
}

int
point_key_cmp(const point* a, const point* b)
{
    int i = point_index(a);
    int j = point_index(b);
    return (i == j) ? 0 : (i < j) ? -1 : 1;
}

#define i_TYPE pque_point, point
#define i_cmp point_cmp_priority
#include "stc/pque.h"

#define i_TYPE deq_point, point
#include "stc/deq.h"

#define i_TYPE smap_pcost, point, int
#define i_cmp point_key_cmp
#include "stc/smap.h"

#define i_TYPE smap_pstep, point, point
#define i_cmp point_key_cmp
#include "stc/smap.h"

deq_point
astar(cstr* maze, int width)
{
    deq_point ret_path = {0};

    pque_point front = {0};
    smap_pstep from = {0};
    smap_pcost costs = {0};
    c_defer(
        pque_point_drop(&front),
        smap_pstep_drop(&from),
        smap_pcost_drop(&costs)
    ){
        point start = point_from(maze, "@", width);
        point goal = point_from(maze, "!", width);
        smap_pcost_insert(&costs, start, 0);
        pque_point_push(&front, start);
        while (!pque_point_empty(&front))
        {
            point current = *pque_point_top(&front);
            pque_point_pop(&front);
            if (point_equal(&current, &goal))
                break;
            point deltas[] = {
                { -1, +1, 0, width }, { 0, +1, 0, width }, { 1, +1, 0, width },
                { -1,  0, 0, width }, /* ~ ~ ~ ~ ~ ~ ~ */  { 1,  0, 0, width },
                { -1, -1, 0, width }, { 0, -1, 0, width }, { 1, -1, 0, width },
            };
            for (size_t i = 0; i < c_arraylen(deltas); i++)
            {
                point delta = deltas[i];
                point next = point_init(current.x + delta.x, current.y + delta.y, width);
                int new_cost = *smap_pcost_at(&costs, current);
                if (cstr_str(maze)[point_index(&next)] != '#')
                {
                    const smap_pcost_value *cost = smap_pcost_get(&costs, next);
                    if (cost == NULL || new_cost < cost->second)
                    {
                        smap_pcost_insert(&costs, next, new_cost);
                        next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                        pque_point_push(&front, next);
                        smap_pstep_insert(&from, next, current);
                    }
                }
            }
        }
        point current = goal;
        while (!point_equal(&current, &start))
        {
            deq_point_push_front(&ret_path, current);
            current = *smap_pstep_at(&from, current);
        }
        deq_point_push_front(&ret_path, start);
    }
    return ret_path;
}

int
main(void)
{
    cstr maze = cstr_lit(
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
        "#########################################################################\n"
    );
    int width = (int)cstr_find(&maze, "\n") + 1;
    deq_point ret_path = astar(&maze, width);

    c_foreach (it, deq_point, ret_path)
        cstr_data(&maze)[point_index(it.ref)] = 'x';

    printf("%s", cstr_str(&maze));

    deq_point_drop(&ret_path);
    cstr_drop(&maze);
}
