#include <stdio.h>

void show_drop(int* x) { printf("drop: %d\n", *x); }

#define i_TYPE Arc,int
#define i_keydrop show_drop
#define i_use_cmp           // enable sort/search for int type
#include "stc/arc.h"        // Shared pointer to int

#define i_type Vec
#define i_key_arc Arc
#define i_use_cmp
#include "stc/vec.h"        // Vec: vec<Arc>


int main(void)
{
    Vec vec = c_init(Vec, {2012, 1990, 2012, 2019, 2015});

    // clone the second 2012 and push it back.
    // note: cloning make sure that vec.data[2] has ref count 2.
    Vec_push(&vec, Arc_clone(vec.data[2]));  // => share vec.data[2]
    Vec_emplace(&vec, *vec.data[2].get);     // => deep-copy vec.data[2]

    printf("vec before erase :");
    c_foreach (i, Vec, vec)
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
    c_foreach (i, Vec, vec)
        printf(" %d", *i.ref->get);

    Vec_sort(&vec);
    printf("\nvec after sort   :");
    c_foreach (i, Vec, vec)
        printf(" %d", *i.ref->get);

    puts("\nDone");
    Vec_drop(&vec);
}
