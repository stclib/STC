// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

#include <stc/algo/coroutine.h>
#include <stdio.h>

void triples_vanilla(int n) {
    for (int i = 1, z = 1;; ++z) {
        for (int x = 1; x < z; ++x) {
            for (int y = x; y < z; ++y) {
                if (x*x + y*y == z*z) {
                    printf("{%d, %d, %d},\n", x, y, z);
                    if (++i > n) goto done;
                }
            }
        }
    }
    done:;
}

struct triples {
    int n;
    int x, y, z;
    int cco_state;
};

bool triples_coro(struct triples* t) {
    cco_begin(t);
        for (t->z = 1;; ++t->z) {
            for (t->x = 1; t->x < t->z; ++t->x) {
                for (t->y = t->x; t->y < t->z; ++t->y) {
                    if (t->x*t->x + t->y*t->y == t->z*t->z) {
                        if (t->n-- == 0) cco_return;
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
    struct triples t = {6};
    while (triples_coro(&t))
        printf("{%d, %d, %d},\n", t.x, t.y, t.z);
}
