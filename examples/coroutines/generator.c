// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include <stc/coroutine.h>
#include <stc/algorithm.h>

// Create an iterable generator Triple with max_triples items.
// Requires coroutine Triple_next() and function Triple_begin() to be defined.

typedef struct {
    int a, b, c;
} Triple, Triple_value;

typedef struct {
    Triple* ref;    // required by iterator
    cco_base base;  // required by coroutine
} Triple_iter;

int Triple_next(Triple_iter* it) {
    Triple* g = it->ref; // get generator before cco_async starts!
    cco_async (it)
    {
        for (g->c = 5;; ++g->c) {
            for (g->a = 1; g->a < g->c; ++g->a) {
                for (g->b = g->a; g->b < g->c; ++g->b) {
                    if (g->a*g->a + g->b*g->b == g->c*g->c) {
                        cco_yield;
                    }
                }
            }
        }

        cco_finalize:
        it->ref = NULL; // stop iteration
        printf("Coroutine done\n");
    }
    return 0;
}

Triple_iter Triple_begin(Triple* g) {
    Triple_iter it = {.ref = g};
    Triple_next(&it); // advance to first
    return it;
}

int main(void)
{
    Triple triple = {0};

    puts("Pythagorean triples.\nGet all triples with c < 40, using for c_each:");
    int n=0;
    for (c_each(i, Triple, triple)) {
        Triple* t = i.ref;
        if (t->c < 40)
            printf("%u: (%d, %d, %d)\n", n++, t->a, t->b, t->c);
        else
            cco_stop(&i);
    }

    puts("\nGet the 10 first triples with c < 40, using c_filter:");
    c_filter(Triple, triple, true
        && (value->c < 40)
        && cco_flt_take(10) // NB! use cco_flt_take(n) instead of c_flt_take(n)
                            // to ensure coroutine cleanup.
                            // Also applies to cco_flt_takewhile(pred)
        && printf("%d: (%d, %d, %d)\n", c_flt_getcount(), value->a, value->b, value->c)
    );
}
