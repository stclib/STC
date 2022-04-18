// https://doc.rust-lang.org/rust-by-example/std/box.html

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stc/ccommon.h>

struct {
    double x;
    double y;
} typedef Point;

// A Rectangle can be specified by where its top left and bottom right 
// corners are in space
struct {
    Point top_left;
    Point bottom_right;
} typedef Rectangle;

#define i_val Point
#define i_opt c_no_cmp
#include <stc/cbox.h> // cbox_Point

#define i_val Rectangle
#define i_opt c_no_cmp
#include <stc/cbox.h> // cbox_Rectangle

// Box in box:
#define i_val_arcbox cbox_Point  // NB: use i_val_arcbox when value is a cbox or carc!
                                 // it will auto-set i_valdrop, i_valfrom, i_cmp for you.
#define i_opt c_no_cmp
#define i_tag BoxPoint
#include <stc/cbox.h> // cbox_BoxPoint

Point origin(void) {
    return (Point){ .x=0.0, .y=0.0 };
}

cbox_Point boxed_origin(void) {
    // Allocate this point on the heap, and return a pointer to it
    return cbox_Point_from((Point){ .x=0.0, .y=0.0 });
}


int main(void) {
    // (all the type annotations are superfluous)
    // Stack allocated variables
    Point point = origin();
    Rectangle rectangle = (Rectangle){
        .top_left = origin(),
        .bottom_right = (Point){ .x=3.0, .y=-4.0 }
    };

    // Heap allocated rectangle
    c_auto (cbox_Rectangle, boxed_rectangle)
    c_auto (cbox_Point, boxed_point)
    c_auto (cbox_BoxPoint, box_in_a_box)
    {
        boxed_rectangle = cbox_Rectangle_from((Rectangle){
            .top_left = origin(),
            .bottom_right = (Point){ .x=3.0, .y=-4.0 }
        });

        // The output of functions can be boxed
        boxed_point = cbox_Point_from(origin());

        // Double indirection
        box_in_a_box = cbox_BoxPoint_from(boxed_origin());

        printf("Point occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(point));
        printf("Rectangle occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(rectangle));

        // box size == pointer size
        printf("Boxed point occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(boxed_point));
        printf("Boxed rectangle occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(boxed_rectangle));
        printf("Boxed box occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(box_in_a_box));

        // Copy the data contained in `boxed_point` into `unboxed_point`
        Point unboxed_point = *boxed_point.get;
        printf("Unboxed point occupies %" PRIuMAX " bytes on the stack\n",
                sizeof(unboxed_point));
    }
}
