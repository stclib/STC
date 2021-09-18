#include <stdio.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

#define i_val int
#define i_valdel int_del
#include <stc/csptr.h>

#define i_key_csptr int
#include <stc/csset.h>

#define i_val_csptr int
#include <stc/cvec.h>

int main()
{
    c_forauto (cvec_int, vec) // raii
    c_forauto (csset_int, set) // raii
    {
        cvec_int_push_back(&vec, csptr_int_make(2021));
        cvec_int_push_back(&vec, csptr_int_make(2012));
        cvec_int_push_back(&vec, csptr_int_make(2022));
        cvec_int_push_back(&vec, csptr_int_make(2015));

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_int, vec)
            if (*i.ref->get & 1)
                csset_int_emplace(&set, *i.ref); // copy shared pointer => increments ref.

        // erase the two last elements in vec
        cvec_int_pop_back(&vec);
        cvec_int_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_int, set) printf(" %d", *i.ref->get);

        puts("\nDone");
    }
}

/* output:
vec: 2021 2012 2022 2015
del: 2022
vec: 2021 2012
set: 2015 2021
Done
del: 2015
del: 2021
del: 2012
*/
