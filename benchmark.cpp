#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cstring.h"
#include "cmap.h"
#include <unordered_map>


declare_CMap(ii, int, int);
declare_CStringVector(s);

int main()
{
    CMap_ii map = cmap_initializer;
    uint64_t checksum = 0;
    clock_t before, difference;
    size_t fib1, fib2, fibx;
    
    const size_t N = 10000000;

    printf("Starting\n");
    //cmap_ii_reserve(&map, N * 1.25);
    before = clock();
    fib1 = 0, fib2 = 1;
    for (size_t i = 0; i < N; ++i) {
        checksum += ++cmap_ii_put(&map, FIBONACCI_NEXT, i)->value;
    }
    difference = clock() - before;
    printf("Check: %llu, size: %llu, time: %f\n", checksum, cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC);
    cmap_ii_destroy(&map);

    std::unordered_map<int, int> map2;
    //map2.reserve(N);
    before = clock();
    fib1 = 0, fib2 = 1; checksum = 0;
    for (size_t i = 0; i < N; ++i)
        checksum += ++(map2[FIBONACCI_NEXT] = i);
    difference = clock() - before;
    printf("Check: %llu, size: %llu, time: %f\n", checksum, map2.size(), 1.0 * difference / CLOCKS_PER_SEC);
    map2.clear();
}