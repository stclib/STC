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

#define cmap_initializer   {cvector_initializer, 0, 0.9f}
#define cmap_size(map)     ((size_t) (map)._size)
#define cmap_buckets(map)  cvector_capacity((map)._vec)


// CMapEntry:
#define declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy) \
struct CMapEntry_##tag { \
    Key key; \
    Value value; \
    uint8_t used, changed; \
}; \
 \
static inline void cmapentry_##tag##_destroy(struct CMapEntry_##tag* e) { \
    keyDestroy(&e->key); \
    valueDestroy(&e->value); \
    e->used = false; \
} \
typedef struct CMapEntry_##tag CMapEntry_##tag


// CMap:
#define declare_CMap(...)  c_MACRO_OVERLOAD(declare_CMap, __VA_ARGS__)

#define declare_CMap_3(tag, Key, Value) \
    declare_CMap_4(tag, Key, Value, c_defaultDestroy)

#define declare_CMap_4(tag, Key, Value, valueDestroy) \
    declare_CMap_7(tag, Key, Value, valueDestroy, memcmp, c_defaultHash, c_defaultDestroy)
    
#define declare_CMap_7(tag, Key, Value, valueDestroy, keyCompare, keyHash, keyDestroy) \
    declare_CMap_10(tag, Key, Value, valueDestroy, keyCompare, keyHash, keyDestroy, Key, c_defaultGetRaw, c_defaultInitRaw)


// CMap<CString, Value>:
#define declare_CMap_stringkey(...)  c_MACRO_OVERLOAD(declare_CMap_stringkey, __VA_ARGS__)

#define declare_CMap_stringkey_2(tag, Value) \
    declare_CMap_stringkey_3(tag, Value, c_defaultDestroy)

#define declare_CMap_stringkey_3(tag, Value, valueDestroy) \
    declare_CMap_10(tag, CString, Value, valueDestroy, cstring_compareRaw, cstring_hashRaw, cstring_destroy, \
                         const char*, cstring_getRaw, cstring_make)


// CMap full:
#define declare_CMap_10(tag, Key, Value, valueDestroy, keyCompareRaw, keyHashRaw, keyDestroy, \
                             KeyRaw, keyGetRaw, keyInitRaw) \
  declare_CMapEntry(tag, Key, Value, valueDestroy, keyDestroy); \
  declare_CVector_3(map_##tag, CMapEntry_##tag, cmapentry_##tag##_destroy); \
 \
typedef struct CMap_##tag { \
    CVector_map_##tag _vec; \
    size_t _size; \
    float maxLoadFactor; \
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
        size_t cap = _cvector_capacity(self->_vec); \
        CMapEntry_##tag* e = self->_vec.data, *end = e + cap; \
        for (; e != end; ++e) if (e->used) cmapentry_##tag##_destroy(e); \
    } \
    cvector_map_##tag##_destroy(&self->_vec); \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size); /* predeclared */ \
 \
static inline void cmap_##tag##_clear(CMap_##tag* self) { \
    CMap_##tag map = cmap_initializer; \
    cmap_##tag##_destroy(self); \
    *self = map; \
} \
 \
static inline void cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    cvector_map_##tag##_swap(&a->_vec, &b->_vec); \
    c_swap(size_t, a->_size, b->_size); \
} \
 \
static inline void cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, float fac) { \
    self->maxLoadFactor = fac; \
    if (cmap_size(*self) > cmap_buckets(*self) * fac) \
        cmap_##tag##_reserve(self, 1 + (size_t) (cmap_size(*self) / fac)); \
} \
 \
static inline size_t cmap_##tag##_bucket(CMap_##tag* self, KeyRaw rawKey) { \
    size_t cap = cvector_capacity(self->_vec), erased_idx = cap; \
    size_t idx = c_reduce(keyHashRaw(&rawKey, sizeof(KeyRaw)), cap); \
    while (self->_vec.data[idx].used && keyCompareRaw(&self->_vec.data[idx].key, &rawKey, sizeof(Key)) != 0) {\
        if (++idx == cap) idx = 0; \
    } \
    return idx; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_get(CMap_##tag map, KeyRaw rawKey) { \
    if (cmap_size(map) == 0) return NULL; \
    size_t idx = cmap_##tag##_bucket(&map, rawKey); \
    return map._vec.data[idx].used ? &map._vec.data[idx] : NULL; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_put(CMap_##tag* self, KeyRaw rawKey, Value value) { \
    size_t cap = cvector_capacity(self->_vec); \
    if (cmap_size(*self) + 1 >= cap * self->maxLoadFactor) \
        cap = cmap_##tag##_reserve(self, (size_t) 7 + (1.6 * cap)); \
    size_t idx = cmap_##tag##_bucket(self, rawKey); \
    CMapEntry_##tag* e = &self->_vec.data[idx]; \
    if (e->used) \
        e->changed = true; \
    else { \
        e->key = keyInitRaw(rawKey); \
        e->used = true; \
        ++self->_size; \
    } \
    e->value = value; \
    return e; \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cvector_capacity(self->_vec), newcap = 1 + (size / 2) * 2; \
    if (cmap_size(*self) >= newcap * self->maxLoadFactor) return oldcap; \
    CVector_map_##tag vec = cvector_initializer; \
    cvector_map_##tag##_reserve(&vec, newcap); \
    memset(vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    cvector_map_##tag##_swap(&self->_vec, &vec); \
 \
    CMapEntry_##tag* e = vec.data, *slot = self->_vec.data; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->used) \
            slot[ cmap_##tag##_bucket(self, keyGetRaw(e->key)) ] = *e; \
    free(_cvector_alloced(vec.data)); /* not cvector_destroy() here */ \
    return newcap; \
} \
 \
static inline bool cmap_##tag##_erase(CMap_##tag* self, KeyRaw rawKey) { \
    if (cmap_size(*self) == 0) \
        return false; \
    size_t cap = cvector_capacity(self->_vec); \
    if (cmap_size(*self) * 1.6 < cap * self->maxLoadFactor) \
        cap = cmap_##tag##_reserve(self, 1.2 * cmap_size(*self) / self->maxLoadFactor); \
    size_t i = cmap_##tag##_bucket(self, rawKey), j = i, k; \
    CMapEntry_##tag* slot = self->_vec.data; \
    if (! slot[i].used) \
        return false; \
    do { /* https://attractivechaos.wordpress.com/2019/12/28/deletion-from-hash-tables-without-tombstones/ */ \
        if (++j == cap) j = 0; /* j %= cap; is slow */ \
        if (! slot[j].used) \
            break; \
        KeyRaw r = keyGetRaw(slot[j].key); \
        k = c_reduce(keyHashRaw(&r, sizeof(KeyRaw)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* Check if k is outside [i, j) range */ \
            slot[i] = slot[j], i = j; \
    } while (true); \
    cmapentry_##tag##_destroy(&slot[i]); \
    slot[i].used = false; \
    --self->_size; \
    return true; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_begin(CMap_##tag map) { \
    cmap_##tag##_iter_t null = {NULL, NULL}; \
    if (cmap_size(map) == 0) return null; \
    CMapEntry_##tag* e = map._vec.data, *end = e + _cvector_capacity(map._vec); \
    while (e != end && !e->used) ++e; \
    cmap_##tag##_iter_t it = {e, end}; return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && !it.item->used); \
    return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_end(CMap_##tag map) { \
    CMapEntry_##tag* end = (cmap_size(map) == 0) ? NULL : map._vec.data + _cvector_capacity(map._vec); \
    cmap_##tag##_iter_t it = {end, end}; \
    return it; \
} \
typedef Key cmap_##tag##_key_t; \
typedef Value cmap_##tag##_value_t


#endif
