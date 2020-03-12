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
    clock_t before, difference;
    CMap_ii map = cmap_initializer;

    uint64_t checksum = 0, erased, get;
    const size_t N = 50000000;

    printf("Starting %llu\n", N);
    //cmap_ii_reserve(&map, N * 1.25);
    cmap_ii_clear(&map);
    srand(123);
    before = clock();
    checksum = 0; erased = 0; get = 0;
    for (size_t i = 0; i < N; ++i) {
		int rnd = rand();
		int op = rand() >> 13;
		switch (op) {
		case 1:
			checksum += ++cmap_ii_put(&map, rnd, i-1)->value;
			break;
		case 2:
			if (cmap_size(map) > 0) erased += cmap_ii_erase(&map, rnd);
			break;
		case 3:
			get += (cmap_ii_get(map, rnd) != NULL);
			break;
		}
    }
    difference = clock() - before;
    printf("CMap_ii: size: %llu, time: %f, sum: %llu, erased: %llu, get %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum, erased, get);
    //c_foreach (i, cmap_ii, map) sum += cmap_ii_get(map, i.item->key)->value;
    cmap_ii_destroy(&map);

    std::unordered_map<int, int> map2;
    //map2.reserve(N);
    srand(123);
    before = clock();
    checksum = 0; erased = 0; get = 0;
    for (size_t i = 0; i < N; ++i) {
		int rnd = rand();
		int op = rand() >> 13;
		switch (op) {		
		case 1:
			checksum += ++(map2[rnd] = i-1);
			break;
		case 2:
			if (map2.size() > 0) erased += map2.erase(rnd);
			break;
		case 3:
			get += (map2.find(rnd) != map2.end());
			break;
		}
    }
    difference = clock() - before;
    printf("std::um: size: %llu, time: %f, sum: %llu, erased: %llu, get %llu\n", cmap_size(map), 1.0 * difference / CLOCKS_PER_SEC, checksum, erased, get);
    map2.clear();
}