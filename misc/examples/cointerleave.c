// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stc/calgo.h>
#include <stdio.h>
#define i_type IVec
#define i_val int
#include <stc/cvec.h>

struct GenValue {
    IVec *v;
    IVec_iter it;
    int cco_state;
};

static int next_value(struct GenValue* g)
{
    cco_begin(g);
        for (g->it = IVec_begin(g->v); g->it.ref; IVec_next(&g->it))
            cco_yield(*g->it.ref);
        cco_final:
    cco_end(0);
}

struct Generator {
    struct GenValue x, y;
    int cco_state;
    int value;
};

bool interleaved(struct Generator* g) 
{
    cco_begin(g);
        while (cco_alive(&g->x) || cco_alive(&g->y))
        {
            g->value = next_value(&g->x);
            if (cco_alive(&g->x)) cco_yield(false);

            g->value = next_value(&g->y);
            if (cco_alive(&g->y)) cco_yield(false);
        } 
        cco_final:
    cco_end(true);
}

void Use(void)
{
    IVec a = c_make(IVec, {2, 4, 6, 8, 10, 11});
    IVec b = c_make(IVec, {3, 5, 7, 9});

    struct Generator g = {{&a}, {&b}};

    while (!interleaved(&g))
        printf("%d\n", g.value);

    c_drop(IVec, &a, &b);
}

int main()
{
    Use();
}
