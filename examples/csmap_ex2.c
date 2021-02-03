#include <stc/csmap.h>
#include <stc/crandom.h>
#include <stdio.h>
#include <time.h>
#ifdef __cplusplus
#include <map>
#endif

using_csmap(i, int, int);
enum {N=2000};
int main()
{
    clock_t t1, t2, t3, t4, t5;
    {
        t1 = clock();
        csmap_i map = csmap_i_init();
        stc64_srandom(123);
        c_forrange (i, 12) {
            int x = stc64_random() & 0xff;
            csmap_i_emplace(&map, x, i);
            printf(" %d", x);
        }
        puts("");
        c_foreach (i, csmap_i, map) printf(" %d", i.ref->first);
        puts("\n");
        //c_foreach (i, csmap_i, map)
            //printf("%d: %d\n", i.ref->first, i.ref->second);
        printf("size %zu\n", csmap_i_size(map));

        stc64_srandom(123);
        c_forrange (i, 6) {
            int x = stc64_random() & 0xff;
            printf("Try erase: %d\n", x);
            csmap_i_erase(&map, x);
            c_foreach (i, csmap_i, map) printf(" %d", i.ref->first);
            puts("");
        }
        t2 = clock();
        size_t n = 0, sum = 0;
        c_foreach (i, csmap_i, map)
            sum += i.ref->first;
        t3 = clock();
        //c_foreach (i, csmap_i, map)
            //if (n++ < 20) printf("%d: %d\n", i.ref->first, i.ref->second); else break;
        printf("size %zu: %zu\n", csmap_i_size(map), sum);

        t4 = clock();
        csmap_i_del(&map);
    }
    t5 = clock();
    printf("emplace: %g sec, sum: %g sec, destruct: %g sec\n", (float)(t2-t1)/CLOCKS_PER_SEC, (float)(t3-t2)/CLOCKS_PER_SEC, (float)(t5-t4)/CLOCKS_PER_SEC);
#ifdef __cplusplus
    {
        t1 = clock();
        std::map<int, int> map;
        stc64_srandom(1);
        c_forrange (i, N)
            map.emplace(stc64_random() & 0xffffff, i);
        c_forrange (i, N/2)
            map.erase(stc64_random() & 0xffffff);
        t2 = clock();
        size_t n = 0, sum = 0;
        for (auto i: map)
            sum += i.first;
        t3 = clock();
        for (auto i: map)
            if (n++ < 20) printf("%d: %d\n", i.first, i.second); else break;
        printf("size %zu: %zu\n", map.size(), sum);
        t4 = clock();
    }
    t5 = clock();
    printf("emplace %g sec, sum: %g sec, destruct: %g sec\n", (float)(t2-t1)/CLOCKS_PER_SEC, (float)(t3-t2)/CLOCKS_PER_SEC, (float)(t5-t4)/CLOCKS_PER_SEC);
#endif
}