// Demo of typesafe polymorphism in C99, using STC.

#include <stdlib.h>
#include <stdio.h>
#include <stc/ccommon.h>

#define c_self(s, T, vtable) \
    T* self = c_container_of(s, T, vtable)
#define c_is_first(T, memb) \
    c_static_assert(offsetof(T, memb) == 0)


typedef struct { float x, y; } Point;

typedef struct Shape {
    void (*drop)(struct Shape**);
    void (*draw)(struct Shape**);
} Shape;

void Shape_drop(Shape** shape)
{
}

void Shape_delete(Shape** shape)
{ 
    if (shape) {
        (*shape)->drop(shape);
        c_free(shape);
    }
}

// Triangle implementation
// ============================================================

typedef struct {
    Shape* api;
    Point p[3];
} Triangle;

c_is_first(Triangle, api);


void Triangle_draw(Shape** shape)
{
    const c_self(shape, Triangle, api);
    printf("Triangle: (%g,%g), (%g,%g), (%g,%g)\n",
           self->p[0].x, self->p[0].y,
           self->p[1].x, self->p[1].y,
           self->p[2].x, self->p[2].y);
}

static Shape Triangle_api = {
    .drop = Shape_drop,
    .draw = Triangle_draw,
};

Shape** Triangle_new(Point a, Point b, Point c)
{
    Triangle* s = c_new(Triangle, {&Triangle_api, .p={a, b, c}});
    return &s->api;
}

// Polygon implementation
// ============================================================

#define i_type PVec 
#define i_val Point
#include <stc/cstack.h>

typedef struct {
    Shape* api;
    PVec points;
} Polygon;

c_is_first(Polygon, api);


void Polygon_drop(Shape** shape)
{
    puts("drop poly");
    c_self(shape, Polygon, api);
    PVec_drop(&self->points);
}

void Polygon_draw(Shape** shape)
{
    const c_self(shape, Polygon, api);
    printf("Polygon:");
    c_foreach (i, PVec, self->points)
        printf(" (%g,%g)", i.ref->x, i.ref->y);
    puts("");
}

static Shape Polygon_api = {
    .drop = Polygon_drop,
    .draw = Polygon_draw,
};

Shape** Polygon_new(void)
{
    Polygon* s = c_new(Polygon, {&Polygon_api, .points=PVec_init()});
    return &s->api;
}

void Polygon_addPoint(Shape** shape, Point p)
{
    c_self(shape, Polygon, api);
    PVec_push(&self->points, p);
}


// Test
// ============================================================

void testShape(Shape** shape)
{
    (*shape)->draw(shape);
}

int main(void)
{
    c_autovar (Shape** tria = NULL, Shape_delete(tria))
    c_autovar (Shape** poly = Polygon_new(), Shape_delete(poly))
    {
        tria = Triangle_new((Point){5, 7}, (Point){12, 7}, (Point){12, 20});

        c_apply(p, Polygon_addPoint(poly, p), Point, {
            {5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56},
        });

        testShape(tria);
        testShape(poly);
    }
}
