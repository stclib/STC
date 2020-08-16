#include <stc/crandom.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include "others/khash.h"

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
#include <unordered_map>
#include "others/bytell_hash_map.hpp"
#include "others/robin_hood.hpp"
#endif

// Visual Studio: compile with -TP to force C++:  cl -TP -EHsc -O2 benchmark.c

static inline uint32_t fibfast_hash(const void* data, size_t len) {
    return (*(const uint64_t *) data) * 11400714819323198485llu;
}
// cmap, khash implementations
declare_cmap(ii, int64_t, int64_t, c_default_destroy, c_default_equals, fibfast_hash); // c_default_hash32);
KHASH_MAP_INIT_INT64(ii, uint64_t)

size_t seed;
static const float max_load_factor = 0.77f;

crandom_eng64_t rng;
#define SEED(s) rng = crandom_eng64_init(seed)
#define RAND(N) (crandom_i64(&rng) & ((1 << N) - 1))


#define CMAP_SETUP(Key, Value) cmap_ii map = cmap_init \
                                    ; cmap_ii_set_load_factors(&map, max_load_factor, 0.0)
#define CMAP_PUT(key, val)     cmap_ii_put(&map, key, val)->value
#define CMAP_ERASE(key)        cmap_ii_erase(&map, key)
#define CMAP_FIND(key)         (cmap_ii_find(map, key) != NULL)
#define CMAP_SIZE()            cmap_size(map)
#define CMAP_BUCKETS()         (map).bucket_count
#define CMAP_CLEAR()           cmap_ii_destroy(&map)

#define KMAP_SETUP(Key, Value) khash_t(ii)* map = kh_init(ii); khiter_t ki; int ret
#define KMAP_PUT(key, val)     (*(ki = kh_put(ii, map, key, &ret), map->vals[ki] = val, &map->vals[ki]))
#define KMAP_ERASE(key)        ((ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0)
#define KMAP_FIND(key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE()            kh_size(map)
#define KMAP_BUCKETS()         kh_n_buckets(map)
#define KMAP_CLEAR()           kh_destroy(ii, map)

#define UMAP_SETUP(Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define UMAP_PUT(key, val)     (map[key] = val)
#define UMAP_FIND(key)         (map.find(key) != map.end())
#define UMAP_ERASE(key)        map.erase(key)
#define UMAP_SIZE()            map.size()
#define UMAP_BUCKETS()         map.bucket_count()
#define UMAP_CLEAR()           map.clear()

#define BMAP_SETUP(Key, Value) ska::bytell_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define BMAP_PUT(key, val)     (map[key] = val)
#define BMAP_FIND(key)         (map.find(key) != map.end())
#define BMAP_ERASE(key)        map.erase(key)
#define BMAP_SIZE()            map.size()
#define BMAP_BUCKETS()         map.bucket_count()
#define BMAP_CLEAR()           map.clear()

#define FMAP_SETUP(Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define FMAP_PUT(key, val)     (map[key] = val)
#define FMAP_FIND(key)         (map.find(key) != map.end())
#define FMAP_ERASE(key)        map.erase(key)
#define FMAP_SIZE()            map.size()
#define FMAP_BUCKETS()         map.bucket_count()
#define FMAP_CLEAR()           map.clear()

#define RMAP_SETUP(Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(key, val)     (map[key] = val)
#define RMAP_FIND(key)         (map.find(key) != map.end())
#define RMAP_ERASE(key)        map.erase(key)
#define RMAP_SIZE()            map.size()
#define RMAP_BUCKETS()         map.mask()
#define RMAP_CLEAR()           map.clear()

const size_t N1 = 10000000 * 5;
const size_t N2 = 10000000 * 5;
const size_t N3 = 10000000 * 10;
#define      RR   20
int rr = RR;


#define MAP_TEST1(M) \
{ \
    M##_SETUP(int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++ M##_PUT(RAND(rr), i); \
        erased += M##_ERASE(RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, sum: %zu, erased %zu\n", \
           (size_t) M##_SIZE(), (size_t) M##_BUCKETS(), (float) difference / CLOCKS_PER_SEC, checksum, erased); \
    M##_CLEAR(); \
}

#define MAP_TEST2(M) \
{ \
    M##_SETUP(int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N2; ++i) \
        M##_PUT(i, i); \
    for (size_t i = 0; i < N2; ++i) \
        erased += M##_ERASE(i); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(), (size_t) M##_BUCKETS(), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(); \
}

#define MAP_TEST3(M) \
{ \
    M##_SETUP(int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        M##_PUT(RAND(rr), i); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        erased += M##_ERASE(RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(), (size_t) M##_BUCKETS(), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(); \
}

#ifndef __cplusplus
#define RUN_TEST(n) MAP_TEST##n(CMAP) MAP_TEST##n(KMAP)
#else
#define RUN_TEST(n) MAP_TEST##n(CMAP) MAP_TEST##n(KMAP) MAP_TEST##n(UMAP) MAP_TEST##n(BMAP) MAP_TEST##n(FMAP) MAP_TEST##n(RMAP)
#endif

int main(int argc, char* argv[])
{
    rr = argc == 2 ? atoi(argv[1]) : RR;
    seed = time(NULL);
    printf("\nRandom keys are in range [0, 2^%d), seed = %zu:\n",  rr, seed);
    printf("\nUnordered maps: %zu repeats of Insert random key + try to remove a random key:\n", N1);
    RUN_TEST(1)

    printf("\nUnordered maps: Insert %zu index keys, then remove them in same order:\n", N2);
    RUN_TEST(2)

    printf("\nUnordered maps: Insert %zu random keys, then remove them in same order:\n", N3);
    RUN_TEST(3)
#endif
}
