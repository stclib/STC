// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stc/algo/coroutine.h>
#include <stdio.h>

typedef struct {
    int size;
    int a, b, c;
} Triple, Triple_value;

typedef struct {
    Triple_value* ref;
    int count;
    int cco_state;
} Triple_iter;

void Triple_next(Triple_iter* it) {
    Triple_value* t = it->ref;
    cco_begin(it);
        for (t->c = 5; t->size; ++t->c) {
            for (t->a = 1; t->a < t->c; ++t->a) {
                for (t->b = t->a; t->b < t->c; ++t->b) {
                    if (t->a*t->a + t->b*t->b == t->c*t->c) {
                        if (it->count++ == t->size)
                            cco_return;
                        cco_yield();
                    }
                }
            }
        }
    cco_final:
        it->ref = NULL;
    cco_end();
}

Triple_iter Triple_begin(Triple* t) {
    Triple_iter it = {.ref=t}; 
    Triple_next(&it);
    return it;
}


int main()
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
