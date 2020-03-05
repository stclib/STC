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

#define cmap_initializer  {cvector_initializer, 0}
#define cmap_size(cm)     ((size_t) (cm)._size)
#define cmap_capacity(cm) cvector_capacity((cm)._vec)


// CMapEntry:
#define declare_CMapEntry(tag, Key, Value, keyDestroy, valueDestroy) \
struct CMapEntry_##tag { \
    Key key; \
    Value value; \
    short _used, changed; \
}; \
 \
static inline void cmapentry_##tag##_destroy(struct CMapEntry_##tag* p) { \
    keyDestroy(&p->key); \
    valueDestroy(&p->value); \
    p->_used = p->changed = 0; \
} \
typedef struct CMapEntry_##tag CMapEntry_##tag


// CMap:
#define declare_CMap(...)  cdef_MACRO_OVERLOAD(declare_CMap, __VA_ARGS__)

#define declare_CMap_3(tag, Key, Value) \
    declare_CMap_4(tag, Key, Value, cdef_destroy)

#define declare_CMap_4(tag, Key, Value, valueDestroy) \
    declare_CMap_10(tag, Key, Value, valueDestroy, Key, cdef_getRaw, memcmp, cdef_murmurHash, cdef_initRaw, cdef_destroy)


// CMap<CString, Value>:
#define declare_CMap_StringKey(...)  cdef_MACRO_OVERLOAD(declare_CMap_StringKey, __VA_ARGS__)

#define declare_CMap_StringKey_2(tag, Value) \
    declare_CMap_StringKey_3(tag, Value, cdef_destroy)

#define declare_CMap_StringKey_3(tag, Value, valueDestroy) \
    declare_CMap_10(tag, CString, Value, valueDestroy, const char*, cstring_getRaw, cstring_compare, cstring_hash, cstring_make, cstring_destroy)


// CMap full:
#define declare_CMap_10(tag, Key, Value, valueDestroy, KeyRaw, keyGetRaw, keyCompare, keyHasher, keyInit, keyDestroy) \
  declare_CMapEntry(tag, Key, Value, keyDestroy, valueDestroy); \
  declare_CVector_3(map_##tag, CMapEntry_##tag, cmapentry_##tag##_destroy); \
 \
typedef struct CMap_##tag { \
    CVector_map_##tag _vec; \
    size_t _size; \
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
    if (self->_size) { \
        size_t cap = _cvector_capacity(self->_vec); \
        CMapEntry_##tag* p = self->_vec.data, *end = p + cap; \
        for (; p != end; ++p) if (p->_used) cmapentry_##tag##_destroy(p); \
    } \
    cvector_map_##tag##_destroy(&self->_vec); \
} \
 \
static inline void cmap_##tag##_clear(CMap_##tag* self) { \
    CMap_##tag cm = cmap_initializer; \
    cmap_##tag##_destroy(self); \
    *self = cm; \
} \
 \
static inline void cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    cvector_map_##tag##_swap(&a->_vec, &b->_vec); \
    _cdef_swap(size_t, a->_size, b->_size); \
} \
 \
static inline size_t _cmap_##tag##_findIndex(CMap_##tag cm, KeyRaw rawKey) { \
    size_t cap = cvector_capacity(cm._vec); \
    size_t idx = keyHasher(&rawKey, sizeof(Key)) % cap, first = idx; \
    FIBONACCI_DECL; \
    while (cm._vec.data[idx]._used && keyCompare(&cm._vec.data[idx].key, &rawKey, sizeof(Key)) != 0) \
        idx = (first + FIBONACCI_NEXT) % cap; \
    return idx; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_get(CMap_##tag cm, KeyRaw rawKey) { \
    if (cm._size == 0) return NULL; \
    size_t idx = _cmap_##tag##_findIndex(cm, rawKey); \
    return cm._vec.data[idx]._used ? &cm._vec.data[idx] : NULL; \
} \
 \
static inline size_t cmap_##tag##_rehash(CMap_##tag* self); /* predeclared */ \
 \
static inline CMapEntry_##tag* cmap_##tag##_put(CMap_##tag* self, KeyRaw rawKey, Value value) { \
    size_t cap = cvector_capacity(self->_vec); \
    if (self->_size >= cap * 8 / 10) \
        cap = cmap_##tag##_rehash(self); \
    size_t idx = _cmap_##tag##_findIndex(*self, rawKey); \
    CMapEntry_##tag* e = &self->_vec.data[idx]; \
    e->value = value; \
    e->changed = e->_used; \
    if (!e->_used) { \
        e->key = keyInit(rawKey); \
        e->_used = 1; \
        ++self->_size; \
    } \
    return e; \
} \
 \
static inline size_t cmap_##tag##_rehash(CMap_##tag* self) { \
    CVector_map_##tag vec = cvector_initializer; \
    size_t newcap = 7 + cmap_capacity(*self) * 2; \
    cvector_map_##tag##_swap(&self->_vec, &vec); \
    cvector_map_##tag##_reserve(&self->_vec, newcap); \
    self->_size = 0; \
    memset(self->_vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    CMapEntry_##tag* p = vec.data; \
    size_t i, oldcap = cvector_capacity(vec); \
    for (i = 0; i < oldcap; ++i, ++p) \
        if (p->_used) cmap_##tag##_put(self, keyGetRaw(p->key), p->value); \
    return newcap; \
} \
 \
static inline int cmap_##tag##_erase(CMap_##tag* self, KeyRaw rawKey) { \
    CMapEntry_##tag* entryPtr = cmap_##tag##_get(*self, rawKey); \
    if (entryPtr) { \
        cmapentry_##tag##_destroy(entryPtr); \
        --self->_size; \
        return 1; \
    } \
    return 0; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_begin(CMap_##tag map) { \
    cmap_##tag##_iter_t null = {NULL, NULL}; \
    if (map._size == 0) return null; \
    CMapEntry_##tag* p = map._vec.data, *end = p + _cvector_capacity(map._vec); \
    while (p != end && !p->_used) ++p; \
    cmap_##tag##_iter_t it = {p, end}; return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    ++it.item; \
    while (it.item != it._end && !it.item->_used) ++it.item; \
    return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_end(CMap_##tag map) { \
    CMapEntry_##tag* end = (map._size == 0) ? NULL : map._vec.data + _cvector_capacity(map._vec); \
    cmap_##tag##_iter_t it = {end, end}; \
    return it; \
} \
typedef Key cmap_##tag##_key_t; \
typedef Value cmap_##tag##_value_t


#define FIBONACCI_DECL  size_t fib1 = 1, fib2 = 2, fibx
#define FIBONACCI_NEXT  (fibx = fib1 + fib2, fib1 = fib2, fib2 = fibx)

#endif
