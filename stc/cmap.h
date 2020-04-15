// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CMAP_H_
#define CMAP_H_

#include "cvector.h"


#define cmap_init          {cvector_init, 0, 90, 0}
#define cmap_size(map)     ((size_t) (map)._size)
#define cmap_bucketCount(map)  cvector_capacity((map)._table)


// CMapEntry:
#define declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy) \
struct CMapEntry_##tag { \
    Key key; \
    Value value; \
    uint16_t hashx; \
}; \
 \
static inline struct CMapEntry_##tag cmapentry_##tag##_make(Key key, Value value) { \
    struct CMapEntry_##tag e = {key, value, 0}; \
    return e; \
} \
static inline void \
cmapentry_##tag##_destroy(struct CMapEntry_##tag* e) { \
    keyDestroy(&e->key); \
    valueDestroy(&e->value); \
    e->hashx = 0; \
} \
typedef struct CMapEntry_##tag CMapEntry_##tag

enum   {cmapentry_HASH=0x7fff, cmapentry_USED=0x8000};
#define cmapentry_noCompare(x, y) (0)


// CMap:
#define declare_CMap(...)  c_MACRO_OVERLOAD(declare_CMap, __VA_ARGS__)

#define declare_CMap_3(tag, Key, Value) \
    declare_CMap_4(tag, Key, Value, c_defaultDestroy)

#define declare_CMap_4(tag, Key, Value, valueDestroy) \
    declare_CMap_5(tag, Key, Value, valueDestroy, c_defaultHash)

#define declare_CMap_5(tag, Key, Value, valueDestroy, keyHash) \
    declare_CMap_7(tag, Key, Value, valueDestroy, keyHash, c_defaultEquals, c_defaultDestroy)
    
#define declare_CMap_7(tag, Key, Value, valueDestroy, keyHash, keyEquals, keyDestroy) \
    declare_CMap_10(tag, Key, Value, valueDestroy, keyHash, keyEquals, keyDestroy, \
                         Key, c_defaultGetRaw, c_defaultInitRaw)


// CMap<CString, Value>:
#define declare_CMap_stringkey(...)  c_MACRO_OVERLOAD(declare_CMap_stringkey, __VA_ARGS__)

#define declare_CMap_stringkey_2(tag, Value) \
    declare_CMap_stringkey_3(tag, Value, c_defaultDestroy)

#define declare_CMap_stringkey_3(tag, Value, valueDestroy) \
    declare_CMap_10(tag, CString, Value, valueDestroy, cstring_hashRaw, cstring_equalsRaw, cstring_destroy, \
                         const char* const, cstring_getRaw, cstring_make)


// CMap full:
#define declare_CMap_10(tag, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, keyDestroy, \
                             RawKey, keyGetRaw, keyInitRaw) \
  declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy); \
  declare_CVector_4(map_##tag, CMapEntry_##tag, cmapentry_##tag##_destroy, cmapentry_noCompare); \
  typedef RawKey cmap_##tag##_rawkey_t; \
 \
typedef struct CMap_##tag { \
    CVector_map_##tag _table; \
    size_t _size; \
    uint8_t maxLoadPercent; \
    uint8_t shrinkLimitPercent; \
} CMap_##tag; \
static const CMap_##tag cmap_##tag##_init = cmap_init; \
 \
typedef struct cmap_##tag##_iter_t { \
    CMapEntry_##tag *item, *_end; \
} cmap_##tag##_iter_t; \
 \
static inline void \
cmap_##tag##_destroy(CMap_##tag* self) { \
    if (cmap_size(*self)) { \
        size_t cap = _cvector_capacity(self->_table); \
        CMapEntry_##tag* e = self->_table.data, *end = e + cap; \
        for (; e != end; ++e) if (e->hashx) cmapentry_##tag##_destroy(e); \
    } \
    free(_cvector_alloced(self->_table.data)); \
} \
 \
static inline size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size); /* predeclared */ \
 \
static inline void cmap_##tag##_clear(CMap_##tag* self) { \
    memset(self->_table.data, 0, sizeof(CMapEntry_##tag) * _cvector_capacity(self->_table)); \
    self->_size = 0; \
} \
 \
static inline void \
cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    c_swap(CMap_##tag, *a, *b); \
} \
 \
static inline void \
cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac) { \
    self->maxLoadPercent = (uint8_t) (fac * 100); \
    if (cmap_size(*self) >= cmap_bucketCount(*self) * fac) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) / fac)); \
} \
 \
static inline void \
cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit) { \
    self->shrinkLimitPercent = (uint8_t) (limit * 100); \
    if (cmap_size(*self) < cmap_bucketCount(*self) * limit) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) * 1.2 / limit)); \
} \
 \
static inline size_t \
cmap_##tag##_bucket(CMap_##tag* self, cmap_##tag##_rawkey_t* const rawKey, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKey, sizeof(cmap_##tag##_rawkey_t)), hx = (hash & cmapentry_HASH) | cmapentry_USED; \
    size_t cap = cvector_capacity(self->_table); \
    size_t idx = cmap_reduce(hash, cap); \
    CMapEntry_##tag* slot = self->_table.data; \
    while (slot[idx].hashx && (slot[idx].hashx != hx || !keyEqualsRaw((RawKey* const) keyGetRaw(&slot[idx].key), rawKey))) { \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
static inline CMapEntry_##tag* \
cmap_##tag##_get(CMap_##tag map, cmap_##tag##_rawkey_t rawKey) { \
    if (cmap_size(map) == 0) return NULL; \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(&map, &rawKey, &hx); \
    return map._table.data[idx].hashx ? &map._table.data[idx] : NULL; \
} \
 \
static inline void \
cmap_##tag##_expand(CMap_##tag* self) { \
    size_t cap = cvector_capacity(self->_table); \
    if (cmap_size(*self) + 1 >= cap * self->maxLoadPercent * 0.01) \
        cmap_##tag##_reserve(self, (size_t) 7 + (1.6 * cap)); \
} \
 \
static inline CMapEntry_##tag* \
cmap_##tag##_put(CMap_##tag* self, cmap_##tag##_rawkey_t rawKey, Value value) { \
    cmap_##tag##_expand(self);  \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(self, &rawKey, &hx); \
    CMapEntry_##tag* e = &self->_table.data[idx]; \
    if (e->hashx) \
        valueDestroy(&e->value); \
    else { \
        e->key = keyInitRaw(rawKey); \
        e->hashx = hx; \
        ++self->_size; \
    } \
    e->value = value; \
    return e; \
} \
/* \
static inline CMapEntry_##tag* \
cmap_##tag##_insert(CMap_##tag* self, CMapEntry_##tag entry) { \
    cmap_##tag##_expand(self);  \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(self, (RawKey* const) keyGetRaw(&entry.key), &hx); \
    CMapEntry_##tag* e = &self->_table.data[idx]; \
    if (e->hashx) \
        valueDestroy(&e->value); \
    else { \
        e->key = entry.key; \
        e->hashx = hx; \
        ++self->_size; \
    } \
    e->value = entry.value; \
    return e; \
} \
*/ \
static inline size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cvector_capacity(self->_table), newcap = 1 + (size / 2) * 2; \
    if (cmap_size(*self) >= newcap * self->maxLoadPercent * 0.01) return oldcap; \
    CVector_map_##tag vec = cvector_map_##tag##_init; \
    cvector_map_##tag##_reserve(&vec, newcap); \
    memset(vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    cvector_map_##tag##_swap(&self->_table, &vec); \
 \
    CMapEntry_##tag* e = vec.data, *slot = self->_table.data; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->hashx) \
            slot[ cmap_##tag##_bucket(self, (RawKey* const) keyGetRaw(&e->key), &hx) ] = *e; \
    free(_cvector_alloced(vec.data)); /* not cvector_destroy() here */ \
    return newcap; \
} \
 \
static inline bool \
cmap_##tag##_erase(CMap_##tag* self, cmap_##tag##_rawkey_t rawKey) { \
    if (cmap_size(*self) == 0) \
        return false; \
    size_t cap = cvector_capacity(self->_table); \
    if (cmap_size(*self) < cap * self->shrinkLimitPercent * 0.01) \
        cap = cmap_##tag##_reserve(self, cmap_size(*self) * 120 / self->maxLoadPercent); \
    uint32_t hx; \
    size_t i = cmap_##tag##_bucket(self, &rawKey, &hx), j = i, k; \
    CMapEntry_##tag* slot = self->_table.data; \
    if (! slot[i].hashx) \
        return false; \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* j %= cap; is slow */ \
        if (! slot[j].hashx) \
            break; \
        k = cmap_reduce(keyHashRaw((RawKey* const) keyGetRaw(&slot[j].key), sizeof(cmap_##tag##_rawkey_t)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], i = j; \
    } while (true); \
    cmapentry_##tag##_destroy(&slot[i]); /* sets used=false */ \
    --self->_size; \
    return true; \
} \
 \
static inline cmap_##tag##_iter_t \
cmap_##tag##_begin(CMap_##tag map) { \
    cmap_##tag##_iter_t null = {NULL, NULL}; \
    if (cmap_size(map) == 0) return null; \
    CMapEntry_##tag* e = map._table.data, *end = e + _cvector_capacity(map._table); \
    while (e != end && !e->hashx) ++e; \
    cmap_##tag##_iter_t it = {e, end}; return it; \
} \
 \
static inline cmap_##tag##_iter_t \
cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && !it.item->hashx); \
    return it; \
} \
 \
static inline cmap_##tag##_iter_t \
cmap_##tag##_end(CMap_##tag map) { \
    CMapEntry_##tag* end = (cmap_size(map) == 0) ? NULL : map._table.data + _cvector_capacity(map._table); \
    cmap_##tag##_iter_t it = {end, end}; \
    return it; \
} \
typedef Key cmap_##tag##_key_t; \
typedef Value cmap_##tag##_value_t


// https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
static inline uint32_t cmap_reduce(uint32_t x, uint32_t N) {
    return ((uint64_t) x * (uint64_t) N) >> 32 ;
}

#endif
