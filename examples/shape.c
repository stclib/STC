// Demo of typesafe polymorphism in C99, using STC.

#include <stdlib.h>
#include <stdio.h>
#include <stc/ccommon.h>

#define c_self(s, T) \
    T* self = c_container_of(s, T, base); \
    assert(s->api == &T##_api)

#define c_vtable(Api, T) \
    c_static_assert(offsetof(T, base) == 0); \
    static Api T##_api


// Shape definition
// ============================================================

typedef struct { float x, y; } Point;

typedef struct {
    struct ShapeAPI* api;
    uint32_t color;
    uint16_t style;
    uint8_t thickness;
    uint8_t hardness;
} Shape;

struct ShapeAPI {
    void (*drop)(Shape*);
    void (*draw)(const Shape*);
};

void Shape_drop(Shape* shape)
{
}

void Shape_delete(Shape* shape)
{
    if (shape) {
        shape->api->drop(shape);
        c_free(shape);
    }
}

// Triangle implementation
// ============================================================

typedef struct {
    Shape base;
    Point p[3];
} Triangle;

c_vtable(struct ShapeAPI, Triangle);


static void Triangle_draw(const Shape* shape)
{
    const c_self(shape, Triangle);
    printf("Triangle: (%g,%g), (%g,%g), (%g,%g)\n",
           self->p[0].x, self->p[0].y,
           self->p[1].x, self->p[1].y,
           self->p[2].x, self->p[2].y);
}

static struct ShapeAPI Triangle_api = {
    .drop = Shape_drop,
    .draw = Triangle_draw,
};

Shape* Triangle_new(Point a, Point b, Point c)
{
    Triangle* s = c_new(Triangle, {{.api=&Triangle_api}, .p={a, b, c}});
    return &s->base;
}

// Polygon implementation
// ============================================================

#define i_type PVec 
#define i_val Point
#include <stc/cstack.h>

typedef struct {
    Shape base;
    PVec points;
} Polygon;

c_vtable(struct ShapeAPI, Polygon);


static void Polygon_drop(Shape* shape)
{
    c_self(shape, Polygon);
    puts("drop poly");
    PVec_drop(&self->points);
}

static void Polygon_draw(const Shape* shape)
{
    const c_self(shape, Polygon);
    printf("Polygon:");
    c_foreach (i, PVec, self->points)
        printf(" (%g,%g)", i.ref->x, i.ref->y);
    puts("");
}

static struct ShapeAPI Polygon_api = {
    .drop = Polygon_drop,
    .draw = Polygon_draw,
};

Shape* Polygon_new(void)
{
    Polygon* s = c_new(Polygon, {{.api=&Polygon_api}, .points=PVec_init()});
    return &s->base;
}

void Polygon_addPoint(Shape* shape, Point p)
{
    c_self(shape, Polygon);
    PVec_push(&self->points, p);
}


// Test
// ============================================================

void testShape(Shape* shape)
{
    shape->api->draw(shape);
}

#define i_type Shapes
#define i_val Shape*
#define i_opt c_no_clone
#define i_valdrop(x) Shape_delete(*x)
#include <stc/cstack.h>

int main(void)
{
    c_auto (Shapes, shapes)
    {
        Shape* tria = Triangle_new((Point){5, 7}, (Point){12, 7}, (Point){12, 20});
        Shape* poly = Polygon_new();

        c_apply(p, Polygon_addPoint(poly, p), Point, {
            {5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56},
        });

        Shapes_push(&shapes, tria);
        Shapes_push(&shapes, poly);

        c_foreach (i, Shapes, shapes)
            testShape(*i.ref);
    }
}
