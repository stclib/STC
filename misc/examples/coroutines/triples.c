// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include "stc/coroutine.h"

void triples_vanilla(int max_c) {
    for (int c = 5, i = 0;; ++c) {
        for (int a = 1; a < c; ++a) {
            for (int b = a + 1; b < c; ++b) {
                if ((int64_t)a*a + (int64_t)b*b == (int64_t)c*c) {
                    if (c > max_c)
                        goto done;
                    printf("%d: {%d, %d, %d}\n", ++i, a, b, c);
                }
            }
        }
    }
    done:;
}

struct triples {
    int max_c;
    int a, b, c;
    int cco_state;
};

int triples_coro(struct triples* t) {
    cco_scope(t) {
        for (t->c = 5;; ++t->c) {
            for (t->a = 1; t->a < t->c; ++t->a) {
                for (t->b = t->a + 1; t->b < t->c; ++t->b) {
                    if ((int64_t)t->a * t->a +
                        (int64_t)t->b * t->b ==
                        (int64_t)t->c * t->c)
                    {
                        if (t->c > t->max_c)
                            cco_return;
                        cco_yield;
                    }
                }
            }
        }
        cco_final:
        puts("done");
    }
    return 0;
}

int gcd(int a, int b) {
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int main(void)
{
    puts("Vanilla triples:");
    triples_vanilla(20);

    puts("\nCoroutine triples with GCD = 1:");
    struct triples t = {.max_c = 100};
    int n = 0;

    cco_run_coroutine(triples_coro(&t)) {
        if (gcd(t.a, t.b) > 1)
            continue;
        if (++n <= 20)
            printf("%d: {%d, %d, %d}\n", n, t.a, t.b, t.c);
        else
            cco_stop(&t);
    }
}
