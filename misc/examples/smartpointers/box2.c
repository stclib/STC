// example: https://doc.rust-lang.org/rust-by-example/std/box.html
#include <stdio.h>

typedef struct {
    double x;
    double y;
} Point;

// A Rectangle can be specified by where its top left and bottom right
// corners are in space
typedef struct {
    Point top_left;
    Point bottom_right;
} Rectangle;

#define i_TYPE BoxPoint,Point
#include "stc/box.h"

#define i_TYPE BoxRect,Rectangle
#include "stc/box.h"

// Box in box:
#define i_type BoxBoxPoint
#define i_key_arcbox BoxPoint // NB: use i_key_arcbox when value is a box or arc!
#include "stc/box.h" // BoxBoxPoint

Point origin(void) {
    return c_LITERAL(Point){ .x=1.0, .y=2.0 };
}

BoxPoint boxed_origin(void) {
    // Allocate this point on the heap, and return a pointer to it
    return BoxPoint_from(c_LITERAL(Point){ .x=1.0, .y=2.0 });
}


int main(void) {
    // Stack allocated variables
    Point point = origin();
    Rectangle rectangle = {
        .top_left = origin(),
        .bottom_right = { .x=3.0, .y=-4.0 }
    };

    // Heap allocated rectangle
    BoxRect boxed_rectangle = BoxRect_from(c_LITERAL(Rectangle){
        .top_left = origin(),
        .bottom_right = { .x=3.0, .y=-4.0 }
    });
    // The output of functions can be boxed
    BoxPoint boxed_point = BoxPoint_from(origin());

    // Create BoxBoxPoint from either a Point or a BoxPoint:
    BoxBoxPoint box_in_a_box = BoxBoxPoint_from(origin());

    c_defer(
        BoxBoxPoint_drop(&box_in_a_box),
        BoxPoint_drop(&boxed_point),
        BoxRect_drop(&boxed_rectangle)
    ){
        printf("box_in_a_box: x = %g\n", BoxBoxPoint_toraw(&box_in_a_box).x);

        printf("Point occupies %d bytes on the stack\n",
                (int)sizeof(point));
        printf("Rectangle occupies %d bytes on the stack\n",
                (int)sizeof(rectangle));

        // box size == pointer size
        printf("Boxed point occupies %d bytes on the stack\n",
                (int)sizeof(boxed_point));
        printf("Boxed rectangle occupies %d bytes on the stack\n",
                (int)sizeof(boxed_rectangle));
        printf("Boxed box occupies %d bytes on the stack\n",
                (int)sizeof(box_in_a_box));

        // Copy the data contained in `boxed_point` into `unboxed_point`
        Point unboxed_point = *boxed_point.get;
        printf("Unboxed point occupies %d bytes on the stack\n",
                (int)sizeof(unboxed_point));
    }
}
