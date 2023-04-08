#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// carc implements its own clone method using reference counting,
// so 'i_valclone' is not required to be defined (ignored).

#define i_type Arc          // set type name to be defined (instead of 'carc_int')
#define i_val int
#define i_valdrop int_drop  // optional, just to display the elements destroyed
#define i_no_clone          // required because of valdrop
#include <stc/carc.h>       // Arc

#define i_keyboxed Arc    // note: use i_keyboxed instead of i_key for carc/cbox elements
#include <stc/csset.h>      // csset_Arc (like: std::set<std::shared_ptr<int>>)

#define i_valboxed Arc    // note: as above.
#include <stc/cvec.h>       // cvec_Arc (like: std::vector<std::shared_ptr<int>>)

int main()
{
    const int years[] = {2021, 2012, 2022, 2015};

    cvec_Arc vec = {0};
    c_forrange (i, c_arraylen(years))
        cvec_Arc_push(&vec, Arc_from(years[i]));

    printf("vec:");
    c_foreach (i, cvec_Arc, vec)
        printf(" %d", *i.ref->get);
    puts("");

    // add odd numbers from vec to set
    csset_Arc set = {0};
    c_foreach (i, cvec_Arc, vec)
        if (*i.ref->get & 1)
            csset_Arc_insert(&set, Arc_clone(*i.ref)); // copy shared pointer => increments counter.

    // erase the two last elements in vec
    cvec_Arc_pop_back(&vec);
    cvec_Arc_pop_back(&vec);

    printf("vec:");
    c_foreach (i, cvec_Arc, vec) printf(" %d", *i.ref->get);

    printf("\nset:");
    c_foreach (i, csset_Arc, set) printf(" %d", *i.ref->get);

    Arc p = Arc_clone(vec.data[0]);
    printf("\n%d is now owned by %ld objects\n", *p.get, *p.use_count);

    Arc_drop(&p);
    cvec_Arc_drop(&vec);
    csset_Arc_drop(&set);
}
