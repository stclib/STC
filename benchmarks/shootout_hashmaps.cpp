#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include "external/khash.h"

enum {max_load_factor = 77};

#ifdef __cplusplus
#include <limits>
#include <unordered_map>
#include "external/robin_hood.h"
#include "external/skarupke/bytell_hash_map.hpp"
#include "external/parallel_hashmap/phmap.h"
#include "external/tsl/hopscotch_map.h"
#include "external/tsl/robin_map.h"

template<typename C> inline void std_destroy(C& c) { C().swap(c); }

template <class K, class V> using robin_hood_flat_map = robin_hood::unordered_flat_map<
                                  K, V, robin_hood::hash<K>, std::equal_to<K>, max_load_factor>;
#endif

KHASH_MAP_INIT_INT64(ii, int64_t)

// cmap and khash template expansion
#define i_key int64_t
#define i_val int64_t
#define i_hash c_hash64
#define i_tag ii
#include <stc/cmap.h>

stc64_t rng;
size_t seed;
#define SEED(s) rng = stc64_new(seed)
#define RAND(N) (stc64_rand(&rng) & (((uint64_t)1 << N) - 1))


#define CMAP_SETUP(X, Key, Value) cmap_##X map = cmap_##X##_init() \
                                  ; cmap_##X##_max_load_factor(&map, max_load_factor/100.0f)
#define CMAP_PUT(X, key, val)     cmap_##X##_insert_or_assign(&map, key, val).ref->second
#define CMAP_EMPLACE(X, key, val) cmap_##X##_insert(&map, key, val).ref->second
#define CMAP_ERASE(X, key)        cmap_##X##_erase(&map, key)
#define CMAP_FIND(X, key)         cmap_##X##_contains(&map, key)
#define CMAP_FOR(X, i)            c_foreach (i, cmap_##X, map)
#define CMAP_ITEM(X, i)           i.ref->second
#define CMAP_SIZE(X)              cmap_##X##_size(map)
#define CMAP_BUCKETS(X)           cmap_##X##_bucket_count(map)
#define CMAP_CLEAR(X)             cmap_##X##_clear(&map)
#define CMAP_DTOR(X)              cmap_##X##_drop(&map)

#define KMAP_SETUP(X, Key, Value) khash_t(X)* map = kh_init(X); khiter_t ki; int ret
#define KMAP_PUT(X, key, val)     (*(ki = kh_put(X, map, key, &ret), map->vals[ki] = val, map->vals+ki))
#define KMAP_EMPLACE(X, key, val) (*(ki = kh_put(X, map, key, &ret), ret ? (map->vals[ki] = val, 0) : 1, map->vals+ki))
#define KMAP_ERASE(X, key)        ((ki = kh_get(X, map, key)) != kh_end(map) ? kh_del(X, map, ki), 1 : 0)
#define KMAP_FOR(X, i)            for (khint_t i = kh_begin(map); i != kh_end(map); ++i) if (kh_exist(map, i))
#define KMAP_ITEM(X, i)           map->vals[i]
#define KMAP_FIND(X, key)         (kh_get(X, map, key) != kh_end(map))
#define KMAP_SIZE(X)              kh_size(map)
#define KMAP_BUCKETS(X)           kh_n_buckets(map)
#define KMAP_CLEAR(X)             kh_clear(X, map)
#define KMAP_DTOR(X)              kh_destroy(X, map)

#define UMAP_SETUP(X, Key, Value) std::unordered_map<Key, Value> map; map.max_load_factor(max_load_factor/100.0f)
#define UMAP_PUT(X, key, val)     (map[key] = val)
#define UMAP_EMPLACE(X, key, val) map.emplace(key, val).first->second
#define UMAP_FIND(X, key)         int(map.find(key) != map.end())
#define UMAP_ERASE(X, key)        map.erase(key)
#define UMAP_FOR(X, i)            for (auto i: map)
#define UMAP_ITEM(X, i)           i.second
#define UMAP_SIZE(X)              map.size()
#define UMAP_BUCKETS(X)           map.bucket_count()
#define UMAP_CLEAR(X)             map.clear()
#define UMAP_DTOR(X)              std_destroy(map)

#define FMAP_SETUP(X, Key, Value) ska::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor/100.0f)
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

#define HMAP_SETUP(X, Key, Value) tsl::hopscotch_map<Key, Value> map; map.max_load_factor(max_load_factor/100.0f)
#define HMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define HMAP_EMPLACE(X, key, val) map.emplace(key, val).first.value()
#define HMAP_FIND(X, key)         UMAP_FIND(X, key)
#define HMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define HMAP_FOR(X, i)            UMAP_FOR(X, i)
#define HMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define HMAP_SIZE(X)              UMAP_SIZE(X)
#define HMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define HMAP_CLEAR(X)             UMAP_CLEAR(X)
#define HMAP_DTOR(X)              UMAP_DTOR(X)

#define TMAP_SETUP(X, Key, Value) tsl::robin_map<Key, Value> map; map.max_load_factor(max_load_factor/100.0f)
#define TMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define TMAP_EMPLACE(X, key, val) map.emplace(key, val).first.value()
#define TMAP_FIND(X, key)         UMAP_FIND(X, key)
#define TMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define TMAP_FOR(X, i)            UMAP_FOR(X, i)
#define TMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define TMAP_SIZE(X)              UMAP_SIZE(X)
#define TMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define TMAP_CLEAR(X)             UMAP_CLEAR(X)
#define TMAP_DTOR(X)              UMAP_DTOR(X)

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

#define PMAP_SETUP(X, Key, Value) phmap::flat_hash_map<Key, Value> map; map.max_load_factor(max_load_factor/100.0f)
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
{   /* Insert, update */ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t sum = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) { \
        sum += ++ M##_EMPLACE(X, RAND(keybits), i); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, sum: %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), sum); \
    M##_DTOR(X); \
}

#define MAP_TEST1(M, X, n) \
{   /* Insert, update and erase another */ \
    M##_SETUP(X, int64_t, int64_t); \
    uint64_t sum = 0, erased = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) { \
        sum += ++ M##_EMPLACE(X, RAND(keybits), i); \
        erased += M##_ERASE(X, RAND(keybits)); \
    } \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, erased %" PRIuMAX ", sum: %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), erased, sum); \
    M##_DTOR(X); \
}

#define MAP_TEST2(M, X, n) \
{   /* Insert sequential keys, then erase them */ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, i, i); \
    for (size_t i = 0; i < n; ++i) \
        erased += M##_ERASE(X, i); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, erased %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), erased); \
    M##_DTOR(X); \
}

#define MAP_TEST3(M, X, n) \
{   /* Erase elements */ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t erased = 0; \
    clock_t difference, before; \
    SEED(seed); \
    for (size_t i = 0; i < n; ++i) \
        M##_PUT(X, RAND(keybits), i); \
    SEED(seed); \
    before = clock(); \
    for (size_t i = 0; i < n; ++i) \
        erased += M##_ERASE(X, RAND(keybits)); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, erased %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), erased); \
    M##_DTOR(X); \
}

#define MAP_TEST4(M, X, n) \
{   /* Iterate */ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t sum = 0, m = 1ull << (keybits + 1), nn = n; \
    if (nn < m) m = nn; \
    SEED(seed); \
    for (size_t i = 0; i < m; ++i) \
        M##_PUT(X, RAND(keybits), i); \
    size_t x = 300000000/M##_SIZE(X); \
    clock_t difference, before = clock(); \
    for (size_t k=0; k < x; k++) M##_FOR (X, it) \
        sum += M##_ITEM(X, it); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, repeats: %" PRIuMAX ", sum: %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), x, sum); \
    M##_DTOR(X); \
}

#define MAP_TEST5(M, X, n) \
{   /* Lookup */ \
    M##_SETUP(X, int64_t, int64_t); \
    size_t found = 0, m = 1ull << (keybits + 1), nn = n; \
    clock_t difference, before; \
    if (nn < m) m = nn; \
    SEED(seed); \
    for (size_t i = 0; i < m; ++i) \
        M##_PUT(X, RAND(keybits), i); \
    before = clock(); \
    size_t x = m * 10000000/M##_SIZE(X); \
    for (size_t i = 0; i < x; ++i) \
        found += M##_FIND(X, RAND(keybits)); \
    SEED(seed); \
    for (size_t i = 0; i < x; ++i) \
        found += M##_FIND(X, RAND(keybits)); \
    difference = clock() - before; \
    printf(#M ": time: %5.02f, size: %" PRIuMAX ", buckets: %8zu, lookups: %" PRIuMAX ", found: %" PRIuMAX "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), x*2, found); \
    M##_DTOR(X); \
}


#ifdef __cplusplus
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii, N##n) MAP_TEST##n(KMAP, ii, N##n) \
                    MAP_TEST##n(PMAP, ii, N##n) MAP_TEST##n(FMAP, ii, N##n) \
                    MAP_TEST##n(RMAP, ii, N##n) MAP_TEST##n(HMAP, ii, N##n) \
                    MAP_TEST##n(TMAP, ii, N##n) MAP_TEST##n(UMAP, ii, N##n)
#else
#define RUN_TEST(n) MAP_TEST##n(CMAP, ii, N##n) MAP_TEST##n(KMAP, ii, N##n)
#endif

enum {
    DEFAULT_N_MILL = 40,
    DEFAULT_KEYBITS = 25,
};

int main(int argc, char* argv[])
{
    unsigned n_mill = argc >= 2 ? atoi(argv[1]) : DEFAULT_N_MILL;
    unsigned keybits = argc >= 3 ? atoi(argv[2]) : DEFAULT_KEYBITS;
    unsigned n = n_mill * 1000000;
    unsigned N0 = n, N1 = n/2, N2 = n/2, N3 = n, N4 = n, N5 = n;
    seed = time(NULL); // 1636306010;

    printf("\nUnordered hash map shootout\n");
    printf("CMAP = https://github.com/tylov/STC\n"
           "KMAP = https://github.com/attractivechaos/klib\n"
           "PMAP = https://github.com/greg7mdp/parallel-hashmap\n"
           "FMAP = https://github.com/skarupke/flat_hash_map\n"
           "RMAP = https://github.com/martinus/robin-hood-hashing\n"
           "HMAP = https://github.com/Tessil/hopscotch-map\n"
           "TMAP = https://github.com/Tessil/robin-map\n"
           "UMAP = std::unordered_map\n\n");
           
    printf("Usage %s [n-million=%d key-bits=%d]\n", argv[0], DEFAULT_N_MILL, DEFAULT_KEYBITS);
    printf("N-base = %d. Random keys are in range [0, 2^%d). Seed = %" PRIuMAX ":\n", n_mill, keybits, seed);

    printf("\nT0: Insert/update random keys:\n");
    RUN_TEST(0)

    printf("\nT1: Insert/update random key + try to remove another random key:\n");
    RUN_TEST(1)

    printf("\nT2: Insert sequential keys, then remove them in same order:\n");
    RUN_TEST(2)

    printf("\nT3: Remove random keys:\n");
    RUN_TEST(3)

    printf("\nT4: Iterate random keys:\n");
    RUN_TEST(4)

    printf("\nT5: Lookup random keys:\n");
    RUN_TEST(5)
}
