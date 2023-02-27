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

struct tricoro {
    int cco_state;
    int x, y, z;
};

bool triples_coro(struct tricoro* t) {
    cco_begin(t);
        for (t->z = 1;; ++t->z) {
            for (t->x = 1; t->x < t->z; ++t->x) {
                for (t->y = t->x; t->y < t->z; ++t->y) {
                    if (t->x*t->x + t->y*t->y == t->z*t->z) {
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
    triples_vanilla(20);

    puts("\nCoroutine triples:");
    struct tricoro t = {0};
    int i = 0;
    while (i++ < 20 && triples_coro(&t))
        printf("{%d, %d, %d},\n", t.x, t.y, t.z);
    triples_coro(cco_stop(&t));
}