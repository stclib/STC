// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stc/algo/coroutine.h>
#include <stdio.h>

void triples_vanilla(int n) {
    for (int c = 5; n; ++c) {
        for (int a = 1; a < c; ++a) {
            for (int b = a + 1; b < c; ++b) {
                if ((int64_t)a*a + (int64_t)b*b == (int64_t)c*c) {
                    printf("{%d, %d, %d}\n", a, b, c);
                    if (--n == 0) goto done;
                }
            }
        }
    }
    done:;
}

struct triples {
    int n;
    int a, b, c;
    int cco_state;
};

bool triples_next(struct triples* I) {
    cco_begin(I);
        for (I->c = 5; I->n; ++I->c) {
            for (I->a = 1; I->a < I->c; ++I->a) {
                for (I->b = I->a + 1; I->b < I->c; ++I->b) {
                    if ((int64_t)I->a*I->a + (int64_t)I->b*I->b == (int64_t)I->c*I->c) {
                        cco_yield(true);
                        if (--I->n == 0) cco_return;
                    }
                }
            }
        }
        cco_final:
        puts("done");
    cco_end(false);
}

int gcd(int a, int b) {
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int main()
{
    puts("Vanilla triples:");
    triples_vanilla(6);

    puts("\nCoroutine triples:");
    struct triples t = {INT32_MAX};
    int n = 0;

    while (triples_next(&t)) {
        if (gcd(t.a, t.b) > 1)
            continue;
        if (t.c < 100)
            printf("%d: {%d, %d, %d}\n", ++n, t.a, t.b, t.c);
        else
            cco_stop(&t);
    }
}
