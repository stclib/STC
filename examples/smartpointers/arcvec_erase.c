#include <stdio.h>

void show_drop(int* x) { printf("drop: %d\n", *x); }


// enable sort/search for non-atomic Arc int type
#define T Arc, int, (c_no_atomic | c_use_cmp)
#define i_keydrop show_drop
#include <stc/arc.h>        // Shared pointer to int

// arc is "pro", enable search/sort
#define T Vec, Arc, (c_keypro | c_use_cmp)
#include <stc/vec.h>        // Vec: vec<Arc>


int main(void)
{
    Vec vec = c_make(Vec, {2012, 1990, 2012, 2019, 2015});
    printf("elm size: %d\n", (int)sizeof(Arc));

    // clone the second 2012 and push it back.
    // note: cloning make sure that vec.data[2] has ref count 2.
    Vec_push(&vec, Arc_clone(vec.data[2]));  // => share vec.data[2]
    Vec_emplace(&vec, *vec.data[2].get);     // => deep-copy vec.data[2]

    printf("vec before erase :");
    for (c_each(i, Vec, vec))
        printf(" %d", *i.ref->get);

    printf("\nerase vec.data[2]; or first matching value depending on compare.\n");
    Vec_iter it;
    it = Vec_find(&vec, *vec.data[2].get);
    if (it.ref)
        Vec_erase_at(&vec, it);

    int year = 2015;
    it = Vec_find(&vec, year); // Ok as tmp only.
    if (it.ref)
        Vec_erase_at(&vec, it);

    printf("vec after erase  :");
    for (c_each(i, Vec, vec))
        printf(" %d", *i.ref->get);

    Vec_sort(&vec);
    printf("\nvec after sort   :");
    for (c_each(i, Vec, vec))
        printf(" %d", *i.ref->get);

    puts("\nDone");
    Vec_drop(&vec);
}
