// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stdio.h>
#include <stc/coroutine.h>

int gcd(int a, int b) {
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

void triples_vanilla(int n) {
    for (int c = 5, i = 0; n; ++c) {
        for (int a = 1; a < c; ++a) {
            for (int b = a + 1; b < c; ++b) {
                if ((int64_t)a*a + (int64_t)b*b == (int64_t)c*c && gcd(a, b) == 1) {
                    printf("%d: {%d, %d, %d}\n", ++i, a, b, c);
                    if (--n == 0) goto done;
                }
            }
        }
    }
    done:;
}

struct triples {
    int size, count;
    int a, b, c;
    int cco_state;
};

int triples_coro(struct triples* t) {
    cco_routine(t) {
        t->count = 0;
        for (t->c = 5; t->size; ++t->c) {
            for (t->a = 1; t->a < t->c; ++t->a) {
                for (t->b = t->a + 1; t->b < t->c; ++t->b) {
                    if ((int64_t)t->a*t->a + (int64_t)t->b*t->b == (int64_t)t->c*t->c) {
                        if (t->count++ == t->size)
                            cco_return;
                        cco_yield();
                    }
                }
            }
        }
        cco_cleanup:
        puts("done");
    }
    return 0;
}

int main(void)
{
    puts("Vanilla triples:");
    triples_vanilla(5);

    puts("\nCoroutine triples:");
    struct triples t = {.size=INT32_MAX};
    int n = 0;

    while (triples_coro(&t)) {
        if (gcd(t.a, t.b) > 1)
            continue;
        if (t.c < 100)
            printf("%d: {%d, %d, %d}\n", ++n, t.a, t.b, t.c);
        else
            cco_stop(&t);
    }
}
