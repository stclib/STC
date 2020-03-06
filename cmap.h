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
enum { CMapEntry_VACANT = 0, 
       CMapEntry_INUSE = 1,
       CMapEntry_REMOVED = 2
};
#define declare_CMapEntry(tag, Key, Value, keyDestroy, valueDestroy) \
struct CMapEntry_##tag { \
    Key key; \
    Value value; \
    uint8_t state, changed; \
}; \
 \
static inline void cmapentry_##tag##_destroy(struct CMapEntry_##tag* e) { \
    keyDestroy(&e->key); \
    valueDestroy(&e->value); \
    e->state = CMapEntry_VACANT; \
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
        CMapEntry_##tag* e = self->_vec.data, *end = e + cap; \
        for (; e != end; ++e) if (e->state == CMapEntry_INUSE) cmapentry_##tag##_destroy(e); \
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
static inline size_t cmap_##tag##_bucket(CMap_##tag cm, KeyRaw rawKey) { \
    size_t cap = cvector_capacity(cm._vec); \
    size_t idx = keyHasher(&rawKey, sizeof(Key)) % cap; \
    size_t first = idx, found = cap, state = cm._vec.data[idx].state; \
    FIBONACCI_DECL; \
    do { \
        if ((state == CMapEntry_INUSE) && keyCompare(&cm._vec.data[idx].key, &rawKey, sizeof(Key)) == 0) return idx; \
        if ((state == CMapEntry_REMOVED) && found == cap) found = idx; \
        state = cm._vec.data[ idx = (first + FIBONACCI_NEXT) % cap ].state; \
    } while (state != CMapEntry_VACANT); \
    return found == cap ? idx : found; \
} \
 \
static inline CMapEntry_##tag* cmap_##tag##_get(CMap_##tag cm, KeyRaw rawKey) { \
    if (cm._size == 0) return NULL; \
    size_t idx = cmap_##tag##_bucket(cm, rawKey); \
    return cm._vec.data[idx].state == CMapEntry_INUSE ? &cm._vec.data[idx] : NULL; \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size); /* predeclared */ \
 \
static inline CMapEntry_##tag* cmap_##tag##_put(CMap_##tag* self, KeyRaw rawKey, Value value) { \
    size_t cap = cvector_capacity(self->_vec); \
    if (self->_size >= cap * 4 / 5) \
        cap = cmap_##tag##_reserve(self, cap * 5 / 3); \
    size_t idx = cmap_##tag##_bucket(*self, rawKey); \
    CMapEntry_##tag* e = &self->_vec.data[idx]; \
    e->value = value; \
    e->changed = (e->state == CMapEntry_INUSE); \
    if (e->state != CMapEntry_INUSE) { \
        e->key = keyInit(rawKey); \
        e->state = CMapEntry_INUSE; \
        ++self->_size; \
    } \
    return e; \
} \
 \
static inline size_t cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cvector_capacity(self->_vec), newcap = (oldcap ? 1 : 7) + (size / 2) * 2; \
    if (oldcap >= newcap) return oldcap; \
    CVector_map_##tag vec = cvector_initializer; \
    cvector_map_##tag##_swap(&self->_vec, &vec); \
    cvector_map_##tag##_reserve(&self->_vec, newcap); \
    self->_size = 0; \
    memset(self->_vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    CMapEntry_##tag* e = vec.data; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->state == CMapEntry_INUSE) cmap_##tag##_put(self, keyGetRaw(e->key), e->value); \
    return newcap; \
} \
 \
static inline int cmap_##tag##_erase(CMap_##tag* self, KeyRaw rawKey) { \
    CMapEntry_##tag* e = cmap_##tag##_get(*self, rawKey); \
    if (e) { \
        cmapentry_##tag##_destroy(e); \
        e->state = CMapEntry_REMOVED; \
        --self->_size; \
        return 1; \
    } \
    return 0; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_begin(CMap_##tag map) { \
    cmap_##tag##_iter_t null = {NULL, NULL}; \
    if (map._size == 0) return null; \
    CMapEntry_##tag* e = map._vec.data, *end = e + _cvector_capacity(map._vec); \
    while (e != end && e->state != CMapEntry_INUSE) ++e; \
    cmap_##tag##_iter_t it = {e, end}; return it; \
} \
 \
static inline cmap_##tag##_iter_t cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && it.item->state != CMapEntry_INUSE); \
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


#define FIBONACCI_DECL  size_t fib1 = 0, fib2 = 1, fibx
#define FIBONACCI_NEXT  (fibx = fib1 + fib2, fib1 = fib2, fib2 = fibx)

#endif
