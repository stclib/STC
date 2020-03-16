#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <c_lib/cstring.h>
#include <c_lib/cmap.h>

declare_CMap(ii, int64_t, int64_t);
declare_CVector_string(s);

#define RAND() rand() * rand()

int main()
{
    clock_t before, difference;
    CMap_ii map = cmap_initializer;
    CMapEntry_ii* entry;
    uint64_t checksum, inserted, erased, get;
    const size_t N = 70000000, seed = 123; // time(NULL);

    printf("Starting %llu\n", N);
    cmap_ii_clear(&map);
    srand(seed);
    before = clock();
    checksum = inserted = erased = get = 0;
    for (size_t i = 0; i < N; ++i) {
        int64_t rnd = RAND();
        int op = rand() >> 13;
        switch (op) {
        case 1:
            ++inserted;
            checksum += ++cmap_ii_put(&map, rnd, i-1)->value;
            break;
        case 2:
            entry = cmap_ii_get(map, rnd);
            if (entry) { 
                ++erased;
                checksum += entry->value;
                cmap_ii_erase(&map, rnd);
            }
            break;
        case 3:
            get += (cmap_ii_get(map, rnd) != NULL);
            break;
        }
    }
    difference = clock() - before;
    printf("CMap: sz: %llu, bk: %llu, time: %.02f, sum: %llu, ins: %llu del: %llu\n", cmap_size(map), cmap_buckets(map), (float) difference / CLOCKS_PER_SEC, checksum, inserted, erased);
    cmap_ii_destroy(&map);
   

    std::unordered_map<int64_t, int64_t> map2;
    std::unordered_map<int64_t, int64_t>::const_iterator iter;
    srand(seed);
    before = clock();
    checksum = inserted = erased = get = 0;
    for (size_t i = 0; i < N; ++i) {
        int64_t rnd = RAND();
        int op = rand() >> 13;
        switch (op) {        
        case 1:
            ++inserted;
            checksum += ++(map2[rnd] = i-1);
            break;
        case 2:
            iter = map2.find(rnd);
            if (iter != map2.end()) { 
                ++erased;
                checksum += iter->second;
                map2.erase(rnd);
            }
            break;
        case 3:
            get += (map2.find(rnd) != map2.end());
            break;
        }
    }
    difference = clock() - before;
    printf("umap: sz: %llu, bk: %llu, time: %.02f, sum: %llu, ins: %llu del: %llu\n", map2.size(), map2.bucket_count(), (float) difference / CLOCKS_PER_SEC, checksum, inserted, erased);
    map2.clear();
}
