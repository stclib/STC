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
    const uint64_t key = *(const uint64_t *) data;
    return (uint32_t) (key * 11400714819323198485llu);
}

declare_cmap(ii, int64_t, int64_t, c_default_destroy, c_default_equals, fibfast_hash); // c_fibonacci_hash64);

KHASH_MAP_INIT_INT64(ii, uint64_t)

size_t seed;
static const float max_load_factor = 0.77f;

crandom_eng64_t rng;
#define SEED(s) rng = crandom_eng64_init(seed)
#define RAND(N) (crandom_gen_i64(&rng) & ((1 << N) - 1))


#define CMAP_SETUP(tag, Key, Value) cmap_##tag map = cmap_init \
                                    ; cmap_##tag##_set_load_factors(&map, max_load_factor, 0.0)
#define CMAP_PUT(tag, key, val)     cmap_##tag##_put(&map, key, val)->value
#define CMAP_ERASE(tag, key)        cmap_##tag##_erase(&map, key)
#define CMAP_FIND(tag, key)         (cmap_##tag##_find(map, key) != c_nullptr)
#define CMAP_SIZE(tag)              cmap_size(map)
#define CMAP_BUCKETS(tag)           (map).bucket_count
#define CMAP_CLEAR(tag)             cmap_##tag##_destroy(&map)

#define KMAP_SETUP(tag, Key, Value) khash_t(ii)* map = kh_init(ii); khiter_t ki; int ret
#define KMAP_PUT(tag, key, val)     (*(ki = kh_put(ii, map, key, &ret), map->vals[ki] = val, &map->vals[ki]))
#define KMAP_ERASE(tag, key)        ((ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0)
#define KMAP_FIND(tag, key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE(tag)              kh_size(map)
#define KMAP_BUCKETS(tag)           kh_n_buckets(map)
#define KMAP_CLEAR(tag)             kh_destroy(ii, map)

#define UMAP_SETUP(tag, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define UMAP_PUT(tag, key, val)     (map[key] = val)
#define UMAP_FIND(tag, key)         (map.find(key) != map.end())
#define UMAP_ERASE(tag, key)        map.erase(key)
#define UMAP_SIZE(tag)              map.size()
#define UMAP_BUCKETS(tag)           map.bucket_count()
#define UMAP_CLEAR(tag)             map.clear()

#define BMAP_SETUP(tag, Key, Value) ska::bytell_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define BMAP_PUT(tag, key, val)     (map[key] = val)
#define BMAP_FIND(tag, key)         (map.find(key) != map.end())
#define BMAP_ERASE(tag, key)        map.erase(key)
#define BMAP_SIZE(tag)              map.size()
#define BMAP_BUCKETS(tag)           map.bucket_count()
#define BMAP_CLEAR(tag)             map.clear()

#define FMAP_SETUP(tag, Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define FMAP_PUT(tag, key, val)     (map[key] = val)
#define FMAP_FIND(tag, key)         (map.find(key) != map.end())
#define FMAP_ERASE(tag, key)        map.erase(key)
#define FMAP_SIZE(tag)              map.size()
#define FMAP_BUCKETS(tag)           map.bucket_count()
#define FMAP_CLEAR(tag)             map.clear()

#define RMAP_SETUP(tag, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(tag, key, val)     (map[key] = val)
#define RMAP_FIND(tag, key)         (map.find(key) != map.end())
#define RMAP_ERASE(tag, key)        map.erase(key)
#define RMAP_SIZE(tag)              map.size()
#define RMAP_BUCKETS(tag)           map.bucket_count()
#define RMAP_CLEAR(tag)             map.clear()

const size_t N1 = 10000000 * 5;
const size_t N2 = 10000000 * 5;
const size_t N3 = 10000000 * 10;
#define      RR   20
int rr = RR;


#define MAP_TEST1(M, tag) \
{ \
    M##_SETUP(tag, int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++ M##_PUT(tag, RAND(rr), i); \
        erased += M##_ERASE(tag, RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, sum: %zu, erased %zu\n", \
           (size_t) M##_SIZE(tag), (size_t) M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, checksum, erased); \
    M##_CLEAR(tag); \
}

#define MAP_TEST2(M, tag) \
{ \
    M##_SETUP(tag, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N2; ++i) \
        M##_PUT(tag, i, i); \
    for (size_t i = 0; i < N2; ++i) \
        erased += M##_ERASE(tag, i); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(tag), (size_t) M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tag); \
}

#define MAP_TEST3(M, tag) \
{ \
    M##_SETUP(tag, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        M##_PUT(tag, RAND(rr), i); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        erased += M##_ERASE(tag, RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(tag), (size_t) M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tag); \
}


int main(int argc, char* argv[])
{
    rr = argc == 2 ? atoi(argv[1]) : RR;
    seed = time(NULL);
    printf("\nRandom keys are in range [0, 2^%d), seed = %zu:\n",  rr, seed);
    printf("\nUnordered maps: %zu repeats of Insert random key + try to remove a random key:\n", N1);
    MAP_TEST1(CMAP, ii)
    MAP_TEST1(KMAP, ii)
#ifdef __cplusplus
    MAP_TEST1(UMAP, ii)
    MAP_TEST1(BMAP, ii)
    MAP_TEST1(FMAP, ii)
    MAP_TEST1(RMAP, ii)
#endif

    printf("\nUnordered maps: Insert %zu index keys, then remove them in same order:\n", N2);
    MAP_TEST2(CMAP, ii)
    MAP_TEST2(KMAP, ii)
#ifdef __cplusplus
    MAP_TEST2(UMAP, ii)
    MAP_TEST2(BMAP, ii)
    MAP_TEST2(FMAP, ii)
    MAP_TEST2(RMAP, ii)
#endif

    printf("\nUnordered maps: Insert %zu random keys, then remove them in same order:\n", N3);
    MAP_TEST3(CMAP, ii)
    MAP_TEST3(KMAP, ii)
#ifdef __cplusplus
    MAP_TEST3(UMAP, ii)
    MAP_TEST3(BMAP, ii)
    MAP_TEST3(FMAP, ii)
    MAP_TEST3(RMAP, ii)
#endif
}
