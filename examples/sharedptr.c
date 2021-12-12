#include <stdio.h>
#include <string.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

// csptr implements its own clone method using ref counting, so 'i_valfrom'
// should not be defined (ignored).
#define i_val int
#define i_del int_del       // optional func, just to display elements destroyed
#include <stc/csptr.h>      // csptr_int

#define i_key_ref csptr_int // note: use i_key_ref instead of i_key
#define i_tag int           // tag otherwise defaults to 'ref'
#include <stc/csset.h>      // csset_int (like: std::set<std::shared_ptr<int>>)

#define i_val_ref csptr_int // Note: use i_val_ref instead of i_val
#define i_tag int           // tag otherwise defaults to 'ref'
#include <stc/cvec.h>       // cvec_int (like: std::vector<std::shared_ptr<int>>)

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call del at scope exit
    c_auto (csset_int, set)  // declare and init set, call del at scope exit
    {
        const int years[] = {2021, 2012, 2022, 2015};
        c_forrange (i, c_arraylen(years))
            cvec_int_push_back(&vec, csptr_int_new(years[i]));

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

        puts("\nDone");
    }
}
