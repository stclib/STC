// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include <stc/coroutine.h>

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
    cco_base base;
};

int triples_coro(struct triples* o) {
    cco_async (o) {
        for (o->c = 5;; ++o->c) {
            for (o->a = 1; o->a < o->c; ++o->a) {
                for (o->b = o->a + 1; o->b < o->c; ++o->b) {
                    if ((int64_t)o->a * o->a +
                        (int64_t)o->b * o->b ==
                        (int64_t)o->c * o->c)
                    {
                        if (o->c > o->max_c)
                            cco_return;
                        cco_yield;
                    }
                }
            }
        }

        cco_finalize:
        puts("done");
    }
    return 0;
}

int gcd(int a, int b) {
    while (b) {
        int tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

int main(void)
{
    puts("Vanilla triples:");
    triples_vanilla(20);

    puts("\nCoroutine triples with GCD = 1:");
    struct triples tri = {.max_c = 100};
    int n = 0;

    cco_run_coroutine(triples_coro(&tri)) {
        if (gcd(tri.a, tri.b) > 1)
            continue;
        if (++n <= 20)
            printf("%d: {%d, %d, %d}\n", n, tri.a, tri.b, tri.c);
        else
            cco_stop(&tri);
    }
}
