#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// arc implements its own clone method using reference counting,
// so 'i_keyclone' is not required to be defined (ignored).

#define i_TYPE Arc,int      // set type name to be defined (instead of 'arc_int')
#define i_keydrop int_drop  // optional, just to display the elements destroyed
#define i_use_cmp           // enable default comparison (on int)
#include "stc/arc.h"        // Arc

#define i_type Arcset
#define i_key_arc Arc       // note: use i_key_arc instead of i_key for arc/box elements
#include "stc/sset.h"       // Arcset (like: std::set<std::shared_ptr<int>>)

#define i_type Arcvec
#define i_key_arc Arc       // note: as above.
#define i_use_cmp
#include "stc/vec.h"        // Arcvec (like: std::vector<std::shared_ptr<int>>)

int main(void)
{
    const int years[] = {2021, 2012, 2022, 2015};

    Arcvec vec = {0};
    c_forrange (i, c_arraylen(years)) {
        Arcvec_emplace(&vec, years[i]);
        // Arcvec_push(&vec, Arc_from(years[i])); // alt.
    }

    Arcvec_sort(&vec);

    printf("vec:");
    c_foreach (i, Arcvec, vec)
        printf(" %d", *i.ref->get);
    puts("");

    // add odd numbers from vec to set
    Arcset set = {0};
    c_foreach (i, Arcvec, vec)
        if (*i.ref->get & 1)
            Arcset_insert(&set, Arc_clone(*i.ref)); // copy shared pointer => increments counter.

    // erase the two last elements in vec
    Arcvec_pop_back(&vec);
    Arcvec_pop_back(&vec);

    printf("vec:");
    c_foreach (i, Arcvec, vec) printf(" %d", *i.ref->get);

    printf("\nset:");
    c_foreach (i, Arcset, set) printf(" %d", *i.ref->get);

    Arc p = Arc_clone(vec.data[0]);
    printf("\n%d is now owned by %ld objects\n", *p.get, *p.use_count);

    Arc_drop(&p);
    Arcvec_drop(&vec);
    Arcset_drop(&set);
}
