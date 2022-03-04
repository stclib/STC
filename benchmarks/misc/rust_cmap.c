#include <time.h>
#include <stdio.h>
#define i_key uint64_t
#define i_val uint64_t
#define i_hash c_hash64
#define i_tag u64
#include <stc/cmap.h>

uint64_t romu_rotl(uint64_t val, uint32_t r) {
    return (val << r) | (val >> (64 - r));
}

uint64_t romu_trio(uint64_t s[3]) {
   const uint64_t xp = s[0],
                  yp = s[1],
                  zp = s[2];
   s[0] = 15241094284759029579u * zp;
   s[1] = yp - xp;
   s[1] = romu_rotl(s[1], 12);
   s[2] = zp - yp;
   s[2] = romu_rotl(s[2], 44);
   return xp;
}

int main()
{
    c_auto (cmap_u64, m) {
        const size_t n = 50000000, 
                     mask = (1 << 25) - 1,
                     ms = CLOCKS_PER_SEC/1000; 
        cmap_u64_max_load_factor(&m, 0.8);
        cmap_u64_reserve(&m, n);
        printf("STC cmap  n = %" PRIuMAX ", mask = 0x%" PRIxMAX "\n", n, mask);

        uint64_t rng[3] = {1872361123, 123879177, 87739234}, sum;
        clock_t now = clock();
        c_forrange (n) {
            uint64_t key = romu_trio(rng) & mask;
            cmap_u64_insert(&m, key, 0).ref->second += 1;
        }
        printf("insert  : %zums  \tsize : %" PRIuMAX "\n", (clock() - now)/ms, cmap_u64_size(m));

        now = clock();
        sum = 0;
        c_forrange (key, mask + 1) { sum += cmap_u64_contains(&m, key); }
        printf("lookup  : %zums  \tsum  : %" PRIuMAX "\n", (clock() - now)/ms, sum);

        now = clock();
        sum = 0;
        c_foreach (i, cmap_u64, m) { sum += i.ref->second; }
        printf("iterate : %zums  \tsum  : %" PRIuMAX "\n", (clock() - now)/ms, sum);

        uint64_t rng2[3] = {1872361123, 123879177, 87739234};
        now = clock();
        c_forrange (n) {
            uint64_t key = romu_trio(rng2) & mask;
            cmap_u64_erase(&m, key);
        }
        printf("remove  : %zums  \tsize : %" PRIuMAX "\n", (clock() - now)/ms, cmap_u64_size(m));
        printf("press a key:\n"); getchar();
    }
}