// Demo of typesafe polymorphism in C99, using STC.

#include <stdlib.h>
#include <stdio.h>
#include <stc/ccommon.h>

#define c_dyn_ptr(T, s) \
    (&T##_api == (s)->api ? (T*)(s) : (T*)0)

#define c_vtable(Api, T) \
    c_static_assert(offsetof(T, base) == 0); \
    static Api T##_api

// Shape definition
// ============================================================

typedef struct { 
    float x, y;
} Point;

typedef struct Shape Shape;

struct ShapeAPI {
    void (*drop)(Shape*);
    void (*draw)(const Shape*);
};

struct Shape {
    struct ShapeAPI* api;
    uint32_t color;
    uint16_t style;
    uint8_t thickness;
    uint8_t hardness;
};

void Shape_drop(Shape* shape)
{
    printf("base destructed\n");
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


Triangle* Triangle_new(Point a, Point b, Point c)
{
    return c_new(Triangle, {{.api=&Triangle_api}, .p={a, b, c}});
}

static void Triangle_draw(const Shape* shape)
{
    const Triangle* self = c_dyn_ptr(Triangle, shape);
    printf("Triangle : (%g,%g), (%g,%g), (%g,%g)\n",
           self->p[0].x, self->p[0].y,
           self->p[1].x, self->p[1].y,
           self->p[2].x, self->p[2].y);
}

static struct ShapeAPI Triangle_api = {
    .drop = Shape_drop,
    .draw = Triangle_draw,
};

// Polygon implementation
// ============================================================

#define i_type PointVec 
#define i_val Point
#include <stc/cstack.h>

typedef struct {
    Shape base;
    PointVec points;
} Polygon;

c_vtable(struct ShapeAPI, Polygon);


Polygon* Polygon_new(void)
{
    return c_new(Polygon, {{.api=&Polygon_api}, .points=PointVec_init()});
}

void Polygon_addPoint(Polygon* self, Point p)
{
    PointVec_push(&self->points, p);
}

static void Polygon_drop(Shape* shape)
{
    Polygon* self = c_dyn_ptr(Polygon, shape);
    printf("poly destructed\n");
    PointVec_drop(&self->points);
    Shape_drop(shape);
}

static void Polygon_draw(const Shape* shape)
{
    const Polygon* self = c_dyn_ptr(Polygon, shape);
    printf("Polygon  :");
    c_foreach (i, PointVec, self->points)
        printf(" (%g,%g)", i.ref->x, i.ref->y);
    puts("");
}

static struct ShapeAPI Polygon_api = {
    .drop = Polygon_drop,
    .draw = Polygon_draw,
};

// Test
// ============================================================

#define i_type Shapes
#define i_val Shape*
#define i_valdrop(x) Shape_delete(*x)
#include <stc/cstack.h>

void testShape(const Shape* shape)
{
    shape->api->draw(shape);
}


int main(void)
{
    c_auto (Shapes, shapes)
    {
        Triangle* tri1 = Triangle_new((Point){5, 7}, (Point){12, 7}, (Point){12, 20});
        Polygon* pol1 = Polygon_new();
        Polygon* pol2 = Polygon_new();

        c_apply(p, Polygon_addPoint(pol1, *p), Point,
            {{50, 72}, {123, 73}, {127, 201}, {828, 333}});

        c_apply(p, Polygon_addPoint(pol2, *p), Point,
            {{5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56}});
        
        Shapes_push(&shapes, &tri1->base);
        Shapes_push(&shapes, &pol1->base);
        Shapes_push(&shapes, &pol2->base);

        c_foreach (i, Shapes, shapes)
            testShape(*i.ref);
    }
}
