#include <stdio.h>

void show_del(int* x) { printf("del: %d\n", *x); }

#define i_val int
#define i_del show_del        // this "destroy" func shows which elements are destroyed
// csptr/cbox will use pointer address comparison of i_val if no i_cmp func is specified,
// or 'i_opt c_no_compare' is defined, otherwise i_cmp is used to compare object values.
// See the different results by commenting out the next line.
#define i_cmp(x, y) (*(x) - *(y))
#include <stc/csptr.h>        // csptr_int: shared pointer to int

#define i_val_ref csptr_int
#define i_tag intp
#include <stc/cvec.h>         // cvec_intp: cvec<csptr_int>


int main()
{
    c_auto (cvec_intp, vec)
    {
        const int v[] = {2012, 1990, 2012, 2019, 2015};
        c_forrange (i, c_arraylen(v))
            cvec_intp_push_back(&vec, csptr_int_new(v[i]));
        
        // clone the second 2012 and push it back.
        // note: cloning make sure that vec.data[2] has ref count 2.
        cvec_intp_emplace_back(&vec, vec.data[2]);
        
        printf("vec before erase :");
        c_foreach (i, cvec_intp, vec)
            printf(" %d", *i.ref->get);
        puts("");

        // erase vec.data[2]; or first matching value depending on compare.
        cvec_intp_iter it;
        it = cvec_intp_find(&vec, vec.data[2]);
        if (it.ref != cvec_intp_end(&vec).ref)
            cvec_intp_erase_at(&vec, it);

        int year = 2015;
        it = cvec_intp_find(&vec, (csptr_int){&year}); // Ok as tmp only.
        if (it.ref != cvec_intp_end(&vec).ref)
            cvec_intp_erase_at(&vec, it);

        printf("vec after erase  :");
        c_foreach (i, cvec_intp, vec)
            printf(" %d", *i.ref->get);
        
        puts("\nDone");
    }
}
