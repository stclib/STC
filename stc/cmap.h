/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CMAP__H__
#define CMAP__H__

#include "cvector.h"

#define cmap_init          {cvector_init, 0, 90, 0}
#define cmap_size(map)     ((size_t) (map)._size)
#define cmap_bucketCount(map)  cvector_capacity((map)._table)
/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define cmap_reduce(x, N)  ((uint32_t) (((uint64_t) (x) * (N)) >> 32))

enum   {cmapentry_HASH=0x7fff, cmapentry_USED=0x8000};

/* CMap: */
#define declare_CMap(...) \
    c_MACRO_OVERLOAD(declare_CMap, __VA_ARGS__)

#define declare_CMap_3(tag, Key, Value) \
    declare_CMap_4(tag, Key, Value, c_noDestroy)

#define declare_CMap_4(tag, Key, Value, valueDestroy) \
    declare_CMap_5(tag, Key, Value, valueDestroy, c_defaultHash)

#define declare_CMap_5(tag, Key, Value, valueDestroy, keyHash) \
    declare_CMap_6(tag, Key, Value, valueDestroy, keyHash, c_defaultEquals)
    
#define declare_CMap_6(tag, Key, Value, valueDestroy, keyHash, keyEquals) \
    declare_CMap_10(tag, Key, Value, valueDestroy, c_noDestroy, Key, \
                         keyHash, keyEquals, c_defaultGetRaw, c_defaultInitRaw)


/* CMap<CString, Value>: */
#define declare_CMap_stringkey(...) \
    c_MACRO_OVERLOAD(declare_CMap_stringkey, __VA_ARGS__)

#define declare_CMap_stringkey_2(tag, Value) \
    declare_CMap_stringkey_3(tag, Value, c_noDestroy)

#define declare_CMap_stringkey_3(tag, Value, valueDestroy) \
    declare_CMap_10(tag, CString, Value, valueDestroy, cstring_destroy, const char*, \
                         cstring_hashRaw, cstring_equalsRaw, cstring_getRaw, cstring_make)


/* CMap full: */
#define declare_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                             keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
\
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
  typedef struct CMapEntry_##tag CMapEntry_##tag; \
\
  declare_CVector_4(map_##tag, CMapEntry_##tag, cmapentry_##tag##_destroy, c_noCompare); \
  typedef RawKey cmap_##tag##_rawkey_t; \
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
STC_API void \
cmap_##tag##_destroy(CMap_##tag* self); \
 \
STC_API void \
cmap_##tag##_clear(CMap_##tag* self); \
 \
STC_API void \
cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac); \
 \
STC_API void \
cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_get(CMap_##tag map, cmap_##tag##_rawkey_t rawKey); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_put(CMap_##tag* self, cmap_##tag##_rawkey_t rawKey, Value value); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_insert(CMap_##tag* self, CMapEntry_##tag entry); \
 \
STC_API size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size); \
 \
STC_API bool \
cmap_##tag##_erase(CMap_##tag* self, cmap_##tag##_rawkey_t rawKey); \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_begin(CMap_##tag* map); \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_next(cmap_##tag##_iter_t it); \
 \
implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                       keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
 \
typedef Key cmap_##tag##_key_t; \
typedef Value cmap_##tag##_value_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                               keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
 \
STC_API void \
cmap_##tag##_destroy(CMap_##tag* self) { \
    if (cmap_size(*self)) { \
        size_t cap = _cvector_capacity(self->_table); \
        CMapEntry_##tag* e = self->_table.data, *end = e + cap; \
        for (; e != end; ++e) if (e->hashx) cmapentry_##tag##_destroy(e); \
    } \
    free(_cvector_alloced(self->_table.data)); \
} \
 \
STC_API void cmap_##tag##_clear(CMap_##tag* self) { \
    memset(self->_table.data, 0, sizeof(CMapEntry_##tag) * _cvector_capacity(self->_table)); \
    self->_size = 0; \
} \
 \
STC_API void \
cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac) { \
    self->maxLoadPercent = (uint8_t) (fac * 100); \
    if (cmap_size(*self) >= cmap_bucketCount(*self) * fac) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) / fac)); \
} \
 \
STC_API void \
cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit) { \
    self->shrinkLimitPercent = (uint8_t) (limit * 100); \
    if (cmap_size(*self) < cmap_bucketCount(*self) * limit) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) * 1.2 / limit)); \
} \
 \
static inline size_t \
cmap_##tag##_bucket(CMap_##tag* self, const cmap_##tag##_rawkey_t* rawKeyPtr, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKeyPtr, sizeof(cmap_##tag##_rawkey_t)), sx, hx = (hash & cmapentry_HASH) | cmapentry_USED; \
    size_t cap = cvector_capacity(self->_table); \
    size_t idx = cmap_reduce(hash, cap); \
    CMapEntry_##tag* slot = self->_table.data; \
    while ((sx = slot[idx].hashx)) { \
        if (sx == hx) { \
            cmap_##tag##_rawkey_t r = keyGetRaw(&slot[idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
STC_API CMapEntry_##tag* \
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
STC_API CMapEntry_##tag* \
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
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_insert(CMap_##tag* self, CMapEntry_##tag entry) { \
    cmap_##tag##_expand(self);  \
    uint32_t hx; \
    cmap_##tag##_rawkey_t r = keyGetRaw(&entry.key); \
    size_t idx = cmap_##tag##_bucket(self, &r, &hx); \
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
 \
static inline void \
cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    c_swap(CMap_##tag, *a, *b); \
} \
 \
STC_API size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cvector_capacity(self->_table), newcap = 1 + (size / 2) * 2; \
    if (cmap_size(*self) >= newcap * self->maxLoadPercent * 0.01) return oldcap; \
    CVector_map_##tag vec = cvector_init; \
    cvector_map_##tag##_reserve(&vec, newcap); \
    memset(vec.data, 0, sizeof(CMapEntry_##tag) * newcap); \
    cvector_map_##tag##_swap(&self->_table, &vec); \
 \
    CMapEntry_##tag* e = vec.data, *slot = self->_table.data; \
    uint32_t hx; \
    cmap_##tag##_rawkey_t r; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->hashx) \
            slot[ cmap_##tag##_bucket(self, (r = keyGetRaw(&e->key), &r), &hx) ] = *e; \
    free(_cvector_alloced(vec.data)); /* not cvector_destroy() here */ \
    return newcap; \
} \
 \
STC_API bool \
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
    cmap_##tag##_rawkey_t r; \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j %= cap; is slow */ \
        if (! slot[j].hashx) \
            break; \
        k = cmap_reduce(keyHashRaw((r = keyGetRaw(&slot[j].key), &r), \
                        sizeof(cmap_##tag##_rawkey_t)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], i = j; \
    } while (true); \
    cmapentry_##tag##_destroy(&slot[i]); /* sets used=false */ \
    --self->_size; \
    return true; \
} \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_begin(CMap_##tag* map) { \
    CMapEntry_##tag* e = map->_table.data, *end = e + _cvector_capacity(map->_table); \
    while (e != end && !e->hashx) ++e; \
    cmap_##tag##_iter_t it = {e == end ? NULL : e, end}; return it; \
} \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && !it.item->hashx); \
    if (it.item == it._end) it.item = NULL; \
    return it; \
}

#else
#define implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                               keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw)
#endif

#endif
