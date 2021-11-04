#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include "others/khash.h"

#ifdef __cplusplus
#include <limits>
#include <unordered_map>
#include "others/robin_hood.hpp"
#include "others/skarupke/bytell_hash_map.hpp"
#include "others/tsl/hopscotch_map.h"
#include "others/parallel_hashmap/phmap.h"
template<typename C> inline void destroy_me(C& c) { C().swap(c); }
#endif


// cmap and khash template expansion
#define i_key int64_t
#define i_val int64_t
#define i_hash c_default_hash64
#define i_tag ii
#include <stc/cmap.h>

size_t seed;
static const float max_load_factor = 0.77f;

KHASH_MAP_INIT_INT64(ii, int64_t)
template <class K, class V> using robin_hood_flat_map = robin_hood::unordered_flat_map<
                                    K, V, robin_hood::hash<K>, std::equal_to<K>, 77>;

stc64_t rng;
#define SEED(s) rng = stc64_init(seed)
#define RAND(N) (stc64_rand(&rng) & ((1 << N) - 1))


#define CMAP_SETUP(X, Key, Value) cmap_##X map = cmap_##X##_init() \
                                  ; cmap_##X##_max_load_factor(&map, max_load_factor)
#define CMAP_PUT(X, key, val)     cmap_##X##_emplace_or_assign(&map, key, val).ref->second
#define CMAP_EMPLACE(X, key, val) cmap_##X##_emplace(&map, key, val).ref->second
#define CMAP_ERASE(X, key)        cmap_##X##_erase(&map, key)
#define CMAP_FIND(X, key)         cmap_##X##_contains(&map, key)
#define CMAP_FOR(X, i)            c_foreach (i, cmap_##X, map)
#define CMAP_ITEM(X, i)           i.ref->second
#define CMAP_SIZE(X)              cmap_##X##_size(map)
#define CMAP_BUCKETS(X)           cmap_##X##_bucket_count(map)
#define CMAP_CLEAR(X)             cmap_##X##_clear(&map)
#define CMAP_DTOR(X)              cmap_##X##_del(&map)

#define KMAP_SETUP(X, Key, Value) khash_t(ii)* map = kh_init(ii); khiter_t ki; int ret
#define KMAP_PUT(X, key, val)     (*(ki = kh_put(ii, map, key, &ret), map->vals[ki] = val, map->vals+ki))
#define KMAP_EMPLACE(X, key, val) (*(ki = kh_put(ii, map, key, &ret), ret ? (map->vals[ki] = val, 0) : 1, map->vals+ki))
#define KMAP_ERASE(X, key)        ((ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0)
#define KMAP_FIND(X, key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE(X)              kh_size(map)
#define KMAP_BUCKETS(X)           kh_n_buckets(map)
#define KMAP_CLEAR(X)             kh_clear(ii, map)
#define KMAP_DTOR(X)              kh_destroy(ii, map)

#define UMAP_SETUP(X, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define UMAP_PUT(X, key, val)     (map[key] = val)
#define UMAP_EMPLACE(X, key, val) map.emplace(key, val).first->second
#define UMAP_FIND(X, key)         int(map.find(key) != map.end())
#define UMAP_ERASE(X, key)        map.erase(key)
#define UMAP_FOR(X, i)            for (auto i: map)
#define UMAP_ITEM(X, i)           i.second
#define UMAP_SIZE(X)              map.size()
#define UMAP_BUCKETS(X)           map.bucket_count()
#define UMAP_CLEAR(X)             map.clear()
#define UMAP_DTOR(X)              ((void)0) // destroy_me(map)

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

//#define RMAP_SETUP(X, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_SETUP(X, Key, Value) robin_hood_flat_map<Key, Value> map
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

#define PMAP_SETUP(X, Key, Value) phmap::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor)
#define PMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define PMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define PMAP_FIND(X, key)         UMAP_FIND(X, key)
#define PMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define PMAP_FOR(X, i)            UMAP_FOR(X, i)
#define PMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define PMAP_SIZE(X)              UMAP_SIZE(X)
#define PMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define PMAP_CLEAR(X)             UMAP_CLEAR(X)
#define PMAP_DTOR(X)              UMAP_DTOR(X)


#define MAP_TEST0(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t checksum = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) { \
        checksum += ++ M##_EMPLACE(X, RAND(rr), i); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum: %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, checksum, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}

#define MAP_TEST1(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t checksum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) { \
        checksum += ++ M##_EMPLACE(X, RAND(rr), i); \
        erased += M##_ERASE(X, RAND(rr)); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum: %zu, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, checksum, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}

#define MAP_TEST2(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, i, i); \
    for (size_t i = 0; i < n; ++i) \
        erased += M##_ERASE(X, i); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}

#define MAP_TEST3(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before; \
    SEED(seed); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, RAND(rr), i); \
    SEED(seed); \
    before = clock(); \
    for (size_t i = 0; i < n; ++i) \
        erased += M##_ERASE(X, RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, erased %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, erased, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}

#define MAP_TEST4(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t sum = 0; \
    SEED(seed); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, RAND(rr), i); \
    clock_t difference, before = clock(); \
    for (int k=0; k<5; k++) M##_FOR (X, i) \
        sum += M##_ITEM(X, i); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, sum %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, sum, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}

#define MAP_TEST5(M, X, n) \
{ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t found = 0; \
    clock_t difference, before; \
    SEED(seed); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, RAND(rr), i); \
    before = clock(); \
    for (size_t i = 0; i < n/2; ++i) \
        found += M##_FIND(X, RAND(rr)); \
    SEED(seed); \
    for (size_t i = 0; i < n/2; ++i) \
        found += M##_FIND(X, RAND(rr)); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, found %zu, size: %zu, buckets: %8zu\n", \
           (float) difference / CLOCKS_PER_SEC, found, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X)); \
    M##_DTOR(X); \
}


#ifdef __cplusplus
#define RUN_TEST(n) MAP_TEST##n(KMAP, ii, N##n) MAP_TEST##n(CMAP, ii, N##n) MAP_TEST##n(PMAP, ii, N##n) \
                    MAP_TEST##n(FMAP, ii, N##n) MAP_TEST##n(RMAP, ii, N##n) MAP_TEST##n(UMAP, ii, N##n) 
#define ITR_TEST(n) MAP_TEST##n(CMAP, ii, N##n)                             MAP_TEST##n(PMAP, ii, N##n) \
                    MAP_TEST##n(FMAP, ii, N##n) MAP_TEST##n(RMAP, ii, N##n) MAP_TEST##n(UMAP, ii, N##n)
#else
#define RUN_TEST(n) MAP_TEST##n(KMAP, ii, N##n) MAP_TEST##n(CMAP, ii, N##n)
#define ITR_TEST(n) MAP_TEST##n(CMAP, ii, N##n)
#endif

enum {
    RR = 27,
    NN = 10,
    NF = 1000000,
    F0 = 4,
    F1 = 2,
    F2 = 4,
    F3 = 6,
    F4 = 8,
    F5 = 6,
};


int main(int argc, char* argv[])
{
    int nn = argc >= 2 ? atoi(argv[1]) : NN;
    int rr = argc >= 3 ? atoi(argv[2]) : RR;
    int n = NF * nn;
    int N0 = n*F0, N1 = n*F1, N2 = n*F2, N3 = n*F3, N4 = n*F4, N5 = n*F5;
    seed = time(NULL);

    printf("\nUnordered hash map shootout\nUsage %s [n-base=%d key-bits=%d]\n\n", argv[0], NN, RR);
    printf("CMAP = https://github.com/tylov/STC\n"
           "KMAP = https://github.com/attractivechaos/klib\n"
           "PMAP = https://github.com/greg7mdp/parallel-hashmap\n"
           "FMAP = https://github.com/skarupke/flat_hash_map\n"
           "RMAP = https://github.com/martinus/robin-hood-hashing\n"
           "UMAP = std::unordered_map\n");
           
    printf("\nN-base = %d. Random keys are in range [0, 2^%d). Seed = %zu:\n", nn, rr, seed);
    printf("\nN=%d. Insert random keys:\n", N0);
    RUN_TEST(0)

    printf("\nN=%d. Insert random key + try to remove another random key:\n", N1);
    RUN_TEST(1)

    printf("\nN=%d. Insert sequential keys, then remove them in same order:\n", N2);
    RUN_TEST(2)

    printf("\nN=%d. Remove random keys:\n", N3);
    RUN_TEST(3)

    printf("\nN=%d. Iterate random keys:\n", N4);
    ITR_TEST(4)

    printf("\nN=%d. Lookup random keys:\n", N5);
    RUN_TEST(5)
}
