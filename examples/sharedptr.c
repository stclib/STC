#include <stdio.h>

void int_del(int* x) { printf("del: %d\n", *x); }

#define i_val int
#define i_valdel int_del      // optional func to show elements destroyed
#include <stc/csptr.h>        // csptr_int: shared pointer to int

#define i_key_csptr csptr_int
#define i_tag intp
#include <stc/csset.h>        // csset_intp: csset<csptr_int>

#define i_val_csptr csptr_int
#define i_tag intp
#include <stc/cvec.h>         // cvec_intp: cvec<csptr_int>

int main()
{
    c_auto (cvec_intp, vec)
    c_auto (csset_intp, set)   // declare and init set, call del at scope exit
    {
        c_apply(cvec_intp, push_back, &vec, {
            csptr_int_make(2021),
            csptr_int_make(2012),
            csptr_int_make(2022),
            csptr_int_make(2015),
        });
        printf("vec:");
        c_foreach (i, cvec_intp, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_intp, vec)
            if (*i.ref->get & 1)
                csset_intp_emplace(&set, *i.ref); // copy shared pointer => increments counter.

        // erase the two last elements in vec
        cvec_intp_pop_back(&vec);
        cvec_intp_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_intp, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_intp, set) printf(" %d", *i.ref->get);

        c_autovar (csptr_int p = csptr_int_clone(vec.data[0]), csptr_int_del(&p)) {
            printf("\n%d is now owned by %u objects\n", *p.get, *p.use_count);
        }

        puts("Done");
    }
}
