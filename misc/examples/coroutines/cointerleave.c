// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include "stc/coroutine.h"
#define i_TYPE IVec,int
#include "stc/vec.h"

struct GenValue {
    IVec *v;
    IVec_iter it;
    int cco_state;
};

static long get_value(struct GenValue* g)
{
    cco_scope(g) {
        c_foreach_it(g->it, IVec, *g->v)
            cco_yield_v(*g->it.ref);
    }
    return 0;
}

struct Generator {
    struct GenValue x, y;
    int cco_state;
    long value;
};

int interleaved(struct Generator* g)
{
    cco_scope(g) {
        bool a, b;
        do {
            g->value = get_value(&g->x);
            if ((a = !cco_done(&g->x)))
                cco_yield;

            g->value = get_value(&g->y);
            if ((b = !cco_done(&g->y)))
                cco_yield;
        } while (a | b);
    }
    return 0;
}

void Use(void)
{
    IVec a = c_init(IVec, {2, 4, 6, 8, 10, 11});
    IVec b = c_init(IVec, {3, 5, 7, 9});

    struct Generator g = {{&a}, {&b}};

    cco_run_coroutine(interleaved(&g)) {
        printf("%ld ", g.value);
    }
    puts("");
    c_drop(IVec, &a, &b);
}

int main(void)
{
    Use();
}
