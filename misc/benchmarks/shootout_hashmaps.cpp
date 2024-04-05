#include <stdio.h>
#include <time.h>
#include "stc/crand.h"

#define MAX_LOAD_FACTOR 80

#ifdef __cplusplus
#include <limits>
#include <unordered_map>
#include "external/ankerl/robin_hood.h"
#include "external/ankerl/unordered_dense.h"
#include "external/skarupke/flat_hash_map.hpp"
#include "external/tsl/robin_map.h"
#include "external/emhash/hash_table7.hpp"
#ifdef HAVE_BOOST
#include <boost/unordered/unordered_flat_map.hpp>
#endif
//#include "external/skarupke/bytell_hash_map.hpp"
//#include "external/parallel_hashmap/phmap.h"
//#include "external/tsl/hopscotch_map.h"

template<typename C> inline void std_destroy(C& c) { C().swap(c); }

template <class K, class V> using robin_hood_flat_map = robin_hood::unordered_flat_map<
                                  K, V, robin_hood::hash<K>, std::equal_to<K>, MAX_LOAD_FACTOR>;
#endif

typedef int64_t IKey;
typedef int64_t IValue;

// khash template expansion
#include "external/khash.h"
KHASH_MAP_INIT_INT64(ii, IValue)

// STC hmap template definition
#define i_TYPE hmap_ii,IKey,IValue
#define i_max_load_factor MAX_LOAD_FACTOR / 100.0f
//#include "stc/hmap-robin.h"
#include "stc/hmap.h"

#define SEED(s) rng = crand_init(s)
#define RAND(N) (crand_u64(&rng) & (((uint64_t)1 << N) - 1))

#define HMAP_SETUP(X, Key, Value) hmap_##X map = hmap_##X##_init()
#define HMAP_PUT(X, key, val)     hmap_##X##_insert_or_assign(&map, key, val).ref->second
#define HMAP_EMPLACE(X, key, val) hmap_##X##_insert(&map, key, val).ref->second
#define HMAP_ERASE(X, key)        hmap_##X##_erase(&map, key)
#define HMAP_FIND(X, key)         hmap_##X##_contains(&map, key)
#define HMAP_FOR(X, i)            c_foreach (i, hmap_##X, map)
#define HMAP_ITEM(X, i)           i.ref->second
#define HMAP_SIZE(X)              hmap_##X##_size(&map)
#define HMAP_BUCKETS(X)           hmap_##X##_bucket_count(&map)
#define HMAP_CLEAR(X)             hmap_##X##_clear(&map)
#define HMAP_DTOR(X)              hmap_##X##_drop(&map)

#define KMAP_SETUP(X, Key, Value) khash_t(X)* map = kh_init(X); khiter_t ki; int ret
#define KMAP_PUT(X, key, val)     (ki = kh_put(X, map, key, &ret), \
                                   map->vals[ki] = val, map->vals[ki])
#define KMAP_EMPLACE(X, key, val) (ki = kh_put(X, map, key, &ret), \
                                   (ret ? map->vals[ki] = val, 1 : 0), map->vals[ki])
#define KMAP_ERASE(X, key)        (ki = kh_get(X, map, key), \
                                   ki != kh_end(map) ? (kh_del(X, map, ki), 1) : 0)
#define KMAP_FOR(X, i)            for (khint_t i = kh_begin(map); i != kh_end(map); ++i) \
                                      if (kh_exist(map, i))
#define KMAP_ITEM(X, i)           map->vals[i]
#define KMAP_FIND(X, key)         (kh_get(X, map, key) != kh_end(map))
#define KMAP_SIZE(X)              kh_size(map)
#define KMAP_BUCKETS(X)           kh_n_buckets(map)
#define KMAP_CLEAR(X)             kh_clear(X, map)
#define KMAP_DTOR(X)              kh_destroy(X, map)

#define UMAP_SETUP(X, Key, Value) std::unordered_map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
#define UMAP_PUT(X, key, val)     (map[key] = val)
#define UMAP_EMPLACE(X, key, val) map.emplace(key, val).first->second
#define UMAP_FIND(X, key)         int(map.find(key) != map.end())
#define UMAP_ERASE(X, key)        map.erase(key)
#define UMAP_FOR(X, i)            for (const auto& i: map)
#define UMAP_ITEM(X, i)           i.second
#define UMAP_SIZE(X)              map.size()
#define UMAP_BUCKETS(X)           map.bucket_count()
#define UMAP_CLEAR(X)             map.clear()
#define UMAP_DTOR(X)              std_destroy(map)

#define FMAP_SETUP(X, Key, Value) ska::flat_hash_map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
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

#define BMAP_SETUP(X, Key, Value) boost::unordered_flat_map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
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

#define TMAP_SETUP(X, Key, Value) tsl::robin_map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
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

#define RMAP_SETUP(X, Key, Value) robin_hood_flat_map<Key, Value> map
#define RMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define RMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define RMAP_FIND(X, key)         UMAP_FIND(X, key)
#define RMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define RMAP_FOR(X, i)            UMAP_FOR(X, i)
#define RMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define RMAP_SIZE(X)              UMAP_SIZE(X)
#define RMAP_BUCKETS(X)           (map.mask() + 1)
#define RMAP_CLEAR(X)             UMAP_CLEAR(X)
#define RMAP_DTOR(X)              UMAP_DTOR(X)

#define DMAP_SETUP(X, Key, Value) ankerl::unordered_dense::map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
#define DMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define DMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define DMAP_FIND(X, key)         UMAP_FIND(X, key)
#define DMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define DMAP_FOR(X, i)            UMAP_FOR(X, i)
#define DMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define DMAP_SIZE(X)              UMAP_SIZE(X)
#define DMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define DMAP_CLEAR(X)             UMAP_CLEAR(X)
#define DMAP_DTOR(X)              UMAP_DTOR(X)

#define EMAP_SETUP(X, Key, Value) emhash7::HashMap<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
#define EMAP_PUT(X, key, val)     UMAP_PUT(X, key, val)
#define EMAP_EMPLACE(X, key, val) UMAP_EMPLACE(X, key, val)
#define EMAP_FIND(X, key)         UMAP_FIND(X, key)
#define EMAP_ERASE(X, key)        UMAP_ERASE(X, key)
#define EMAP_FOR(X, i)            UMAP_FOR(X, i)
#define EMAP_ITEM(X, i)           UMAP_ITEM(X, i)
#define EMAP_SIZE(X)              UMAP_SIZE(X)
#define EMAP_BUCKETS(X)           UMAP_BUCKETS(X)
#define EMAP_CLEAR(X)             UMAP_CLEAR(X)
#define EMAP_DTOR(X)              UMAP_DTOR(X)

#define PMAP_SETUP(X, Key, Value) phmap::flat_hash_map<Key, Value> map; \
                                  map.max_load_factor(MAX_LOAD_FACTOR/100.0f)
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


#define MAP_TEST1(M, X, n) \
{   /* Insert, update */ \
    M##_SETUP(X, IKey, IValue); \
    uint64_t sum = 0; \
    SEED(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) { \
        sum += M##_PUT(X, RAND(keybits), i); \
    } \
    difference = clock() - before; \
    printf(#M ": %5.03f s, size: %" c_ZU ", buckets: %8" c_ZU ", sum: %" c_ZU "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), sum); \
    M##_DTOR(X); \
}

#define MAP_TEST2(M, X, n) \
{   /* Insert sequential keys, then erase them */ \
    M##_SETUP(X, IKey, IValue); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < n; ++i) \
        M##_EMPLACE(X, i, i); \
    for (size_t i = 0; i < n; ++i) \
        erased += M##_ERASE(X, i); \
    difference = clock() - before; \
    printf(#M ": %5.03f s, size: %" c_ZU ", buckets: %8" c_ZU ", erased %" c_ZU "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), erased); \
    M##_DTOR(X); \
}

#define MAP_TEST3(M, X, n) \
{   /* Erase elements */ \
    M##_SETUP(X, IKey, IValue); \
    size_t erased = 0, _n = (n)*2; \
    clock_t difference, before; \
    SEED(seed); \
    for (size_t i = 0; i < _n; ++i) \
        M##_EMPLACE(X, RAND(keybits), i); \
    SEED(seed); \
    before = clock(); \
    for (size_t i = 0; i < _n; ++i) \
        erased += M##_ERASE(X, RAND(keybits)); \
    difference = clock() - before; \
    printf(#M ": %5.03f s, size: %" c_ZU ", buckets: %8" c_ZU ", erased %" c_ZU "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), erased); \
    M##_DTOR(X); \
}

#define MAP_TEST4(M, X, n) \
{   /* Iterate */ \
    M##_SETUP(X, IKey, IValue); \
    size_t sum = 0, m = 1ull << (keybits + 1), _n = n; \
    if (_n < m) m = _n; \
    SEED(seed); \
    for (size_t i = 0; i < m; ++i) \
        M##_EMPLACE(X, RAND(keybits), i); \
    size_t rep = 60000000ull/M##_SIZE(X); \
    clock_t difference, before = clock(); \
    for (size_t k=0; k < rep; k++) M##_FOR (X, it) \
        sum += M##_ITEM(X, it); \
    difference = clock() - before; \
    printf(#M ": %5.03f s, size: %" c_ZU ", buckets: %8" c_ZU ", repeats: %" c_ZU ", check: %" c_ZU "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), (size_t) M##_BUCKETS(X), rep, sum & 0xffff); \
    M##_DTOR(X); \
}

#define MAP_TEST5(M, X, n) \
{   /* Lookup */ \
    M##_SETUP(X, IKey, IValue); \
    size_t found = 0, m = 1ull << (keybits + 1), _n = n; \
    clock_t difference, before; \
    if (_n < m) m = _n; \
    SEED(seed); \
    for (size_t i = 0; i < m; ++i) \
        M##_EMPLACE(X, RAND(keybits), i); \
    before = clock(); \
    /* Lookup x random keys */ \
    size_t x = m * 8000000ull/M##_SIZE(X); \
    for (size_t i = 0; i < x; ++i) \
        found += M##_FIND(X, RAND(keybits)); \
    /* Lookup x existing keys by resetting seed */ \
    SEED(seed); \
    for (size_t i = 0; i < x; ++i) \
        found += M##_FIND(X, RAND(keybits)); \
    difference = clock() - before; \
    printf(#M ": %5.03f s, size: %" c_ZU ", lookups: %" c_ZU ", found: %" c_ZU "\n", \
           (float) difference / CLOCKS_PER_SEC, (size_t) M##_SIZE(X), x*2, found); \
    M##_DTOR(X); \
}


#ifdef __cplusplus
#ifdef HAVE_BOOST
#define MAP_TEST_BOOST(n, X) MAP_TEST##n(BMAP, X, N##n)
#else
#define MAP_TEST_BOOST(n, X)
#endif
#define RUN_TEST(n) MAP_TEST##n(KMAP, ii, N##n) \
                    MAP_TEST_BOOST(n, ii) \
                    MAP_TEST##n(HMAP, ii, N##n) \
                    MAP_TEST##n(FMAP, ii, N##n) \
                    MAP_TEST##n(TMAP, ii, N##n) \
                    MAP_TEST##n(RMAP, ii, N##n) \
                    MAP_TEST##n(DMAP, ii, N##n) \
                    MAP_TEST##n(EMAP, ii, N##n) \
                    MAP_TEST##n(UMAP, ii, N##n)
#else
#define RUN_TEST(n) MAP_TEST##n(KMAP, ii, N##n) \
                    MAP_TEST##n(HMAP, ii, N##n)
#endif

enum {
    DEFAULT_N_MILL = 10,
    DEFAULT_KEYBITS = 22,
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
      printf("Usage %s n-million [key-bits (default %d)]\n\n", argv[0], DEFAULT_KEYBITS);
      return 0;
    }
    unsigned n_mill = argc >= 2 ? atoi(argv[1]) : DEFAULT_N_MILL;
    unsigned keybits = argc >= 3 ? atoi(argv[2]) : DEFAULT_KEYBITS;
    unsigned n = n_mill * 1000000;
    unsigned N1 = n, N2 = n, N3 = n, N4 = n, N5 = n;
    crand_t rng;
    size_t seed = 123456; // time(NULL);

    printf("\nUnordered hash map shootout\n");
    printf("KMAP = https://github.com/attractivechaos/klib\n"
#ifdef HAVE_BOOST
           "BMAP = https://www.boost.org (unordered_flat_map)\n"
#endif
           "HMAP = https://github.com/stclib/STC (**)\n"
           //"PMAP = https://github.com/greg7mdp/parallel-hashmap\n"
           "FMAP = https://github.com/skarupke/flat_hash_map\n"
           "TMAP = https://github.com/Tessil/robin-map\n"
           "RMAP = https://github.com/martinus/robin-hood-hashing\n"
           "DMAP = https://github.com/martinus/unordered_dense\n"
           "EMAP = https://github.com//ktprime/emhash\n"
           "UMAP = std::unordered_map\n\n");

    printf("Seed = %" c_ZU ":\n", seed);

    printf("\nT1: Insert %g mill. random keys range [0, 2^%u): map[rnd] = i;\n", N1/1000000.0, keybits);
    RUN_TEST(1)

    //printf("\nT2: Insert %g mill. SEQUENTIAL keys, erase them in same order:\n", N2/1000000.0);
    //RUN_TEST(2)

    printf("\nT3: Erase all elements by lookup (%u mill. random inserts), key range [0, 2^%u)\n", n_mill*2, keybits);
    RUN_TEST(3)

    //printf("\nT4: Iterate map with Min(%u mill, 2^%u) inserts repeated times:\n", n_mill, keybits+1);
    //RUN_TEST(4)

    printf("\nT5: Lookup mix of random/existing keys in range [0, 2^%u). Num lookups depends on size.\n", keybits);
    RUN_TEST(5)
}
