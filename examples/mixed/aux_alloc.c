#include <stdio.h>
#include <stc/algorithm.h>

#define T Vec, double, (c_use_eq)
#include "myalloc.h"
#include <stc/deque.h>

#define T Vec2D, Vec, (c_keyclass | c_use_eq)
#include "myalloc.h"
#include <stc/deque.h>

int main(void)
{
    struct MyAlloc myalloc = {0};
    Vec* v;
    Vec2D vec_a = {.aux={&myalloc}};                // All containers in STC can be initialized with {0}.
    v = Vec2D_push(&vec_a, (Vec){.aux={&myalloc}}); // push() returns a pointer to the new element in vec.
    Vec_push(v, 10.0);
    Vec_push(v, 20.0);

    v = Vec2D_push(&vec_a, (Vec){.aux={&myalloc}});
    Vec_push(v, 30.0);
    Vec_push(v, 40.0);


   Vec2D vec_b = {.aux={&myalloc}};                  // All containers in STC can be initialized with {0}.
    v = Vec2D_push(&vec_b, (Vec){.aux={&myalloc}}); // push() returns a pointer to the new element in vec.
    Vec_push(v, 10.0);
    Vec_push(v, 20.0);

    v = Vec2D_push(&vec_b, (Vec){.aux={&myalloc}});
    Vec_push(v, 30.0);
    Vec_push(v, 40.0);

    printf("vec_a == vec_b is %s.\n", Vec2D_eq(&vec_a, &vec_b) ? "true" : "false");

    Vec2D clone = Vec2D_clone(vec_a);   // Make a deep-copy of vec

    for (c_each(i, Vec2D, clone))         // Loop through the cloned vector
        for (c_each(j, Vec, *i.ref))
            printf(" %g", *j.ref);
    puts("");

    printf("alloc: %d %d\n", (int)vec_a.aux.alloc->count, (int)vec_b.aux.alloc->count);

    c_drop(Vec2D, &vec_a, &vec_b, &clone);  // Free all 9 vectors.

    printf("alloc: %d %d\n", (int)vec_a.aux.alloc->count, (int)vec_b.aux.alloc->count);
}
