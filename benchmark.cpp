#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unordered_map>
#include <c_lib/cstring.h>
#include <c_lib/cmap.h>
#include "bytell_hash_map.hpp"

declare_CMap(ii, int64_t, int64_t);
declare_CVector_string(s);
declare_CMap(ix, short, short);

const size_t seed = 123; // time(NULL);
#define RAND() rand() * rand()

#define CMAP_SETUP(tag, Key, Value) CMap_##tag map = cmap_initializer; \
                                    cmap_##tag##_setMaxLoadFactor(&map, 0.77)
#define CMAP_PUT(tag, __key, __value)   cmap_##tag##_put(&map, __key, __value)->value
#define CMAP_DEL(tag, key)          cmap_##tag##_erase(&map, key)
#define CMAP_FIND(tag, key)         (cmap_##tag##_get(map, key) != NULL)
#define CMAP_SIZE(tag)              cmap_size(map)
#define CMAP_BUCKETS(tag)           cmap_buckets(map)
#define CMAP_CLEAR(tag)             cmap_##tag##_destroy(&map)


#define UMAP_SETUP(tag, Key, Value) std::unordered_map<Key, Value> map
#define UMAP_PUT(tag, key, value)   (map[key] = value)
#define UMAP_FIND(tag, key)         (map.find(key) != map.end())
#define UMAP_DEL(tag, key)          map.erase(key)
#define UMAP_SIZE(tag)              map.size()
#define UMAP_BUCKETS(tag)           map.bucket_count()
#define UMAP_CLEAR(tag)             map.clear()


#define BMAP_SETUP(tag, Key, Value) ska::bytell_hash_map<Key, Value> map
#define BMAP_PUT(tag, key, value)   (map[key] = value)
#define BMAP_FIND(tag, key)         (map.find(key) != map.end())
#define BMAP_DEL(tag, key)          map.erase(key)
#define BMAP_SIZE(tag)              map.size()
#define BMAP_BUCKETS(tag)           map.bucket_count()
#define BMAP_CLEAR(tag)             map.clear()

#define MAP_TEST1(M, tag) \
{ \
    const size_t N = 70000000; \
    M##_SETUP(tag, int64_t, int64_t); \
    uint64_t checksum = 0, inserted = 0, erased = 0; \
    srand(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N; ++i) { \
        int64_t key = RAND(); \
        switch (key & 3) { \
            case 1: { \
                ++inserted; \
                checksum += ++M##_PUT(tag, key, i); \
                break; \
            } \
            case 2: { \
                bool found = M##_FIND(tag, key); \
                if (found) {  \
                    M##_DEL(tag, key); \
                    ++erased; \
                } \
                break; \
            } \
        } \
    } \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bk: %llu, time: %.02f, sum: %llu, ins: %llu\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC, checksum, inserted); \
    M##_CLEAR(tag); \
}

#define MAP_TEST2(M, tag) \
{ \
    const size_t N = 10000000; \
    M##_SETUP(tag, int64_t, int64_t); \
    uint64_t checksum = 0, inserted = 0, erased = 0; \
    srand(seed); \
    clock_t difference, before = clock(); \
    for (size_t i = 0; i < N; ++i) \
        M##_PUT(tag, i*123, i); \
    for (size_t i = 0; i < N/2; ++i) \
        M##_DEL(tag, i*123); \
    difference = clock() - before; \
    printf(#M "(" #tag "): sz: %llu, bk: %llu, time: %.02f\n", M##_SIZE(tag), M##_BUCKETS(tag), (float) difference / CLOCKS_PER_SEC); \
    M##_CLEAR(tag); \
}


int main()
{
    MAP_TEST1(CMAP, ii)
    MAP_TEST1(UMAP, ii)
    MAP_TEST1(BMAP, ii)

    MAP_TEST2(CMAP, ii)
    MAP_TEST2(UMAP, ii)
    MAP_TEST2(BMAP, ii)


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
