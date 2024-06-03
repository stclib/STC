// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include "stc/coroutine.h"
#include "stc/algorithm.h"

typedef struct {
    int max_triples;
    int a, b, c;
} Triple;

// Create an iterable generator over Triple with count items.
// Requires coroutine Triple_next() and function Triple_begin() to be defined.
cco_iter_struct(Triple,
    int count;
);

int Triple_next(Triple_iter* it) {
    Triple* g = it->ref; // note: before cco_scope
    cco_scope(it)
    {
        for (g->c = 5;; ++g->c) {
            for (g->a = 1; g->a < g->c; ++g->a) {
                for (g->b = g->a; g->b < g->c; ++g->b) {
                    if (g->a*g->a + g->b*g->b == g->c*g->c) {
                        if (it->count++ == g->max_triples)
                            cco_return;
                        cco_yield;
                    }
                }
            }
        }
        cco_final:      // cleanup
        it->ref = NULL; // stop iteration
        printf("Done\n");
    }
    return 0;
}

Triple_iter Triple_begin(Triple* g) {
    Triple_iter it = {.ref=g};
    Triple_next(&it);
    return it;
}

int main(void)
{
    Triple triple = {.max_triples=INT32_MAX};

    puts("Pythagorean triples.\nGet all triples with c < 40, using c_foreach:");
    c_foreach (i, Triple, triple) {
        if (i.ref->c < 40)
            printf("%u: (%d, %d, %d)\n", i.count, i.ref->a, i.ref->b, i.ref->c);
        else
            cco_stop(&i);
    }

    puts("\nGet the 10 first triples with c < 40, using c_filter:");
    c_filter(Triple, triple
         , (value->c < 40)
        && (cco_flt_take(10), // NB! use cco_flt_take(n) instead of c_flt_take(n)
                              // to ensure coroutine/iter cleanup if needed
            printf("%d: (%d, %d, %d)\n", c_flt_getcount(), value->a, value->b, value->c))
    );
}
