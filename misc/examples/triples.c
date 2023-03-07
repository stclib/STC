// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stc/algo/coroutine.h>
#include <stdio.h>

void triples_vanilla(int n) {
    for (int i = 1, c = 1;; ++c) {
        for (int a = 1; a < c; ++a) {
            for (int b = a; b < c; ++b) {
                if (a*a + b*b == c*c) {
                    printf("{%d, %d, %d},\n", a, b, c);
                    if (++i > n) goto done;
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
        for (I->c = 1;; ++I->c) {
            for (I->a = 1; I->a < I->c; ++I->a) {
                for (I->b = I->a; I->b < I->c; ++I->b) {
                    if (I->a*I->a + I->b*I->b == I->c*I->c) {
                        if (I->n-- == 0) cco_return;
                        cco_yield(true);
                    }
                }
            }
        }
        cco_final:
    cco_end(false);
}


int main()
{
    puts("Vanilla triples:");
    triples_vanilla(6);

    puts("\nCoroutine triples:");
    struct triples t = {INT32_MAX};
    while (triples_next(&t)) {
        if (t.c < 100) printf("{%d, %d, %d},\n", t.a, t.b, t.c);
        else cco_stop(&t);
    }
}
