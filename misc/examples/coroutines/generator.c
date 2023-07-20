// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include <stc/coroutine.h>

typedef struct {
    int size;
    int a, b, c;
} Triple, Triple_value;

typedef struct {
    Triple_value* ref;
    int count;
    int cco_state;
} Triple_iter;

int Triple_next(Triple_iter* it) {
    Triple_value* g = it->ref;
    cco_routine(it)
    {
        for (g->c = 5; g->size; ++g->c) {
            for (g->a = 1; g->a < g->c; ++g->a) {
                for (g->b = g->a; g->b < g->c; ++g->b) {
                    if (g->a*g->a + g->b*g->b == g->c*g->c) {
                        if (it->count++ == g->size)
                            cco_return;
                        cco_yield();
                    }
                }
            }
        }
        cco_cleanup:
        it->ref = NULL;
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
    puts("Pythagorean triples with c < 100:");
    Triple triple = {.size=30}; // max number of triples
    c_foreach (i, Triple, triple) {
        if (i.ref->c < 100)
            printf("%u: (%d, %d, %d)\n", i.count, i.ref->a, i.ref->b, i.ref->c);
        else
            cco_stop(&i);
    }
}
