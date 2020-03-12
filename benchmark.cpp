#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <clib/cstring.h>
#include <clib/cmap.h>


declare_CMap(ii, int, int);
declare_CVector_string(s);

int main()
{
    CMap_ii map = cmap_initializer;
    int K = 300000;
    printf("BEGIN\n");
    //cmap_ii_reserve(&map, 50);
    /*
    cmap_ii_setMaxLoadFactor(&map, 0.8);
    for (size_t i = 0; i < K; ++i) {
        cmap_ii_put(&map, i*i, i);
    }
    for (size_t i = 0; i < K/2; ++i) {
        cmap_ii_erase(&map, i*i);
    }
    c_foreach (i, cmap_ii, map) cmap_ii_bucket(map, i.item->key);
    for (size_t i = 0; i < K/2; ++i) {
        cmap_ii_put(&map, i, i*i);
    }    
    c_foreach (i, cmap_ii, map) cmap_ii_bucket(map, i.item->key);
    */
    
    uint64_t checksum = 0;
    clock_t before, difference;
    size_t fib1, fib2, fibx;
    
    const size_t N = 20000000;

    printf("Starting %llu\n", N);
    //cmap_ii_reserve(&map, N * 1.25);
    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N; ++i) {
        checksum += ++cmap_ii_put(&map, FIBONACCI_NEXT, i)->value;
    }
    difference = clock() - before;
    printf("CMap_ii added: size: %llu, time: %f, check: %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum);
    before = clock();
    fib1 = 0, fib2 = 1;
    for (size_t i = 0; i < N*2/3; ++i) {
        cmap_ii_erase(&map, FIBONACCI_NEXT);
    }
    difference = clock() - before;
    printf("CMap_ii removed: size: %llu, time: %f, check: %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum);

    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N; ++i) {
        checksum += ++cmap_ii_put(&map, i, i)->value;
    }
    difference = clock() - before;
    printf("CMap_ii re-add: size: %llu, time: %f, check: %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum);
    
    size_t sum = 0;
    c_foreach (i, cmap_ii, map) sum += cmap_ii_get(map, i.item->key)->value;
    
    cmap_ii_destroy(&map);


    std::unordered_map<int, int> map2;
    //map2.reserve(N);
    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N; ++i)
        checksum += ++(map2[FIBONACCI_NEXT] = i);
    difference = clock() - before;
    printf("std::unordered_map added: size: %llu, time: %f, check: %llu\n", map2.size(), 1.0 * difference / CLOCKS_PER_SEC, checksum);
    
    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N*2/3; ++i)
        map2.erase(FIBONACCI_NEXT);
    difference = clock() - before;
    printf("std::unordered_map erased: size: %llu, time: %f\n", map2.size(), 1.0 * difference / CLOCKS_PER_SEC);
   
    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N; ++i)
        checksum += ++(map2[i] = i);
    difference = clock() - before;
    printf("std::unordered_map re-add: size: %llu, time: %f, check: %llu\n", map2.size(), 1.0 * difference / CLOCKS_PER_SEC, checksum);
    
    map2.clear();
}