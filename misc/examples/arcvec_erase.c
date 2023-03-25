#include <stdio.h>

void show_drop(int* x) { printf("drop: %d\n", *x); }

#define i_type Arc
#define i_val int
#define i_valdrop show_drop
#define i_no_clone          // required because of valdrop
#include <stc/carc.h>       // Shared pointer to int

#define i_type Vec
#define i_valboxed Arc
#include <stc/cvec.h>       // Vec: cvec<Arc>


int main()
{
    Vec vec = c_make(Vec, {2012, 1990, 2012, 2019, 2015});
    
    // clone the second 2012 and push it back.
    // note: cloning make sure that vec.data[2] has ref count 2.
    Vec_push(&vec, Arc_clone(vec.data[2]));
    
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
