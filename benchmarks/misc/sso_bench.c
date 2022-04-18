// https://gobyexample.com/maps
#include <stc/alt/cstr.h>
#define i_type Map
#define i_key_str
#define i_val int
#include <stc/cmap.h>
#define i_type Vec
#define i_val_str
#include <stc/cvec.h>

#include <time.h>
#include <stc/crandom.h>

void rndstr(char buf[64], int max) {
    unsigned n = crandom() % max;
    static char chr[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
    c_forrange (i, n)
        buf[i] = chr[crandom() & 63];
    buf[n] = '\0';
}

enum {N = 10000000};

int main(void) {

    c_auto (Vec, vec)
    c_auto (Map, map) {
        char buf[128];

        csrandom(time(NULL));
        printf("map\n");
        clock_t t0 = clock(), t1, t2;
        c_forrange (i, int, N) {
            rndstr(buf, 64);
            Map_emplace(&map, buf, i);
        }
        t1 = clock();
        printf("vec\n");
        c_apply_cnt(v, Vec_push(&vec, cstr_clone(v->first)), Map, map);
        t2 = clock();
        puts("map items:");
        int k = 0;
        c_forpair (k, v, Map, map)
            if (k++ == 5) break; else printf("  %s: %d\n", cstr_str(&_.k), _.v);
        puts("vec items:");
        k = 0;
        c_foreach (i, Vec, vec)
            if (k++ == 5) break; else printf("  %s\n", cstr_str(i.ref));

        printf("\nmap insert: %d ms\nvec pushes: %d ms\n", (int)((t1 - t0)*10000/CLOCKS_PER_SEC), (int)((t2 - t1)*10000/CLOCKS_PER_SEC));
    }
}
