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

/*  // Example:
#include <stdio.h>
#include "stc/cmap.h"
declare_CHash(sx, set, int);
declare_CHash(mx, map, int, char);

int main(void) {
    CHash_sx s = chash_init;
    chash_sx_put(&s, 5);
    chash_sx_put(&s, 8);
    c_foreach (i, chash_sx, s) printf("set %d\n", i.item->key);
    chash_mx_destroy(&s);

    CHash_mx m = chash_init;
    chash_mx_put(&m, 5, 'a');
    chash_mx_put(&m, 8, 'b');
    chash_mx_put(&m, 12, 'c');
    CHashEntry_mx *e = chash_mx_get(&m, 10); // = NULL
    char val = chash_mx_get(&m, 5)->value;
    chash_mx_put(&m, 5, 'd'); // update
    chash_mx_erase(&m, 8);
    c_foreach (i, chash_mx, m) printf("map %d: %c\n", i.item->key, i.item->value);
    chash_mx_destroy(&m);
}
*/

#ifndef CHASH__H__
#define CHASH__H__

#include <stdlib.h>
#include "cdefs.h"

#define chash_init          {NULL, NULL, 0, 0, 0.85f, 0.15f}
#define chash_size(map)     ((size_t) (map)._size)
#define chash_bucketCount(map)  ((size_t) (map)._cap)
/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)  ((uint32_t) (((uint64_t) (x) * (N)) >> 32))

enum {chash_HASH = 0x7f, chash_USED = 0x80};

/* CHash: */
#define declare_CHash(...) \
    c_MACRO_OVERLOAD(declare_CHash, __VA_ARGS__)

#define declare_CHash_3(tag, type, Key) \
    declare_CHash_4(tag, type, Key, void)

#define declare_CHash_4(tag, type, Key, Value) \
    declare_CHash_5(tag, type, Key, Value, c_emptyDestroy)

#define declare_CHash_5(tag, type, Key, Value, valueDestroy) \
    declare_CHash_6(tag, type, Key, Value, valueDestroy, c_defaultHash)

#define declare_CHash_6(tag, type, Key, Value, valueDestroy, keyHash) \
    declare_CHash_7(tag, type, Key, Value, valueDestroy, keyHash, c_defaultEquals)
    
#define declare_CHash_7(tag, type, Key, Value, valueDestroy, keyHash, keyEquals) \
    declare_CHash_11(tag, type, Key, Value, valueDestroy, keyHash, keyEquals, \
                          c_emptyDestroy, Key, c_defaultGetRaw, c_defaultInitRaw)

/* CHash<CString, Value>: */
#define declare_CHash_string(...) \
    c_MACRO_OVERLOAD(declare_CHash_string, __VA_ARGS__)

#define declare_CHash_string_2(tag, type) \
    declare_CHash_string_3(tag, type, void)

#define declare_CHash_string_3(tag, type, Value) \
    declare_CHash_string_4(tag, type, Value, c_emptyDestroy)

#define declare_CHash_string_4(tag, type, Value, valueDestroy) \
    declare_CHash_11(tag, type, CString, Value, valueDestroy, cstring_hashRaw, cstring_equalsRaw, \
                          cstring_destroy, const char*, cstring_getRaw, cstring_make)

#define _chash1_SET(x)
#define _chash2_SET(x, y) x
#define _chash1_MAP(x) x
#define _chash2_MAP(x, y) x, y

/* CHash full: */
#define declare_CHash_11(tag, type, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, \
                              keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
typedef struct CHashEntry_##tag { \
    Key key; \
    _chash1_##type(Value value;) \
} CHashEntry_##tag; \
 \
static inline CHashEntry_##tag chashentry_##tag##_make(_chash2_##type(Key k, Value v)) { \
    CHashEntry_##tag e = {_chash2_##type(k, v)}; return e; \
} \
static inline void \
chashentry_##tag##_destroy(CHashEntry_##tag* e) { \
    keyDestroy(&e->key); \
    _chash1_##type(valueDestroy(&e->value);) \
} \
 \
typedef RawKey CHashRawKey_##tag; \
 \
typedef struct CHash_##tag { \
    CHashEntry_##tag* _table; \
    uint8_t* _hashx; \
    uint32_t _size, _cap; \
    float maxLoadFactor; \
    float shrinkLimitFactor; \
} CHash_##tag; \
 \
typedef struct { \
    CHashEntry_##tag *item, *_end; \
    uint8_t* _hx; \
} CHashIter_##tag, chash_##tag##_iter_t; \
 \
typedef struct { \
    CHashRawKey_##tag rawKey; \
    size_t index; \
    uint32_t hashx; \
} CHashBucket_##tag; \
 \
STC_API CHash_##tag \
chash_##tag##_make(size_t initialSize); \
STC_API void \
chash_##tag##_destroy(CHash_##tag* self); \
STC_API void \
chash_##tag##_clear(CHash_##tag* self); \
STC_API void \
chash_##tag##_setLoadFactors(CHash_##tag* self, float maxLoadFactor, float shrinkLimitFactor); \
STC_API size_t \
chash_##tag##_bucket(const CHash_##tag* self, const CHashRawKey_##tag* rawKeyPtr, uint32_t* hxPtr); \
STC_API CHashEntry_##tag* \
chash_##tag##_get(const CHash_##tag* self, CHashRawKey_##tag rawKey); \
STC_API CHashEntry_##tag* \
chash_##tag##_put(CHash_##tag* self, _chash2_##type(CHashRawKey_##tag rawKey, Value value)); \
STC_API CHashEntry_##tag* \
chash_##tag##_find(CHash_##tag* self, CHashRawKey_##tag rawKey, CHashBucket_##tag* b); \
STC_API void \
chash_##tag##_insert(CHash_##tag* self, _chash2_##type(CHashBucket_##tag b, Value value)); \
STC_API size_t \
chash_##tag##_reserve(CHash_##tag* self, size_t size); \
STC_API bool \
chash_##tag##_eraseBucket(CHash_##tag* self, size_t i); \
STC_API bool \
chash_##tag##_erase(CHash_##tag* self, CHashRawKey_##tag rawKey); \
STC_API chash_##tag##_iter_t \
chash_##tag##_begin(CHash_##tag* map); \
STC_API chash_##tag##_iter_t \
chash_##tag##_next(chash_##tag##_iter_t it); \
 \
implement_CHash_11(tag, type, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, \
                        keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
typedef Key CHashKey_##tag; \
typedef Value CHashValue_##tag

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CHash_11(tag, type, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, \
                                keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
 \
STC_API CHash_##tag \
chash_##tag##_make(size_t initialSize) { \
    CHash_##tag h = chash_init; \
    chash_##tag##_reserve(&h, initialSize); \
    return h; \
} \
 \
STC_API void \
chash_##tag##_destroy(CHash_##tag* self) { \
    if (chash_size(*self)) { \
        size_t cap = chash_bucketCount(*self); \
        CHashEntry_##tag* e = self->_table, *end = e + cap; \
        uint8_t *hashx = self->_hashx; \
        for (; e != end; ++e) if (*hashx++) chashentry_##tag##_destroy(e); \
    } \
    free(self->_hashx); \
    free(self->_table); \
} \
 \
STC_API void chash_##tag##_clear(CHash_##tag* self) { \
    chash_##tag##_destroy(self); \
    self->_cap = self->_size = 0; \
} \
 \
STC_API void \
chash_##tag##_setLoadFactors(CHash_##tag* self, float maxLoadFactor, float shrinkLimitFactor) { \
    self->maxLoadFactor = maxLoadFactor; \
    self->shrinkLimitFactor = shrinkLimitFactor; \
} \
 \
STC_API size_t \
chash_##tag##_bucket(const CHash_##tag* self, const CHashRawKey_##tag* rawKeyPtr, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKeyPtr, sizeof(CHashRawKey_##tag)); \
    uint32_t sx, hx = (hash & chash_HASH) | chash_USED; \
    size_t cap = chash_bucketCount(*self); \
    size_t idx = chash_reduce(hash, cap); \
    uint8_t* hashx = self->_hashx; \
    while ((sx = hashx[idx])) { \
        if (sx == hx) { \
            CHashRawKey_##tag r = keyGetRaw(&self->_table[idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
STC_API CHashEntry_##tag* \
chash_##tag##_get(const CHash_##tag* self, CHashRawKey_##tag rawKey) { \
    if (chash_bucketCount(*self) == 0) return NULL; \
    uint32_t hx; \
    size_t idx = chash_##tag##_bucket(self, &rawKey, &hx); \
    return self->_hashx[idx] ? &self->_table[idx] : NULL; \
} \
 \
static inline void _chash_##tag##_reserveExpand(CHash_##tag* self) { \
    if (chash_size(*self) + 1 >= chash_bucketCount(*self) * self->maxLoadFactor) \
        chash_##tag##_reserve(self, (size_t) 7 + (1.6 * chash_bucketCount(*self))); \
} \
 \
STC_API CHashEntry_##tag* \
chash_##tag##_put(CHash_##tag* self, _chash2_##type(CHashRawKey_##tag rawKey, Value value)) { \
    _chash_##tag##_reserveExpand(self); \
    uint32_t hx; \
    size_t idx = chash_##tag##_bucket(self, &rawKey, &hx); \
    CHashEntry_##tag* e = &self->_table[idx]; \
    if (self->_hashx[idx]) \
        _chash1_##type(valueDestroy(&e->value)) ; \
    else { \
        e->key = keyInitRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->_size; \
    } \
    _chash1_##type(e->value = value;) \
    return e; \
} \
 \
STC_API CHashEntry_##tag* \
chash_##tag##_find(CHash_##tag* self, CHashRawKey_##tag rawKey, CHashBucket_##tag* b) { \
    _chash_##tag##_reserveExpand(self); \
    b->rawKey = rawKey; \
    b->index = chash_##tag##_bucket(self, &rawKey, &b->hashx); \
    return self->_hashx[b->index] ? &self->_table[b->index] : NULL; \
} \
 \
STC_API void \
chash_##tag##_insert(CHash_##tag* self, _chash2_##type(CHashBucket_##tag b, Value value)) { \
    CHashEntry_##tag* e = &self->_table[b.index]; \
    if (self->_hashx[b.index]) \
        _chash1_##type(valueDestroy(&e->value)) ; \
    else { \
        e->key = keyInitRaw(b.rawKey); \
        self->_hashx[b.index] = (uint8_t) b.hashx; \
        ++self->_size; \
    } \
    _chash1_##type(e->value = value;) \
} \
 \
static inline void \
chash_##tag##_swap(CHash_##tag* a, CHash_##tag* b) { \
    c_swap(CHash_##tag, *a, *b); \
} \
 \
STC_API size_t \
chash_##tag##_reserve(CHash_##tag* self, size_t newcap) { \
    size_t oldcap = chash_bucketCount(*self); newcap |= 1; \
    if (chash_size(*self) >= newcap * self->maxLoadFactor) return oldcap; \
    CHash_##tag tmp = { \
        c_new_N(CHashEntry_##tag, newcap), \
        (uint8_t *) calloc(newcap, sizeof(uint8_t)), \
        self->_size, (uint32_t) newcap, \
        self->maxLoadFactor, self->shrinkLimitFactor \
    }; \
    chash_##tag##_swap(self, &tmp); \
 \
    CHashEntry_##tag* e = tmp._table, *slot = self->_table; \
    uint8_t* hashx = self->_hashx; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (tmp._hashx[i]) { \
            CHashRawKey_##tag r = keyGetRaw(&e->key); \
            size_t idx = chash_##tag##_bucket(self, &r, &hx); \
            slot[idx] = *e, \
            hashx[idx] = (uint8_t) hx; \
        } \
    free(tmp._hashx); \
    free(tmp._table); \
    return newcap; \
} \
 \
STC_API bool \
chash_##tag##_eraseBucket(CHash_##tag* self, size_t i) { \
    size_t j = i, k, cap = chash_bucketCount(*self); \
    CHashEntry_##tag* slot = self->_table; \
    uint8_t* hashx = self->_hashx; \
    CHashRawKey_##tag r; \
    if (! hashx[i]) \
        return false; \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
        if (! hashx[j]) \
            break; \
        r = keyGetRaw(&slot[j].key); \
        k = chash_reduce(keyHashRaw(&r, sizeof(CHashRawKey_##tag)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
    } while (true); \
    hashx[i] = 0; \
    chashentry_##tag##_destroy(&slot[i]); \
    --self->_size; \
    return true; \
} \
 \
STC_API bool \
chash_##tag##_erase(CHash_##tag* self, CHashRawKey_##tag rawKey) { \
    if (chash_size(*self) == 0) \
        return false; \
    size_t cap = chash_bucketCount(*self); \
    if (chash_size(*self) < cap * self->shrinkLimitFactor && cap * sizeof(CHashEntry_##tag) > 1024) \
        chash_##tag##_reserve(self, (size_t) (chash_size(*self) * 1.2f / self->maxLoadFactor)); \
    uint32_t hx; \
    size_t i = chash_##tag##_bucket(self, &rawKey, &hx); \
    return chash_##tag##_eraseBucket(self, i); \
} \
 \
STC_API chash_##tag##_iter_t \
chash_##tag##_begin(CHash_##tag* map) { \
    uint8_t* hx = map->_hashx; \
    CHashEntry_##tag* e = map->_table, *end = e + chash_bucketCount(*map); \
    while (e != end && !*hx) ++e, ++hx; \
    chash_##tag##_iter_t it = {e == end ? NULL : e, end, hx}; return it; \
} \
 \
STC_API chash_##tag##_iter_t \
chash_##tag##_next(chash_##tag##_iter_t it) { \
    do { ++it.item, ++it._hx; } while (it.item != it._end && !*it._hx); \
    if (it.item == it._end) it.item = NULL; \
    return it; \
}

#else
#define implement_CHash_11(tag, type, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, \
                                keyDestroy, RawKey, keyGetRaw, keyInitRaw)
#endif

#endif
