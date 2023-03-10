// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stc/algo/coroutine.h>
#include <stdio.h>

typedef struct {
    int n;
    int a, b, c;
} Triple_value, Triple;

typedef struct {
    Triple_value* ref;
    int cco_state;
} Triple_iter;

bool Triple_next(Triple_iter* it) {
    Triple_value* t = it->ref;
    cco_begin(it);
        for (t->c = 1;; ++t->c) {
            for (t->a = 1; t->a < t->c; ++t->a) {
                for (t->b = t->a; t->b < t->c; ++t->b) {
                    if (t->a*t->a + t->b*t->b == t->c*t->c) {
                        if (t->n-- == 0) cco_return;
                        cco_yield(true);
                    }
                }
            }
        }
        cco_final:
            it->ref = NULL;
    cco_end(false);
}

Triple_iter Triple_begin(Triple* t) {
    Triple_iter it = {t}; 
    if (t->n > 0) Triple_next(&it);
    else it.ref = NULL;
    return it;
}


int main()
{
    puts("Pythagorean triples with c < 100:");
    Triple t = {INT32_MAX};
    c_foreach (i, Triple, t)
    {
        if (i.ref->c < 100)
            printf("%u: (%d, %d, %d)\n", INT32_MAX - i.ref->n + 1, i.ref->a, i.ref->b, i.ref->c);
        else
            cco_stop(&i);
    }
}
