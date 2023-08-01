// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>
#define i_type IVec
#define i_key int
#include <stc/cvec.h>

struct GenValue {
    IVec *v;
    IVec_iter it;
    int cco_state;
};

static int get_value(struct GenValue* g)
{
    cco_routine(g) {
        for (g->it = IVec_begin(g->v); g->it.ref; IVec_next(&g->it))
            cco_yield_v(*g->it.ref);
    }
    return -1;
}

struct Generator {
    struct GenValue x, y;
    int cco_state;
    int value;
};

cco_result interleaved(struct Generator* g) 
{
    cco_routine(g) {
        while (!(cco_done(&g->x) & cco_done(&g->y))) {
            g->value = get_value(&g->x);
            if (!cco_done(&g->x))
                cco_yield();

            g->value = get_value(&g->y);
            if (!cco_done(&g->y))
                cco_yield();
        }
    }
    return CCO_DONE;
}

void Use(void)
{
    IVec a = c_init(IVec, {2, 4, 6, 8, 10, 11});
    IVec b = c_init(IVec, {3, 5, 7, 9});

    struct Generator g = {{&a}, {&b}};

    cco_blocking_call(interleaved(&g)) {
        printf("%d ", g.value);
    }
    puts("");
    c_drop(IVec, &a, &b);
}

int main(void)
{
    Use();
}
