// Demo of typesafe polymorphism in C99, using STC.

#include <stdlib.h>
#include <stdio.h>
#include <stc/ccommon.h>

#define c_self(s, T) \
    T* self = (T *)(s); \
    assert(&T##_api == (s)->api && \
           &T##_api == self->base.api)

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
    printf("base destructed\n");
}

void Shape_delete(Shape* shape)
{
    if (shape) {
        shape->api->drop(shape);
        c_free(shape);
    }
    printf("shape deleted\n");
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
    printf("Triangle : (%g,%g), (%g,%g), (%g,%g)\n",
           self->p[0].x, self->p[0].y,
           self->p[1].x, self->p[1].y,
           self->p[2].x, self->p[2].y);
}

static struct ShapeAPI Triangle_api = {
    .drop = Shape_drop,
    .draw = Triangle_draw,
};

Triangle* Triangle_new(Point a, Point b, Point c)
{
    return c_new(Triangle, {{.api=&Triangle_api}, .p={a, b, c}});
}

// Polygon implementation
// ============================================================

#define i_type PntVec 
#define i_val Point
#include <stc/cstack.h>

typedef struct {
    Shape base;
    PntVec points;
} Polygon;

c_vtable(struct ShapeAPI, Polygon);


static void Polygon_drop(Shape* shape)
{
    c_self(shape, Polygon);
    printf("poly destructed\n");
    PntVec_drop(&self->points);
    Shape_drop(shape);
}

static void Polygon_draw(const Shape* shape)
{
    const c_self(shape, Polygon);
    printf("Polygon  :");
    c_foreach (i, PntVec, self->points)
        printf(" (%g,%g)", i.ref->x, i.ref->y);
    puts("");
}

static struct ShapeAPI Polygon_api = {
    .drop = Polygon_drop,
    .draw = Polygon_draw,
};

Polygon* Polygon_new(void)
{
    return c_new(Polygon, {{.api=&Polygon_api}, .points=PntVec_init()});
}

void Polygon_addPoint(Polygon* self, Point p)
{
    PntVec_push(&self->points, p);
}


// Test
// ============================================================

#define i_type Shapes
#define i_val Shape*
#define i_opt c_no_clone
#define i_valdrop(x) Shape_delete(*x)
#include <stc/cstack.h>

void testShape(Shape* shape)
{
    shape->api->draw(shape);
}


int main(void)
{
    c_auto (Shapes, shapes)
    {
        Triangle* t1 = Triangle_new((Point){5, 7}, (Point){12, 7}, (Point){12, 20});

        Polygon* p1 = Polygon_new();
        c_apply(pnt, Polygon_addPoint(p1, pnt), Point, {
            {50, 72}, {123, 73}, {127, 201}, {828, 333},
        });

        Polygon* p2 = Polygon_new();
        c_apply(pnt, Polygon_addPoint(p2, pnt), Point, {
            {5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56},
        });
        
        Shapes_push(&shapes, &t1->base);
        Shapes_push(&shapes, &p1->base);
        Shapes_push(&shapes, &p2->base);

        c_foreach (i, Shapes, shapes)
            testShape(*i.ref);
    }
}
