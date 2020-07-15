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
#include <stc/chash.h>
declare_CHash(sx, int);       // Set of int
declare_CHash(mx, int, char); // Map of int -> char

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

#define chash_init                       {NULL, NULL, 0, 0, 0.85f, 0.15f}
#define chash_size(map)                  ((size_t) (map).size)
#define chash_bucketCount(map)           ((size_t) (map).buckets)
#define chash_entryIndex(map, entryPtr)  ((entryPtr) - (map).table)
/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)               ((uint32_t) (((uint64_t) (x) * (N)) >> 32))

enum {chash_HASH = 0x7f, chash_USED = 0x80};

#define declare_CHash(...) \
    c_MACRO_OVERLOAD(declare_CHash, __VA_ARGS__)

#define declare_CHash_2(tag, Key) \
    declare_CHash_set_2(tag, Key)

#define declare_CHash_3(tag, Key, Value) \
    declare_CHash_6(tag, Key, Value, c_emptyDestroy, c_defaultHash, c_defaultEquals)

#define declare_CHash_4(tag, Key, Value, valueDestroy) \
    declare_CHash_6(tag, Key, Value, valueDestroy, c_defaultHash, c_defaultEquals)

#define declare_CHash_5(tag, Key, Value, valueDestroy, keyHash) \
    declare_CHash_6(tag, Key, Value, valueDestroy, keyHash, c_defaultEquals)

#define declare_CHash_6(tag, Key, Value, valueDestroy, keyHash, keyEquals) \
    declare_CHash_11(tag, MAP, Key, Value, valueDestroy, keyHash, keyEquals, \
                          c_emptyDestroy, Key, c_defaultGetRaw, c_defaultInitRaw)

/* CHash_set: */
#define declare_CHash_set(...) \
    c_MACRO_OVERLOAD(declare_CHash_set, __VA_ARGS__)

#define declare_CHash_set_2(tag, Key) \
    declare_CHash_set_5(tag, Key, c_emptyDestroy, c_defaultHash, c_defaultEquals)

#define declare_CHash_set_3(tag, Key, keyDestroy) \
    declare_CHash_set_5(tag, Key, keyDestroy, c_defaultHash, c_defaultEquals)

#define declare_CHash_set_4(tag, Key, keyDestroy, keyHash) \
    declare_CHash_set_5(tag, Key, keyDestroy, keyHash, c_defaultEquals)

#define declare_CHash_set_5(tag, Key, keyDestroy, keyHash, keyEquals) \
    declare_CHash_11(tag, SET, Key, void, void, keyHash, keyEquals, \
                          keyDestroy, Key, c_defaultGetRaw, c_defaultInitRaw)

/* CHash_string: */
#define declare_CHash_string(...) \
    c_MACRO_OVERLOAD(declare_CHash_string, __VA_ARGS__)

#define declare_CHash_string_1(tag) \
    declare_CHash_string_4(tag, SET, void, void)

#define declare_CHash_string_2(tag, Value) \
    declare_CHash_string_4(tag, MAP, Value, c_emptyDestroy)

#define declare_CHash_string_3(tag, Value, ValueDestroy) \
    declare_CHash_string_4(tag, MAP, Value, ValueDestroy)

#define declare_CHash_string_4(tag, type, Value, valueDestroy) \
    declare_CHash_11(tag, type, CString, Value, valueDestroy, cstring_hashRaw, cstring_equalsRaw, \
                          cstring_destroy, const char*, cstring_getRaw, cstring_make)

#define _chash1_SET(x)
#define _chash2_SET(x, y) x
#define _chash1_MAP(x) x
#define _chash2_MAP(x, y) x, y

/* CHash full: use 'void' for Value if type is SET */
#define declare_CHash_11(tag, type, Key, Value, valueDestroy, keyHashRaw, keyEqualsRaw, \
                              keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
typedef struct CHashEntry_##tag { \
    Key key; \
    _chash1_##type(Value value;) \
} CHashEntry_##tag; \
 \
STC_INLINE void \
chashentry_##tag##_destroy(CHashEntry_##tag* e) { \
    keyDestroy(&e->key); \
    _chash1_##type(valueDestroy(&e->value);) \
} \
 \
typedef RawKey CHashRawKey_##tag; \
 \
typedef struct CHash_##tag { \
    CHashEntry_##tag* table; \
    uint8_t* _hashx; \
    uint32_t size, buckets; \
    float maxLoadFactor; \
    float shrinkLimitFactor; \
} CHash_##tag; \
 \
typedef struct { \
    CHashEntry_##tag *item, *_end; \
    uint8_t* _hx; \
} CHashIter_##tag, chash_##tag##_iter_t; \
 \
STC_API CHash_##tag \
chash_##tag##_make(size_t initialSize); \
STC_API void \
chash_##tag##_destroy(CHash_##tag* self); \
STC_API void \
chash_##tag##_clear(CHash_##tag* self); \
STC_API void \
chash_##tag##_setLoadFactors(CHash_##tag* self, float maxLoadFactor, float shrinkLimitFactor); \
STC_API CHashEntry_##tag* \
chash_##tag##_get(const CHash_##tag* self, CHashRawKey_##tag rawKey); \
STC_API CHashEntry_##tag* \
chash_##tag##_put(CHash_##tag* self, _chash2_##type(CHashRawKey_##tag rawKey, Value value)); \
_chash1_##type( STC_API CHashEntry_##tag* \
chash_##tag##_at(CHash_##tag* self, CHashRawKey_##tag rawKey, Value initValue);) \
STC_INLINE void \
chash_##tag##_swap(CHash_##tag* a, CHash_##tag* b) { c_swap(CHash_##tag, *a, *b); } \
STC_API size_t \
chash_##tag##_reserve(CHash_##tag* self, size_t size); \
STC_API bool \
chash_##tag##_eraseEntry(CHash_##tag* self, CHashEntry_##tag* entry); \
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
        CHashEntry_##tag* e = self->table, *end = e + cap; \
        uint8_t *hashx = self->_hashx; \
        for (; e != end; ++e) if (*hashx++) chashentry_##tag##_destroy(e); \
    } \
    free(self->_hashx); \
    free(self->table); \
} \
 \
STC_API void chash_##tag##_clear(CHash_##tag* self) { \
    chash_##tag##_destroy(self); \
    self->buckets = self->size = 0; \
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
            CHashRawKey_##tag r = keyGetRaw(&self->table[idx].key); \
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
    return self->_hashx[idx] ? &self->table[idx] : NULL; \
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
    CHashEntry_##tag* e = &self->table[idx]; \
    if (self->_hashx[idx]) \
        _chash1_##type(valueDestroy(&e->value)) ; \
    else { \
        e->key = keyInitRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->size; \
    } \
    _chash1_##type(e->value = value;) \
    return e; \
} \
 \
_chash1_##type( \
STC_API CHashEntry_##tag* \
chash_##tag##_at(CHash_##tag* self, CHashRawKey_##tag rawKey, Value initValue) { \
    _chash_##tag##_reserveExpand(self); \
    uint32_t hx; \
    size_t idx = chash_##tag##_bucket(self, &rawKey, &hx); \
    CHashEntry_##tag* e = &self->table[idx]; \
    if (! self->_hashx[idx]) { \
        e->key = keyInitRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->size; \
        e->value = initValue; \
    } \
    return e; \
}) \
 \
STC_API size_t \
chash_##tag##_reserve(CHash_##tag* self, size_t newcap) { \
    size_t oldcap = chash_bucketCount(*self); newcap |= 1; \
    if (chash_size(*self) >= newcap * self->maxLoadFactor) return oldcap; \
    CHash_##tag tmp = { \
        c_new_N(CHashEntry_##tag, newcap), \
        (uint8_t *) calloc(newcap, sizeof(uint8_t)), \
        self->size, (uint32_t) newcap, \
        self->maxLoadFactor, self->shrinkLimitFactor \
    }; \
    chash_##tag##_swap(self, &tmp); \
 \
    CHashEntry_##tag* e = tmp.table, *slot = self->table; \
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
    free(tmp.table); \
    return newcap; \
} \
 \
STC_API bool \
chash_##tag##_eraseEntry(CHash_##tag* self, CHashEntry_##tag* entry) { \
    size_t i = chash_entryIndex(*self, entry), j = i, k, cap = chash_bucketCount(*self); \
    CHashEntry_##tag* slot = self->table; \
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
    --self->size; \
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
    return chash_##tag##_eraseEntry(self, self->table + i); \
} \
 \
STC_API chash_##tag##_iter_t \
chash_##tag##_begin(CHash_##tag* map) { \
    uint8_t* hx = map->_hashx; \
    CHashEntry_##tag* e = map->table, *end = e + chash_bucketCount(*map); \
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
