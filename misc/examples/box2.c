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

#define i_val Point
#define i_no_cmp
#include <stc/cbox.h> // cbox_Point

#define i_val Rectangle
#define i_no_cmp
#include <stc/cbox.h> // cbox_Rectangle

// Box in box:
#define i_valboxed cbox_Point // NB: use i_valboxed when value is a cbox or carc!
#define i_type BoxBoxPoint
#define i_no_cmp
#include <stc/cbox.h> // BoxBoxPoint

Point origin(void) {
    return (Point){ .x=1.0, .y=2.0 };
}

cbox_Point boxed_origin(void) {
    // Allocate this point on the heap, and return a pointer to it
    return cbox_Point_make((Point){ .x=1.0, .y=2.0 });
}


int main(void) {
    // Stack allocated variables
    Point point = origin();
    Rectangle rectangle = {
        .top_left = origin(),
        .bottom_right = { .x=3.0, .y=-4.0 }
    };

    // Heap allocated rectangle
    cbox_Rectangle boxed_rectangle = cbox_Rectangle_make((Rectangle){
        .top_left = origin(),
        .bottom_right = { .x=3.0, .y=-4.0 }
    });
    // The output of functions can be boxed
    cbox_Point boxed_point = cbox_Point_make(origin());

    // Can use from(raw) and toraw instead:
    BoxBoxPoint box_in_a_box = BoxBoxPoint_from(origin());

    c_defer(
        BoxBoxPoint_drop(&box_in_a_box),
        cbox_Point_drop(&boxed_point),
        cbox_Rectangle_drop(&boxed_rectangle)
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
