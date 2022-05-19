// Demo of typesafe polymorphism in C99, using STC.

#include <stdlib.h>
#include <stdio.h>
#include <stc/ccommon.h>

#define c_self(s, T, api) \
    T* self = (T *)s; \
    assert(*s == &T##_##api)

#define c_vtable(Interface, T, api) \
    c_static_assert(offsetof(T, api) == 0); \
    static Interface T##_##api


// Shape definition
// ============================================================

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

c_vtable(Shape, Triangle, api);


static void Triangle_draw(Shape** shape)
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

c_vtable(Shape, Polygon, api);


static void Polygon_drop(Shape** shape)
{
    c_self(shape, Polygon, api);
    puts("drop poly");
    PVec_drop(&self->points);
}

static void Polygon_draw(Shape** shape)
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

#define i_type Shapes
#define i_val Shape**
#define i_opt c_no_clone
#define i_valdrop(x) Shape_delete(*x)
#include <stc/cstack.h>

int main(void)
{
    c_auto (Shapes, shapes)
    {
        Shape** tria = Triangle_new((Point){5, 7}, (Point){12, 7}, (Point){12, 20});
        Shape** poly = Polygon_new();

        c_apply(p, Polygon_addPoint(poly, p), Point, {
            {5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56},
        });

        Shapes_push(&shapes, tria);
        Shapes_push(&shapes, poly);

        c_foreach (i, Shapes, shapes)
            testShape(*i.ref);
    }
}
