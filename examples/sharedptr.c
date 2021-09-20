#include <stdio.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

#define i_val int
#define i_valdel int_del  // optional func to show elements destroyed
#include <stc/csptr.h>    // define csptr_int shared pointers

#define i_key_csptr int   // refer to csptr_int definition above
#include <stc/csset.h>    // define a sorted set of csptr_int

#define i_val_csptr int
#include <stc/cvec.h>

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call del at scope exit
    c_auto (csset_int, set)  // declare and init set, call del at scope exit
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
                csset_int_emplace(&set, *i.ref); // copy shared pointer => increments counter.

        // erase the two last elements in vec
        cvec_int_pop_back(&vec);
        cvec_int_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_int, set) printf(" %d", *i.ref->get);

        c_autovar (csptr_int p = csptr_int_clone(vec.data[0]), csptr_int_del(&p)) {
            printf("\n%d is now owned by %zu objects\n", *p.get, *p.use_count);
        }

        puts("Done");
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
