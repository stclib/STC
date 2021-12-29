#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// carc implements its own clone method using reference counting,
// so 'i_valfrom' need not be defined (will be ignored).

#define i_type iref         // set type name to be defined (instead of 'carc_int')
#define i_val int
#define i_drop int_drop     // optional, just to display the elements destroyed
#include <stc/carc.h>       // iref

#define i_key_sptr iref     // note: use i_key_bind instead of i_key for carc/cbox elements
#include <stc/csset.h>      // csset_iref (like: std::set<std::shared_ptr<int>>)

#define i_val_sptr iref     // note: as above.
#include <stc/cvec.h>       // cvec_iref (like: std::vector<std::shared_ptr<int>>)

int main()
{
    c_auto (cvec_iref, vec)   // declare and init vec, call cvec_iref_drop() at scope exit
    c_auto (csset_iref, set)  // declare and init set, call csset_iref_drop() at scope exit
    {
        const int years[] = {2021, 2012, 2022, 2015};
        c_forrange (i, c_arraylen(years))
            cvec_iref_push_back(&vec, iref_from(years[i]));

        printf("vec:");
        c_foreach (i, cvec_iref, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_iref, vec)
            if (*i.ref->get & 1)
                csset_iref_insert(&set, iref_clone(*i.ref)); // copy shared pointer => increments counter.

        // erase the two last elements in vec
        cvec_iref_pop_back(&vec);
        cvec_iref_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_iref, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_iref, set) printf(" %d", *i.ref->get);

        c_autovar (iref p = iref_clone(vec.data[0]), iref_drop(&p)) {
            printf("\n%d is now owned by %ld objects\n", *p.get, *p.use_count);
        }

        puts("\nDone");
    }
}
