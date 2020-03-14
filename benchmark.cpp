#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <clib/cstring.h>
#include <clib/cmap.h>


declare_CMap(ii, int64_t, int64_t);
declare_CVector_string(s);

int main()
{
    clock_t before, difference;
    CMap_ii map = cmap_initializer;
    CMapEntry_ii* entry;
    uint64_t checksum = 0, erased, get;
    const size_t N = 50000000;

    printf("Starting %llu\n", N);
    //cmap_ii_reserve(&map, N * 1.25);
    cmap_ii_clear(&map);
    srand(123);
    before = clock();
    checksum = 0; erased = 0; get = 0;
    for (size_t i = 0; i < N; ++i) {
        int64_t rnd = rand();
        int op = rand() >> 13;
        switch (op) {
        case 1:
            checksum += ++cmap_ii_put(&map, rnd, i-1)->value;
            break;
        case 2:
            entry = cmap_ii_get(map, rnd);
            if (entry) { checksum += entry->value; ++erased; }
            /*erased +=*/ cmap_ii_erase(&map, rnd);
            break;
        case 3:
            get += (cmap_ii_get(map, rnd) != NULL);
            break;
        }
    }
    difference = clock() - before;
    printf("CMap_ii: size: %llu, time: %f, sum: %llu, erased: %llu, get %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum, erased, get);
    //c_foreach (i, cmap_ii, map) sum += cmap_ii_get(map, i.item->key)->value;
    //cmap_ii_destroy(&map);

    std::unordered_map<int64_t, int64_t> map2, map3;
    std::unordered_map<int64_t, int64_t>::const_iterator iter;
    //map2.reserve(N);
    srand(123);
    before = clock();
    checksum = 0; erased = 0; get = 0;
    for (size_t i = 0; i < N; ++i) {
        int64_t rnd = rand();
        int op = rand() >> 13;
        switch (op) {        
        case 1:
            checksum += ++(map2[rnd] = i-1);
            break;
        case 2:
            iter = map2.find(rnd);
            if (iter != map2.end()) { checksum += iter->second; ++erased; }
            /*erased +=*/ map2.erase(rnd);
            break;
        case 3:
            get += (map2.find(rnd) != map2.end());
            break;
        }
    }
    difference = clock() - before;
    printf("std::um: size: %llu, time: %f, sum: %llu, erased: %llu, get %llu\n", map2.size(), 1.0 * difference / CLOCKS_PER_SEC, checksum, erased, get);
    
    c_foreach (i, cmap_ii, map) map3[ i.item->key ] = i.item->value;
    printf("map3: %llu, %d\n", map3.size(), map2 == map3);
  

#if 0    
    srand(123);
    const size_t N = 40000000;
    CMap_ii map = cmap_initializer;
    CMapEntry_ii* entry;
    uint64_t checksum = 0, erased = 0, get = 0;
    uint64_t checksum2 = 0, erased2 = 0, get2 = 0;
    std::unordered_map<int64_t, int64_t> map2, map3;
    std::unordered_map<int64_t, int64_t>::const_iterator iter;
    srand(123);
    for (size_t i = 0; i < N; ++i) {
        int64_t rnd = rand();
        int op = rand() >> 13;
        switch (op) {        
        case 1:
            checksum += ++cmap_ii_put(&map, rnd, i-1)->value;
            checksum2 += ++(map2[rnd] = i-1);
            if (rnd == 16459) printf("%llu: put 16459: %llu\n", i, erased);
            break;
        case 2:
            erased += cmap_ii_erase(&map, rnd);        
            erased2 += map2.erase(rnd);
            if (erased != erased2) {
                printf("%llu: era %llu: %llu, %llu\n", i, rnd, erased, erased2);
                exit(0);
            }
            if (rnd == 16459) printf("%llu: era 16459: %llu\n", i, erased);
            break;
        case 3:
            get += (cmap_ii_get(map, rnd) != NULL);
            get2 += (map2.find(rnd) != map2.end());
            break;
        }
    }    
    printf("cmap_ii: size: %llu, sum: %llu, erased: %llu, get %llu\n", cmap_size(map), checksum, erased, get);
    printf("std::um: size: %llu, sum: %llu, erased: %llu, get %llu\n", map2.size(), checksum2, erased2, get2);
#endif    
    
    cmap_ii_destroy(&map);
    map2.clear();
}