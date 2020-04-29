#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "stc/cstring.h"
#include "stc/cmap.h"
#ifdef __cplusplus
#include <unordered_map>
#include "others/bytell_hash_map.hpp"
#include "others/robin_hood.hpp"
#endif

// Visual Studio: compile with -TP to force C++:  cl -TP -EHsc -O2 benchmark.c

declare_CMap(ii, int64_t, int64_t, c_noDestroy, c_lowbias32Hash);
declare_CMap(ix, short, short); // sizeof(CMapEntry_ix) = 6 bytes only!

const size_t seed = 123; // time(NULL);
const double maxLoadFactor = 0.77;
#define RAND(N) ((rand() << ((N) - 15)) ^ rand()) // N=16-30

#define CMAP_SETUP(tag, Key, Value) CMap_##tag map = cmap_init; \
                                    cmap_##tag##_setMaxLoadFactor(&map, maxLoadFactor)
#define CMAP_PUT(tag, __key, __value)   cmap_##tag##_put(&map, __key, __value)->value
#define CMAP_DEL(tag, key)          cmap_##tag##_erase(&map, key)
#define CMAP_FIND(tag, key)         (cmap_##tag##_get(map, key) != NULL)
#define CMAP_SIZE(tag)              cmap_size(map)
#define CMAP_BUCKETS(tag)           cmap_bucketCount(map)
#define CMAP_CLEAR(tag)             cmap_##tag##_destroy(&map)

#define UMAP_SETUP(tag, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(maxLoadFactor)
#define UMAP_PUT(tag, key, value)   (map[key] = value)
#define UMAP_FIND(tag, key)         (map.find(key) != map.end())
#define UMAP_DEL(tag, key)          map.erase(key)
#define UMAP_SIZE(tag)              map.size()
#define UMAP_BUCKETS(tag)           map.bucket_count()
#define UMAP_CLEAR(tag)             map.clear()

#define BMAP_SETUP(tag, Key, Value) ska::bytell_hash_map<Key, Value> map; map.max_load_factor(maxLoadFactor)
#define BMAP_PUT(tag, key, value)   (map[key] = value)
#define BMAP_FIND(tag, key)         (map.find(key) != map.end())
#define BMAP_DEL(tag, key)          map.erase(key)
#define BMAP_SIZE(tag)              map.size()
#define BMAP_BUCKETS(tag)           map.bucket_count()
#define BMAP_CLEAR(tag)             map.clear()

#define FMAP_SETUP(tag, Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(maxLoadFactor)
#define FMAP_PUT(tag, key, value)   (map[key] = value)
#define FMAP_FIND(tag, key)         (map.find(key) != map.end())
#define FMAP_DEL(tag, key)          map.erase(key)
#define FMAP_SIZE(tag)              map.size()
#define FMAP_BUCKETS(tag)           map.bucket_count()
#define FMAP_CLEAR(tag)             map.clear()

#define RMAP_SETUP(tag, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(tag, key, value)   (map[key] = value)
#define RMAP_FIND(tag, key)         (map.find(key) != map.end())
#define RMAP_DEL(tag, key)          map.erase(key)
#define RMAP_SIZE(tag)              map.size()
#define RMAP_BUCKETS(tag)           map.bucket_count()
#define RMAP_CLEAR(tag)             map.clear()

const size_t N1 = 7000000;
const size_t N2 = 10000000;
#define      RR   24
int rr = RR;

#define MAP_TEST1(M, tag) \
{ \
    M##_SETUP(tag, int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    srand(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++M##_PUT(tag, RAND(rr), i); \
        erased += M##_DEL(tag, RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bucks: %llu, time: %.02f, sum: %llu, erase: %llu\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, checksum, erased); \
    M##_CLEAR(tag); \
}

#define MAP_TEST2(M, tag) \
{ \
    M##_SETUP(tag, int64_t, int64_t); \
    srand(seed); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N2; ++i) \
        M##_PUT(tag, i*17, i); \
    for (size_t i = 0; i < N2; ++i) \
        erased += M##_DEL(tag, i*17); \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bucks: %llu, time: %.02f, erase %llu\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tag); \
}


int main(int argc, char* argv[])
{
    rr = argc == 2 ? atoi(argv[1]) : RR;
    printf("\nmap<uint64_t, uint64_t>: Insert + remove %llu random keys in range 0 - 2^%d:\n", N1, rr);
    MAP_TEST1(CMAP, ii)
#ifdef __cplusplus
    MAP_TEST1(UMAP, ii)
    MAP_TEST1(BMAP, ii)
    MAP_TEST1(FMAP, ii)
    MAP_TEST1(RMAP, ii)
#endif

    printf("\nmap<uint64_t, uint64_t>: Insert %llu keys, THEN remove them in same order:\n", N2);
    MAP_TEST2(CMAP, ii)
#ifdef __cplusplus
    MAP_TEST2(UMAP, ii)
    MAP_TEST2(BMAP, ii)
    MAP_TEST2(FMAP, ii)
    MAP_TEST2(RMAP, ii)
#endif

}
