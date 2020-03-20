#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unordered_map>
#include <c_lib/cstring.h>
#include <c_lib/cmap.h>
#include "others/bytell_hash_map.h"
#include "others/robin_hood.h"
#include "others/khash.h"

declare_CMap(ii, int64_t, int64_t);
declare_CVector_string(s);
declare_CMap(ix, short, short);

KHASH_MAP_INIT_INT64(ii, uint64_t)

const size_t seed = 123; // time(NULL);
const double maxLoadFactor = 0.77;
#define RAND() rand() * rand()

#define CMAP_SETUP(tag, Key, Value) CMap_##tag map = cmap_initializer; \
                                    cmap_##tag##_setMaxLoadFactor(&map, maxLoadFactor)
#define CMAP_PUT(tag, __key, __value)   cmap_##tag##_put(&map, __key, __value)->value
#define CMAP_DEL(tag, key)          cmap_##tag##_erase(&map, key)
#define CMAP_FIND(tag, key)         (cmap_##tag##_get(map, key) != NULL)
#define CMAP_SIZE(tag)              cmap_size(map)
#define CMAP_BUCKETS(tag)           cmap_buckets(map)
#define CMAP_CLEAR(tag)             cmap_##tag##_destroy(&map)


#define KMAP_SETUP(tag, Key, Value) khash_t(ii)* map = kh_init(ii); \
                                    khiter_t ki; int ret
#define KMAP_PUT(tag, key, value)   kh_value(map, kh_put(ii, map, key, &ret)) = value
#define KMAP_DEL(tag, key)          (ki = kh_get(ii, map, key)) != kh_end(map) ? kh_del(ii, map, ki), 1 : 0
#define KMAP_FIND(tag, key)         (kh_get(ii, map, key) != kh_end(map))
#define KMAP_SIZE(tag)              ((size_t) kh_size(map))
#define KMAP_BUCKETS(tag)           ((size_t) kh_n_buckets(map))
#define KMAP_CLEAR(tag)             kh_destroy(ii, map) 


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

#define RMAP_SETUP(tag, Key, Value) robin_hood::unordered_map<Key, Value> map
#define RMAP_PUT(tag, key, value)   (map[key] = value)
#define RMAP_FIND(tag, key)         (map.find(key) != map.end())
#define RMAP_DEL(tag, key)          map.erase(key)
#define RMAP_SIZE(tag)              map.size()
#define RMAP_BUCKETS(tag)           map.bucket_count()
#define RMAP_CLEAR(tag)             map.clear()


#define MAP_TEST1(M, tag) \
{ \
    const size_t N = 10000000; \
    M##_SETUP(tag, int64_t, int64_t); \
    uint64_t checksum = 0, inserted = 0, erased = 0; \
    srand(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N; ++i) { \
        int64_t key = RAND(); \
        switch (rand() & 1) { \
            case 0: { \
                checksum += ++M##_PUT(tag, key, i); \
                ++inserted; \
                break; \
            } \
            case 1: { \
                bool found = M##_FIND(tag, key); \
                if (found) erased += M##_DEL(tag, key); \
                break; \
            } \
        } \
    } \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bk: %llu, time: %.02f, sum: %llu, ins: %llu, del: %llu\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, checksum, inserted, erased); \
    M##_CLEAR(tag); \
}

#define MAP_TEST2(M, tag) \
{ \
    const size_t N = 10000000; \
    M##_SETUP(tag, int64_t, int64_t); \
    srand(seed); \
    size_t erased = 0; \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N; ++i) \
        M##_PUT(tag, i*123, i); \
    const size_t D=N*3/4; \
    for (size_t i = 0; i < N; ++i) \
        erased += M##_DEL(tag, i*123); \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bk: %llu, time: %.02f, del %llu\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, erased); \
    M##_CLEAR(tag); \
}


int main()
{
    MAP_TEST1(UMAP, ii)
    MAP_TEST1(CMAP, ii)
    MAP_TEST1(KMAP, ii)
    MAP_TEST1(BMAP, ii)
    MAP_TEST1(RMAP, ii)

    MAP_TEST2(UMAP, ii)
    MAP_TEST2(CMAP, ii)
    MAP_TEST2(KMAP, ii)
    MAP_TEST2(BMAP, ii)
    MAP_TEST2(RMAP, ii)

    printf("ix entry size %llu\n", sizeof(CMapEntry_ix));
    
    CVector_s names = cvector_initializer;
    cvector_s_push(&names, cstring_make("Robert"));
    cvector_s_push(&names, cstring_make("Johnny"));
    cvector_s_push(&names, cstring_make("Anne"));
    cvector_s_push(&names, cstring_make("Ruth"));
    cvector_s_push(&names, cstring_make("Burt"));
    
    cvector_s_sort(&names);
    c_foreach (i, cvector_s, names)
        printf("%s\n", i.item->str);
    cvector_s_destroy(&names);
}
