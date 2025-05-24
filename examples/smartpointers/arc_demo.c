#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// arc implements its own clone method using reference counting,
// so 'i_keyclone' is not required to be defined (ignored).

// Define the Arc, no need for atomic in single thread, enable default int comparisons.
#define T Arc, int, (c_no_atomic| c_use_cmp)
#define i_keydrop int_drop  // optional, just to display the elements destroyed
#include <stc/arc.h>        // Arc

#define T Arcset, Arc, (c_keypro) // arc's are "pro" types
#include <stc/sortedset.h>  // Arcset (like: std::set<std::shared_ptr<int>>)

#define T Arcvec, Arc, (c_keypro| c_use_cmp)
#include <stc/vec.h>        // Arcvec (like: std::vector<std::shared_ptr<int>>)

int main(void)
{
    const int years[] = {2021, 2012, 2022, 2015};

    Arcvec vec = {0};
    for (c_range(i, c_countof(years))) {
        Arcvec_emplace(&vec, years[i]);
        // Arcvec_push(&vec, Arc_from(years[i])); // alt.
    }

    Arcvec_sort(&vec);

    printf("vec:");
    for (c_each(i, Arcvec, vec))
        printf(" %d", *i.ref->get);
    puts("");

    // add odd numbers from vec to set
    Arcset set = {0};
    for (c_each(i, Arcvec, vec))
        if (*i.ref->get & 1)
            Arcset_insert(&set, Arc_clone(*i.ref)); // copy shared pointer => increments counter.

    // erase the two last elements in vec
    Arcvec_pop_back(&vec);
    Arcvec_pop_back(&vec);

    printf("vec:");
    for (c_each(i, Arcvec, vec)) printf(" %d", *i.ref->get);

    printf("\nset:");
    for (c_each(i, Arcset, set)) printf(" %d", *i.ref->get);

    Arc p = Arc_clone(vec.data[0]);
    printf("\n%d is now owned by %ld objects\n", *p.get, Arc_use_count(p));

    Arc_drop(&p);
    Arcvec_drop(&vec);
    Arcset_drop(&set);
}
