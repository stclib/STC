#include <stdio.h>

void show_drop(int* x) { printf("drop: %d\n", *x); }

#define i_type Arc
#define i_val int
#define i_drop show_drop      // this "destroy" func shows which elements are destroyed
// csptr/cbox will use pointer address comparison of i_val if no i_cmp func is specified,
// or 'i_opt c_no_cmp' is defined, otherwise i_cmp is used to compare object values.
// See the different results by commenting out the next line.
//#define i_cmp(x, y) (*(x) - *(y))
#include <stc/csptr.h>        // Arc: shared pointer to int

#define i_type Vec
#define i_val_bind Arc
#include <stc/cvec.h>        // Vec: cvec<Arc>


int main()
{
    c_auto (Vec, vec)
    {
        const int v[] = {2012, 1990, 2012, 2019, 2015};
        c_forrange (i, c_arraylen(v))
            Vec_push_back(&vec, Arc_new(v[i]));
        
        // clone the second 2012 and push it back.
        // note: cloning make sure that vec.data[2] has ref count 2.
        Vec_emplace_back(&vec, vec.data[2]);
        
        printf("vec before erase :");
        c_foreach (i, Vec, vec)
            printf(" %d", *i.ref->get);
        puts("");

        // erase vec.data[2]; or first matching value depending on compare.
        Vec_iter it;
        it = Vec_find(&vec, vec.data[2]);
        if (it.ref != Vec_end(&vec).ref)
            Vec_erase_at(&vec, it);

        int year = 2015;
        it = Vec_find(&vec, (Arc){&year}); // Ok as tmp only.
        if (it.ref != Vec_end(&vec).ref)
            Vec_erase_at(&vec, it);

        printf("vec after erase  :");
        c_foreach (i, Vec, vec)
            printf(" %d", *i.ref->get);
        
        puts("\nDone");
    }
}
