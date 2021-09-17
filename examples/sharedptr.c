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
    c_forauto (cvec_int, vec)
    c_forauto (csset_int, set)
    {
        csset_int_insert(&set, csptr_int_make(2021));
        csset_int_insert(&set, csptr_int_make(2012));
        csset_int_insert(&set, csptr_int_make(2022));
        csset_int_insert(&set, csptr_int_make(2015));

        // add odd numbers from set to vec:
        c_foreach (i, csset_int, set) {
            if (*i.ref->get & 1)
                cvec_int_push_back(&vec, csptr_int_clone(*i.ref));
        }

        printf("print set:");
        c_foreach (i, csset_int, set) {
            printf(" %d", *i.ref->get);
        }

        printf("\nprint vec:");
        c_foreach (i, cvec_int, vec) {
            printf(" %d", *i.ref->get);
        }
        puts("");
    }
}
