#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include "others/khash.h"

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
    return (uint32_t) (((*(const uint64_t *) data) * 11400714819323198485llu) >> 24);
}

// cmap and khash template expansion
using_cmap(ii, int64_t, int64_t, c_default_equals, fibonacci_hash); // c_default_hash);
KHASH_MAP_INIT_INT64(ii, int64_t)


size_t seed;
static const float max_load_factor = 0.77f;

stc64_t rng;
#define SEED(s) rng = stc64_init(seed)
#define RAND(N) (stc64_rand(&rng) & ((1 << N) - 1))


#define CMAP_SETUP(X, Key, Value) cmap_##X map = cmap_inits \
                                  ; cmap_##X##_set_load_factors(&map, 0.0, max_load_factor)
#define CMAP_PUT(X, key, val)     cmap_##X##_put(&map, key, val).first->second
#define CMAP_EMPLACE(X, key, val) cmap_##X##_emplace(&map, key, val).first->second
#define CMAP_ERASE(X, key)        cmap_##X##_erase(&map, key)
#define CMAP_FIND(X, key)         (cmap_##X##_find(map, key) != NULL)
#define CMAP_FOR(X, i)            c_foreach (i, cmap_##X, map)
#define CMAP_ITEM(X, i)           i.ref->second
#define CMAP_SIZE(X)              cmap_##X##_size(map)
#define CMAP_BUCKETS(X)           cmap_##X##_bucket_count(map)
#define CMAP_CLEAR(X)             cmap_##X##_clear(&map)
#define CMAP_DTOR(X)              cmap_##X##_del(&map)

#define KMAP_SETUP(X, Key, Value) khash_t(ii)* map = kh_init(ii); khiter_t ki; int ret
#define KMAP_PUT(X, key, val)     (*(ki = kh_put(ii, map, key, &ret), map->vals[ki] = val, &map->vals[ki]))
#define KMAP_EMPLACE(X, key, val) (ki = kh_put(ii, map, key, &ret), ret ? (map->vals[ki] = val, 0) : 0, map->vals[ki])
#define KMAP_ERASE(X, key)        ((ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0)
#define KMAP_FIND(X, key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE(X)              kh_size(map)
#define KMAP_BUCKETS(X)           kh_n_buckets(map)
#define KMAP_CLEAR(X)             kh_clear(ii, map)
#define KMAP_DTOR(X)              kh_destroy(ii, map)

#define UMAP_SETUP(X, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define UMAP_PUT(X, key, val)     (map[key] = val)
#define UMAP_EMPLACE(X, key, val) (*map.emplace(key, val).first).second
#define UMAP_FIND(X, key)         (map.find(key) != map.end())
#define UMAP_ERASE(X, key)        map.erase(key)
#define UMAP_FOR(X, i)            for (auto i: map)
#define UMAP_ITEM(X, i)           i.second
#define UMAP_SIZE(X)              map.size()
#define UMAP_BUCKETS(X)           map.bucket_count()
#define UMAP_CLEAR(X)             map.clear()
#define UMAP_DTOR(X)              destroy_me(map)

#define BMAP_SETUP(X, Key, Value) ska::bytell_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define BMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define BMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define BMAP_FIND(X, key)         UMAP_FIND(X, key)
#define BMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define BMAP_FOR(X, i)            UMAP_FOR(X, i)
#define BMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define BMAP_SIZE(X)              UMAP_SIZE(X)
#define BMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define BMAP_CLEAR(X)             UMAP_CLEAR(X)
#define BMAP_DTOR(X)              UMAP_DTOR(X)

#define FMAP_SETUP(X, Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define FMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define FMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define FMAP_FIND(X, key)         UMAP_FIND(X, key)
#define FMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define FMAP_FOR(X, i)            UMAP_FOR(X, i)
#define FMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define FMAP_SIZE(X)              UMAP_SIZE(X)
#define FMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define FMAP_CLEAR(X)             UMAP_CLEAR(X)
#define FMAP_DTOR(X)              UMAP_DTOR(X)

#define HMAP_SETUP(X, Key, Value) tsl::hopscotch_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define HMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define HMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define HMAP_FIND(X, key)         UMAP_FIND(X, key)
#define HMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define HMAP_FOR(X, i)            UMAP_FOR(X, i)
#define HMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define HMAP_SIZE(X)              UMAP_SIZE(X)
#define HMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define HMAP_CLEAR(X)             UMAP_CLEAR(X)
#define HMAP_DTOR(X)              UMAP_DTOR(X)

#define RMAP_SETUP(X, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define RMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define RMAP_FIND(X, key)         UMAP_FIND(X, key)
#define RMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define RMAP_FOR(X, i)            UMAP_FOR(X, i)
#define RMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define RMAP_SIZE(X)              UMAP_SIZE(X)
#define RMAP_BUCKETS(X)           map.mask()
#define RMAP_CLEAR(X)             UMAP_CLEAR(X)
#define RMAP_DTOR(X)              UMAP_DTOR(X)

#define SMAP_SETUP(X, Key, Value) spp::sparse_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define SMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define SMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define SMAP_FIND(X, key)         UMAP_FIND(X, key)
#define SMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define SMAP_FOR(X, i)            UMAP_FOR(X, i)
#define SMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define SMAP_SIZE(X)              UMAP_SIZE(X)
#define SMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define SMAP_CLEAR(X)             UMAP_CLEAR(X)
#define SMAP_DTOR(X)              UMAP_DTOR(X)

enum {
    FAC = 3,
    N1 = 10000000 * FAC,
    N2 = 10000000 * FAC,
    N3 = 10000000 * FAC,
    N4 = 10000000 * FAC,
    RR =  24
};
int rr = RR;


#define MAP_TEST1(M, X) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++ M##_EMPLACE(X, RAND(rr), i); \
        erased += M##_ERASE(X, RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum: %zu, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, checksum, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_CLEAR(X); \
}

#define MAP_TEST2(M, X) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N2; ++i) \
        M##_PUT(X, i, i); \
    for (size_t i = 0; i < N2; ++i) \
        erased += M##_ERASE(X, i); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_CLEAR(X); \
}

#define MAP_TEST3(M, X) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        M##_PUT(X, RAND(rr), i); \
    SEED(seed); \
    for (size_t i = 0; i < N3; ++i) \
        erased += M##_ERASE(X, RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_CLEAR(X); \
}

#define MAP_TEST4(M, X) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t sum = 0; \
    SEED(seed); \
    for (size_t i = 0; i < N4; ++i) \
        M##_PUT(X, RAND(rr), i); \
    clock_t difference, before = clock(); \
    for (int k=0; k<5; k++) M##_FOR (X, i) \
        sum += M##_ITEM(X, i); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, sum, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_CLEAR(X); \
}

#define MAP_TEST5(M, X) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t checksum = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N1; ++i) { \
        checksum += ++ M##_EMPLACE(X, RAND(rr), i); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum: %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, checksum, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_CLEAR(X); \
}

#ifdef __cplusplus
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii) MAP_TEST##n(KMAP, ii) MAP_TEST##n(UMAP, ii) /*MAP_TEST##n(SMAP, ii)*/ \
                    MAP_TEST##n(BMAP, ii) MAP_TEST##n(FMAP, ii) MAP_TEST##n(RMAP, ii) /*MAP_TEST##n(HMAP, ii)*/
#define RUNX_TEST(n) MAP_TEST##n(CMAP, ii) /*MAP_TEST##n(KMAP, ii)*/ MAP_TEST##n(UMAP, ii) MAP_TEST##n(SMAP, ii) \
                    MAP_TEST##n(BMAP, ii) MAP_TEST##n(FMAP, ii) /*MAP_TEST##n(RMAP, ii)*/ /*MAP_TEST##n(HMAP, ii)*/
#else
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii) MAP_TEST##n(KMAP, ii)
#define RUNX_TEST(n) MAP_TEST##n(CMAP, ii)
#endif


int main(int argc, char* argv[])
{
    rr = argc == 2 ? atoi(argv[1]) : RR;
    seed = time(NULL);

    printf("\nUnordered maps: Insert %d random keys:\n", N3);
    RUN_TEST(5)

    printf("\nRandom keys are in range [0, 2^%d), seed = %zu:\n",  rr, seed);
    printf("\nUnordered maps: %d repeats of Insert random key + try to remove a random key:\n", N1);
    RUN_TEST(1)

    printf("\nUnordered maps: Insert %d index keys, then remove them in same order:\n", N2);
    RUN_TEST(2)

    printf("\nUnordered maps: Insert %d random keys, then remove them in same order:\n", N3);
    RUN_TEST(3)

    printf("\nUnordered maps: Iterate %d random keys:\n", N4);
    RUNX_TEST(4)
}
