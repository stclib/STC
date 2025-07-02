// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>
#define T IVec, int
#include <stc/vec.h>

struct GenValue {
    IVec *v;
    IVec_iter it;
    cco_base base;
};

static long get_value(struct GenValue* o)
{
    cco_async (o) {
        for (cco_each(o->it, IVec, *o->v))
            cco_yield_v(*o->it.ref);
    }
    return 1L<<31;
}

struct Generator {
    struct GenValue a, b;
    bool xactive, yactive;
    long value;
    cco_base base;
};

int interleaved(struct Generator* o)
{
    cco_async (o) {
        do {
            o->value = get_value(&o->a);
            o->xactive = cco_is_active(&o->a);
            if (o->xactive)
                cco_yield;

            o->value = get_value(&o->b);
            o->yactive = cco_is_active(&o->b);
            if (o->yactive)
                cco_yield;
        } while (o->xactive | o->yactive);
    }
    return 0;
}

void Use(void)
{
    IVec a = c_make(IVec, {2, 4, 6, 8, 10, 11});
    IVec b = c_make(IVec, {3, 5, 7, 9});

    struct Generator gen = {{&a}, {&b}};

    cco_run_coroutine(interleaved(&gen)) {
        printf("%ld ", gen.value);
    }
    puts("");
    c_drop(IVec, &a, &b);
}

int main(void)
{
    Use();
}
