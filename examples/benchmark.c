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
#include "others/hopscotch_map.h"
#include "others/sparsepp/spp.h"
template<typename C> inline void destroy_me(C& c) { C().swap(c); }
#endif

// Visual Studio: compile with -TP to force C++:  cl -TP -EHsc -O2 benchmark.c

static inline uint32_t fibonacci_hash(const void* data, size_t len) {
    return ((*(const uint64_t *) data) * 11400714819323198485llu) >> 24;
}

// cmap and khash template expansion
declare_cmap(ii, int64_t, int64_t, c_default_destroy, c_default_equals, fibonacci_hash);
KHASH_MAP_INIT_INT64(ii, int64_t)


size_t seed;
static const float max_load_factor = 0.77f;

crand_rng64_t rng;
#define SEED(s) rng = crand_rng64_init(seed)
#define RAND(N) (crand_i64(&rng) & ((1 << N) - 1))


#define CMAP_SETUP(tt, Key, Value) cmap_##tt map = cmap_ini \
                                    ; cmap_##tt##_set_load_factors(&map, max_load_factor, 0.0)
#define CMAP_PUT(tt, key, val)     cmap_##tt##_put(&map, key, val).item->value
#define CMAP_EMPLACE(tt, key, val) cmap_##tt##_emplace(&map, key, val)
#define CMAP_ERASE(tt, key)        cmap_##tt##_erase(&map, key)
#define CMAP_FIND(tt, key)         (cmap_##tt##_find(map, key) != NULL)
#define CMAP_FOR(tt, i)            c_foreach (i, cmap_##tt, map)
#define CMAP_ITEM(tt, i)           i.item->value
#define CMAP_SIZE(tt)              cmap_size(map)
#define CMAP_BUCKETS(tt)           cmap_bucket_count(map)
#define CMAP_CLEAR(tt)             cmap_##tt##_clear(&map)
#define CMAP_DTOR(tt)              cmap_##tt##_destroy(&map)

#define KMAP_SETUP(tt, Key, Value) khash_t(ii)* map = kh_init(ii); khiter_t ki; int ret
#define KMAP_PUT(tt, key, val)     (*(ki = kh_put(ii, map, key, &ret), map->vals[ki] = val, &map->vals[ki]))
#define KMAP_EMPLACE(tt, key, val)  (ki = kh_put(ii, map, key, &ret), ret ? (map->vals[ki] = val) : val)
#define KMAP_ERASE(tt, key)        ((ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0)
#define KMAP_FIND(tt, key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE(tt)              kh_size(map)
#define KMAP_BUCKETS(tt)           kh_n_buckets(map)
#define KMAP_CLEAR(tt)             kh_clear(ii, map)
#define KMAP_DTOR(tt)              kh_destroy(ii, map)

#define UMAP_SETUP(tt, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define UMAP_PUT(tt, key, val)     (map[key] = val)
#define UMAP_EMPLACE(tt, key, val) map.emplace(key, val)
#define UMAP_FIND(tt, key)         (map.find(key) != map.end())
#define UMAP_ERASE(tt, key)        map.erase(key)
#define UMAP_FOR(tt, i)            for (auto i: map)
#define UMAP_ITEM(tt, i)           i.second
#define UMAP_SIZE(tt)              map.size()
#define UMAP_BUCKETS(tt)           map.bucket_count()
#define UMAP_CLEAR(tt)             map.clear()
#define UMAP_DTOR(tt)              destroy_me(map)

#define BMAP_SETUP(tt, Key, Value) ska::bytell_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define BMAP_PUT(tt, key, val)     UMAP_PUT(tt, key, val)
#define BMAP_EMPLACE(tt, key, val) UMAP_EMPLACE(tt, key, val)
#define BMAP_FIND(tt, key)         UMAP_FIND(tt, key)
#define BMAP_ERASE(tt, key)        UMAP_ERASE(tt, key)
#define BMAP_FOR(tt, i)            UMAP_FOR(tt, i)
#define BMAP_ITEM(tt, i)           UMAP_ITEM(tt, i)
#define BMAP_SIZE(tt)              UMAP_SIZE(tt)
#define BMAP_BUCKETS(tt)           UMAP_BUCKETS(tt)
#define BMAP_CLEAR(tt)             UMAP_CLEAR(tt)
#define BMAP_DTOR(tt)              UMAP_DTOR(tt)

#define FMAP_SETUP(tt, Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define FMAP_PUT(tt, key, val)     UMAP_PUT(tt, key, val)
#define FMAP_EMPLACE(tt, key, val) UMAP_EMPLACE(tt, key, val)
#define FMAP_FIND(tt, key)         UMAP_FIND(tt, key)
#define FMAP_ERASE(tt, key)        UMAP_ERASE(tt, key)
#define FMAP_FOR(tt, i)            UMAP_FOR(tt, i)
#define FMAP_ITEM(tt, i)           UMAP_ITEM(tt, i)
#define FMAP_SIZE(tt)              UMAP_SIZE(tt)
#define FMAP_BUCKETS(tt)           UMAP_BUCKETS(tt)
#define FMAP_CLEAR(tt)             UMAP_CLEAR(tt)
#define FMAP_DTOR(tt)              UMAP_DTOR(tt)

#define HMAP_SETUP(tt, Key, Value) tsl::hopscotch_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define HMAP_PUT(tt, key, val)     UMAP_PUT(tt, key, val)
#define HMAP_EMPLACE(tt, key, val) UMAP_EMPLACE(tt, key, val)
#define HMAP_FIND(tt, key)         UMAP_FIND(tt, key)
#define HMAP_ERASE(tt, key)        UMAP_ERASE(tt, key)
#define HMAP_FOR(tt, i)            UMAP_FOR(tt, i)
#define HMAP_ITEM(tt, i)           UMAP_ITEM(tt, i)
#define HMAP_SIZE(tt)              UMAP_SIZE(tt)
#define HMAP_BUCKETS(tt)           UMAP_BUCKETS(tt)
#define HMAP_CLEAR(tt)             UMAP_CLEAR(tt)
#define HMAP_DTOR(tt)              UMAP_DTOR(tt)

#define RMAP_SETUP(tt, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(tt, key, val)     UMAP_PUT(tt, key, val)
#define RMAP_EMPLACE(tt, key, val) UMAP_EMPLACE(tt, key, val)
#define RMAP_FIND(tt, key)         UMAP_FIND(tt, key)
#define RMAP_ERASE(tt, key)        UMAP_ERASE(tt, key)
#define RMAP_FOR(tt, i)            UMAP_FOR(tt, i)
#define RMAP_ITEM(tt, i)           UMAP_ITEM(tt, i)
#define RMAP_SIZE(tt)              UMAP_SIZE(tt)
#define RMAP_BUCKETS(tt)           map.mask()
#define RMAP_CLEAR(tt)             UMAP_CLEAR(tt)
#define RMAP_DTOR(tt)              UMAP_DTOR(tt)

#define SMAP_SETUP(tt, Key, Value) spp::sparse_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define SMAP_PUT(tt, key, val)     UMAP_PUT(tt, key, val)
#define SMAP_EMPLACE(tt, key, val) UMAP_EMPLACE(tt, key, val)
#define SMAP_FIND(tt, key)         UMAP_FIND(tt, key)
#define SMAP_ERASE(tt, key)        UMAP_ERASE(tt, key)
#define SMAP_FOR(tt, i)            UMAP_FOR(tt, i)
#define SMAP_ITEM(tt, i)           UMAP_ITEM(tt, i)
#define SMAP_SIZE(tt)              UMAP_SIZE(tt)
#define SMAP_BUCKETS(tt)           UMAP_BUCKETS(tt)
#define SMAP_CLEAR(tt)             UMAP_CLEAR(tt)
#define SMAP_DTOR(tt)              UMAP_DTOR(tt)

enum {
    FAC = 3,
    N1 = 10000000 * FAC,
    N2 = 10000000 * FAC,
    N3 = 10000000 * FAC,
    N4 = 10000000 * FAC,
    RR =  24
};
int rr = RR;


#define MAP_TEST1(M, tt) \
{ \
    M##_SETUP(tt, int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++ M##_PUT(tt, RAND(rr), i); \
        erased += M##_ERASE(tt, RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, sum: %zu, erased %zu\n", \
           (size_t) M##_SIZE(tt), (size_t) M##_BUCKETS(tt), (float) difference / CLOCKS_PER_SEC, checksum, erased); \
    M##_CLEAR(tt); \
}

#define MAP_TEST2(M, tt) \
{ \
    M##_SETUP(tt, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N2; ++i) \
        M##_PUT(tt, i, i); \
    for (size_t i = 0; i < N2; ++i) \
        erased += M##_ERASE(tt, i); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(tt), (size_t) M##_BUCKETS(tt), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tt); \
}

#define MAP_TEST3(M, tt) \
{ \
    M##_SETUP(tt, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        M##_PUT(tt, RAND(rr), i); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        erased += M##_ERASE(tt, RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, erased %zu\n", \
           (size_t) M##_SIZE(tt), (size_t) M##_BUCKETS(tt), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tt); \
}

#define MAP_TEST4(M, tt) \
{ \
    M##_SETUP(tt, int64_t, int64_t); \
    size_t sum = 0; \
    SEED(seed); \
    for (size_t i = 0; i < N4; ++i) \
        M##_PUT(tt, RAND(rr), i); \
    clock_t difference, before = clock(); \
    for (int k=0; k<5; k++) M##_FOR (tt, i) \
        sum += M##_ITEM(tt, i); \
    difference = clock() - before; \
    printf(#M ": size: %zu, buckets: %8zu, time: %5.02f, sum %zu\n", \
           (size_t) M##_SIZE(tt), (size_t) M##_BUCKETS(tt), (float) difference / CLOCKS_PER_SEC, sum); \
    M##_CLEAR(tt); \
}


#ifndef __cplusplus
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii) /*MAP_TEST##n(KMAP, ii)*/
#else
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii) /*MAP_TEST##n(KMAP, ii)*/ MAP_TEST##n(UMAP, ii) MAP_TEST##n(SMAP, ii) \
                    MAP_TEST##n(BMAP, ii) MAP_TEST##n(FMAP, ii) /*MAP_TEST##n(RMAP, ii)*/ MAP_TEST##n(HMAP, ii)
#endif

int main(int argc, char* argv[])
{
    rr = argc == 2 ? atoi(argv[1]) : RR;
    seed = time(NULL);
    printf("\nRandom keys are in range [0, 2^%d), seed = %zu:\n",  rr, seed);
    printf("\nUnordered maps: %d repeats of Insert random key + try to remove a random key:\n", N1);
    RUN_TEST(1)

    printf("\nUnordered maps: Insert %d index keys, then remove them in same order:\n", N2);
    RUN_TEST(2)

    printf("\nUnordered maps: Insert %d random keys, then remove them in same order:\n", N3);
    RUN_TEST(3)

    printf("\nUnordered maps: Iterate %d random keys:\n", N4);
    RUN_TEST(4)
}
