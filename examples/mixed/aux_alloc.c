#include <stdio.h>
#include <stc/algorithm.h>
#include "myalloc.h"

#define T List, double, (c_use_eq)
#define i_aux MyAlloc* // define an auxiliary field `aux` in the List
#define i_allocator my // explicitly define allocator prefix.
#include <stc/stack.h>

#define T ListDeque, List, (c_keyclass | c_use_eq)
// Shorthand for defining both i_aux AND i_allocator as above, in one line:
#define i_aux MyAlloc*, my
#include <stc/deque.h>

int main(void)
{
    MyAlloc myalloc = {.bytes=0};
    List* v;
    ListDeque store_a = {.aux=&myalloc};                // All containers in STC can be initialized with {0}.

    v = ListDeque_push(&store_a, (List){.aux=&myalloc}); // push() returns a pointer to the new element in vec.
    List_push(v, 100.0);
    List_push(v, 200.0);

    v = ListDeque_push(&store_a, (List){.aux=&myalloc});
    List_push(v, 300.0);
    List_push(v, 400.0);

    printf("alloc: %d\n", (int)myalloc.bytes);
    ListDeque store_b = {.aux=&myalloc};

    v = ListDeque_push(&store_b, (List){.aux=&myalloc});
    List_push(v, 10.0);
    List_push(v, 20.0);

    v = ListDeque_push(&store_b, (List){.aux=&myalloc});
    List_push(v, 30.0);
    List_push(v, 40.0);

    printf("store_a == store_b is %s.\n", ListDeque_eq(&store_a, &store_b) ? "true" : "false");
    printf("alloc: %d\n", (int)myalloc.bytes);

    ListDeque store_c = ListDeque_clone(store_a); // Make a deep-copy, using myalloc.

    for (c_each(i, ListDeque, store_c))           // Loop through the cloned deque
        for (c_each(j, List, *i.ref))
            printf(" %g", *j.ref);
    printf("\nalloc: %d\n", (int)myalloc.bytes);

    c_drop(ListDeque, &store_a, &store_b, &store_c);  // Free all 9 storages.
    printf("alloc: %d\n", (int)myalloc.bytes);
}
