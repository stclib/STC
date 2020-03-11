#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "c_string.h"
#include "c_hashmap_.h"
#include <unordered_map>


c_declare_Hashmap(ii, int, int);
declare_c_StringVector(s);

int main()
{
    c_Hashmap_ii map = c_hashmap_initializer;
    uint64_t checksum = 0;
    clock_t before, difference;
    size_t fib1, fib2, fibx;
    
    const size_t N = 10000000;

    printf("Starting\n");
    //c_hashmap_ii_reserve(&map, N * 1.25);
    before = clock();
    fib1 = 0, fib2 = 1;
    for (size_t i = 0; i < N; ++i) {
        checksum += ++c_hashmap_ii_put(&map, FIBONACCI_NEXT, i)->value;
    }
    difference = clock() - before;
    printf("Check: %llu, size: %llu, time: %f\n", checksum, c_hashmap_size(map), 1.0 * difference / CLOCKS_PER_SEC);
    c_hashmap_ii_destroy(&map);

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