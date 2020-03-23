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

#define cmap_initializer   {cvector_initializer, 0, 90, 0}
#define cmap_size(map)     ((size_t) (map)._size)
#define cmap_buckets(map)  cvector_capacity((map)._table)


// CMapEntry:
#define declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy) \
struct CMapEntry_##tag { \
    Key key; \
    Value value; \
    uint16_t hashx; \
}; \
 \
static inline void cmapentry_##tag##_destroy(struct CMapEntry_##tag* e) { \
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
    declare_CMap_5(tag, Key, Value, c_defaultDestroy, c_defaultHash)

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
    declare_CMap_10(tag, CString, Value, valueDestroy, cstring_hashRaw, strcmp, cstring_destroy, \
                         const char*, cstring_getRaw, cstring_make)


// CMap full:
#define declare_CMap_10(tag, Key, Value, valueDestroy, keyHashRaw, keyEquals, keyDestroy, \
                             KeyRaw, keyGetRaw, keyInitRaw) \
  declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy); \
  declare_CVector_4(map_##tag, CMapEntry_##tag, cmapentry_##tag##_destroy, cmapentry_noCompare); \
 \
typedef struct CMap_##tag { \
    CVector_map_##tag _table; \
    size_t _size; \
    uint8_t maxLoadPercent; \
    uint8_t shrinkLimitPercent; \
} CMap_##tag; \
 \
typedef struct cmap_##tag##_iter_t { \
    CMapEntry_##tag *item, *_end; \
} cmap_##tag##_iter_t; \
 \
static inline CMap_##tag cmap_##tag##_init(void) { \
    CMap_##tag map = cmap_initializer; \
    return map; \
} \
 \
static inline void cmap_##tag##_destroy(CMap_##tag* self) { \
    if (cmap_size(*self)) { \
        size_t cap = _cvector_capacity(self->_table); \
        CMapEntry_##tag* e = self->_table.data, *end = e + cap; \
        for (; e != end; ++e) if (e->hashx) cmapentry_##tag##_destroy(e); \
    } \
    cvector_map_##tag##_destroy(&self->_table); \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size); /* predeclared */ \
 \
static inline void cmap_##tag##_clear(CMap_##tag* self) { \
    memset(self->_table.data, 0, sizeof(CMapEntry_##tag) * _cvector_capacity(self->_table)); \
    self->_size = 0; \
} \
 \
static inline void cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    cvector_map_##tag##_swap(&a->_table, &b->_table); \
    c_swap(size_t, a->_size, b->_size); \
} \
 \
static inline void cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac) { \
    self->maxLoadPercent = (uint8_t) (fac * 100); \
    if (cmap_size(*self) >= cmap_buckets(*self) * fac) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) / fac)); \
} \
static inline void cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit) { \
    self->shrinkLimitPercent = (uint8_t) (limit * 100); \
    if (cmap_size(*self) < cmap_buckets(*self) * limit) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) * 1.2 / limit)); \
} \
 \
static inline size_t cmap_##tag##_bucket(CMap_##tag* self, KeyRaw* rawKey, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKey, sizeof(KeyRaw)), hx = (hash & cmapentry_HASH) | cmapentry_USED; \
    size_t cap = cvector_capacity(self->_table); \
    size_t idx = c_reduce(hash, cap); \
    CMapEntry_##tag* slot = self->_table.data; \
    while (slot[idx].hashx && (slot[idx].hashx != hx || !keyEquals(keyGetRaw(&slot[idx].key), rawKey))) { \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_get(CMap_##tag map, KeyRaw rawKey) { \
    if (cmap_size(map) == 0) return NULL; \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(&map, &rawKey, &hx); \
    return map._table.data[idx].hashx ? &map._table.data[idx] : NULL; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_put(CMap_##tag* self, KeyRaw rawKey, Value value) { \
    size_t cap = cvector_capacity(self->_table); \
    if (cmap_size(*self) + 1 >= cap * self->maxLoadPercent * 0.01) \
        cap = cmap_##tag##_reserve(self, (size_t) 7 + (1.6 * cap)); \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(self, &rawKey, &hx); \
    CMapEntry_##tag* e = &self->_table.data[idx]; \
    if (! e->hashx) { \
        e->key = keyInitRaw(rawKey); \
        e->hashx = hx; \
        ++self->_size; \
    } \
    e->value = value; \
    return e; \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cvector_capacity(self->_table), newcap = 1 + (size / 2) * 2; \
    if (cmap_size(*self) >= newcap * self->maxLoadPercent * 0.01) return oldcap; \
    CVector_map_##tag vec = cvector_initializer; \
    cvector_map_##tag##_reserve(&vec, newcap); \
    memset(vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    cvector_map_##tag##_swap(&self->_table, &vec); \
 \
    CMapEntry_##tag* e = vec.data, *slot = self->_table.data; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->hashx) \
            slot[ cmap_##tag##_bucket(self, keyGetRaw(&e->key), &hx) ] = *e; \
    free(_cvector_alloced(vec.data)); /* not cvector_destroy() here */ \
    return newcap; \
} \
 \
static inline bool cmap_##tag##_erase(CMap_##tag* self, KeyRaw rawKey) { \
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
        k = c_reduce(keyHashRaw(keyGetRaw(&slot[j].key), sizeof(KeyRaw)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], i = j; \
    } while (true); \
    cmapentry_##tag##_destroy(&slot[i]); /* sets used=false */ \
    --self->_size; \
    return true; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_begin(CMap_##tag map) { \
    cmap_##tag##_iter_t null = {NULL, NULL}; \
    if (cmap_size(map) == 0) return null; \
    CMapEntry_##tag* e = map._table.data, *end = e + _cvector_capacity(map._table); \
    while (e != end && !e->hashx) ++e; \
    cmap_##tag##_iter_t it = {e, end}; return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && !it.item->hashx); \
    return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_end(CMap_##tag map) { \
    CMapEntry_##tag* end = (cmap_size(map) == 0) ? NULL : map._table.data + _cvector_capacity(map._table); \
    cmap_##tag##_iter_t it = {end, end}; \
    return it; \
} \
 \
typedef Key cmap_##tag##_key_t; \
typedef Value cmap_##tag##_value_t

#endif
