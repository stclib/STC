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
#include <stc/cmap.h>
declare_cset(sx, int);       // Set of int
declare_cmap(mx, int, char); // Map of int -> char

int main(void) {
    cset_sx s = cset_init;
    cset_sx_put(&s, 5);
    cset_sx_put(&s, 8);
    c_foreach (i, cset_sx, s) printf("set %d\n", i.item->key);
    cset_sx_destroy(&s);

    cmap_mx m = cmap_init;
    cmap_mx_put(&m, 5, 'a');
    cmap_mx_put(&m, 8, 'b');
    cmap_mx_put(&m, 12, 'c');
    cmapentry_mx *e = cmap_mx_find(&m, 10); // = NULL
    char val = cmap_mx_find(&m, 5)->value;
    cmap_mx_put(&m, 5, 'd'); // update
    cmap_mx_erase(&m, 8);
    c_foreach (i, cmap_mx, m) printf("map %d: %c\n", i.item->key, i.item->value);
    cmap_mx_destroy(&m);
}
*/
#ifndef CMAP__H__
#define CMAP__H__

#include <stdlib.h>
#include <string.h>
#include "cdefs.h"

#define cmap_init                     {NULL, NULL, 0, 0, 0.85f, 0.15f}
#define cmap_size(m)                  ((size_t) (m).size)
#define cset_init                     cmap_init                    
#define cset_size(s)                  cmap_size(s)                 

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};

#define declare_cmap(...) \
    c_MACRO_OVERLOAD(declare_cmap, __VA_ARGS__)

#define declare_cmap_3(tag, Key, Value) \
    declare_cmap_4(tag, Key, Value, c_default_destroy)

#define declare_cmap_4(tag, Key, Value, valueDestroy) \
    declare_cmap_6(tag, Key, Value, valueDestroy, c_default_equals, c_defaultHash)

#define declare_cmap_6(tag, Key, Value, valueDestroy, keyEquals, keyHash) \
    declare_cmap_10(tag, Key, Value, valueDestroy, keyEquals, keyHash, \
                         c_default_destroy, Key, c_defaultGetRaw, c_defaultInitRaw)

#define declare_cmap_10(tag, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
    declare_CHASH(tag, cmap, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyGetRaw, keyInitRaw)

/* cset: */
#define declare_cset(...) \
    c_MACRO_OVERLOAD(declare_cset, __VA_ARGS__)

#define declare_cset_2(tag, Key) \
    declare_cset_4(tag, Key, c_default_equals, c_defaultHash)

#define declare_cset_4(tag, Key, keyEquals, keyHash) \
    declare_cset_5(tag, Key, keyEquals, keyHash, c_default_destroy)

#define declare_cset_5(tag, Key, keyEquals, keyHash, keyDestroy) \
    declare_cset_8(tag, Key, keyEquals, keyHash, keyDestroy, \
                        Key, c_defaultGetRaw, c_defaultInitRaw)

#define declare_cset_8(tag, Key, keyEqualsRaw, keyHashRaw, keyDestroy, \
                            RawKey, keyGetRaw, keyInitRaw) \
    declare_CHASH(tag, cset, Key, void, void, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyGetRaw, keyInitRaw)

/* cset_str, cmap_str: */
#define declare_cset_str() \
    declare_CHASH_STR(str, cset, void, void)

#define declare_cmap_str(...) \
    c_MACRO_OVERLOAD(declare_cmap_str, __VA_ARGS__)

#define declare_cmap_str_2(tag, Value) \
    declare_CHASH_STR(tag, cmap, Value, c_default_destroy)

#define declare_cmap_str_3(tag, Value, ValueDestroy) \
    declare_CHASH_STR(tag, cmap, Value, ValueDestroy)

#define declare_CHASH_STR(tag, ctype, Value, valueDestroy) \
    declare_CHASH(tag, ctype, cstr_t, Value, valueDestroy, cstr_equalsRaw, cstr_hashRaw, \
                       cstr_destroy, const char*, cstr_getRaw, cstr_make)

#define OPT_1_cset(x)
#define OPT_2_cset(x, y) x
#define OPT_1_cmap(x) x
#define OPT_2_cmap(x, y) x, y

/* CHASH full: use 'void' for Value if ctype is cset */
#define declare_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                           keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
typedef struct { \
    Key key; \
    OPT_1_##ctype(Value value;) \
} ctype##entry_##tag, ctype##_##tag##_entry_t; \
 \
STC_INLINE void \
ctype##entry_##tag##_destroy(ctype##entry_##tag* e) { \
    keyDestroy(&e->key); \
    OPT_1_##ctype(valueDestroy(&e->value);) \
} \
typedef struct { \
    RawKey key; \
    OPT_1_##ctype(Value value;) \
} ctype##_##tag##_input_t; \
 \
typedef RawKey ctype##_##tag##_rawkey_t; \
 \
typedef struct { \
    ctype##entry_##tag* table; \
    uint8_t* _hashx; \
    uint32_t size, bucket_count; \
    float max_load_factor; \
    float shrink_limit_factor; \
} ctype##_##tag; \
 \
typedef struct { \
    ctype##entry_##tag *item, *_end; \
    uint8_t* _hx; \
} ctype##_##tag##_iter_t; \
 \
STC_INLINE ctype##_##tag \
ctype##_##tag##_init(void) {ctype##_##tag x = cmap_init; return x;} \
STC_INLINE size_t \
ctype##_##tag##_size(ctype##_##tag m) {return m.size;} \
STC_API ctype##_##tag \
ctype##_##tag##_make(size_t initialSize); \
STC_API void \
ctype##_##tag##_push_n(ctype##_##tag* self, const ctype##_##tag##_input_t in[], size_t size); \
STC_API void \
ctype##_##tag##_destroy(ctype##_##tag* self); \
STC_API void \
ctype##_##tag##_clear(ctype##_##tag* self); \
STC_INLINE void \
ctype##_##tag##_set_load_factors(ctype##_##tag* self, float max, float shrink) { \
    self->max_load_factor = max; self->shrink_limit_factor = shrink; \
} \
STC_API ctype##entry_##tag* \
ctype##_##tag##_find(const ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey); \
STC_FORCE_INLINE ctype##entry_##tag* /* alias */ \
ctype##_##tag##_get(const ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) \
    {return ctype##_##tag##_find(self, rawKey);} \
STC_API ctype##entry_##tag* /* similar to c++ std::map.insert_or_assign(): */ \
ctype##_##tag##_put(ctype##_##tag* self, OPT_2_##ctype(ctype##_##tag##_rawkey_t rawKey, Value value)); \
OPT_1_##ctype(STC_API ctype##entry_##tag* /* similar to c++ std::map.operator[](): */ \
ctype##_##tag##_insert(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey, Value initValue);) \
STC_INLINE void \
ctype##_##tag##_swap(ctype##_##tag* a, ctype##_##tag* b) { c_swap(ctype##_##tag, *a, *b); } \
STC_API size_t \
ctype##_##tag##_reserve(ctype##_##tag* self, size_t size); \
STC_API bool \
ctype##_##tag##_erase_entry(ctype##_##tag* self, ctype##entry_##tag* entry); \
STC_API bool \
ctype##_##tag##_erase(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey); \
STC_API ctype##_##tag##_iter_t \
ctype##_##tag##_begin(ctype##_##tag* map); \
STC_API ctype##_##tag##_iter_t \
ctype##_##tag##_next(ctype##_##tag##_iter_t it); \
 \
implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                     keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
typedef Key ctype##_##tag##_key_t; \
typedef Value ctype##_##tag##_value_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyGetRaw, keyInitRaw) \
 STC_API ctype##_##tag \
ctype##_##tag##_make(size_t initialSize) { \
    ctype##_##tag h = ctype##_init; \
    ctype##_##tag##_reserve(&h, initialSize); \
    return h; \
} \
STC_API void \
ctype##_##tag##_push_n(ctype##_##tag* self, const ctype##_##tag##_input_t in[], size_t size) { \
    for (size_t i=0; i<size; ++i) ctype##_##tag##_put(self, OPT_2_##ctype(in[i].key, in[i].value)); \
} \
 \
STC_INLINE void ctype##_##tag##_wipe_(ctype##_##tag* self) { \
    if (self->size == 0) return; \
    ctype##entry_##tag* e = self->table, *end = e + self->bucket_count; \
    uint8_t *hx = self->_hashx; \
    for (; e != end; ++e) if (*hx++) ctype##entry_##tag##_destroy(e); \
} \
 \
STC_API void ctype##_##tag##_destroy(ctype##_##tag* self) { \
    ctype##_##tag##_wipe_(self); \
    free(self->_hashx); \
    free(self->table); \
} \
 \
STC_API void ctype##_##tag##_clear(ctype##_##tag* self) { \
    ctype##_##tag##_wipe_(self); \
    self->size = 0; \
    memset(self->_hashx, 0, self->bucket_count); \
} \
 \
STC_API size_t \
ctype##_##tag##_bucket(const ctype##_##tag* self, const ctype##_##tag##_rawkey_t* rawKeyPtr, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKeyPtr, sizeof(ctype##_##tag##_rawkey_t)); \
    uint32_t sx, hx = (hash & chash_HASH) | chash_USED; \
    size_t cap = self->bucket_count; \
    size_t idx = chash_reduce(hash, cap); \
    uint8_t* hashx = self->_hashx; \
    while ((sx = hashx[idx])) { \
        if (sx == hx) { \
            ctype##_##tag##_rawkey_t r = keyGetRaw(&self->table[idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
STC_API ctype##entry_##tag* \
ctype##_##tag##_find(const ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) { \
    if (self->size == 0) return NULL; \
    uint32_t hx; \
    size_t idx = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    return self->_hashx[idx] ? &self->table[idx] : NULL; \
} \
 \
static inline void ctype##_##tag##_reserve_expand(ctype##_##tag* self) { \
    if (self->size >= self->bucket_count * self->max_load_factor) \
        ctype##_##tag##_reserve(self, 4 + self->size * 3 / 2); \
} \
 \
STC_API ctype##entry_##tag* \
ctype##_##tag##_put(ctype##_##tag* self, OPT_2_##ctype(ctype##_##tag##_rawkey_t rawKey, Value value)) { \
    ctype##_##tag##_reserve_expand(self); \
    uint32_t hx; \
    size_t idx = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    ctype##entry_##tag* e = &self->table[idx]; \
    if (self->_hashx[idx]) \
        OPT_1_##ctype(valueDestroy(&e->value)) ; \
    else { \
        e->key = keyInitRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->size; \
    } \
    OPT_1_##ctype(e->value = value;) \
    return e; \
} \
 \
OPT_1_##ctype( \
STC_API ctype##entry_##tag* \
ctype##_##tag##_insert(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey, Value initValue) { \
    ctype##_##tag##_reserve_expand(self); \
    uint32_t hx; \
    size_t idx = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    ctype##entry_##tag* e = &self->table[idx]; \
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
ctype##_##tag##_reserve(ctype##_##tag* self, size_t newcap) { \
    size_t oldcap = self->bucket_count; \
    if (self->size > newcap) return oldcap; \
    newcap /= self->max_load_factor; newcap |= 1; \
    ctype##_##tag tmp = { \
        c_new_n(ctype##entry_##tag, newcap), \
        (uint8_t *) calloc(newcap, sizeof(uint8_t)), \
        self->size, (uint32_t) newcap, \
        self->max_load_factor, self->shrink_limit_factor \
    }; \
    ctype##_##tag##_swap(self, &tmp); \
 \
    ctype##entry_##tag* e = tmp.table, *slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (tmp._hashx[i]) { \
            ctype##_##tag##_rawkey_t r = keyGetRaw(&e->key); \
            size_t idx = ctype##_##tag##_bucket(self, &r, &hx); \
            slot[idx] = *e, \
            hashx[idx] = (uint8_t) hx; \
        } \
    free(tmp._hashx); \
    free(tmp.table); \
    return newcap; \
} \
 \
STC_API bool \
ctype##_##tag##_erase_entry(ctype##_##tag* self, ctype##entry_##tag* entry) { \
    size_t i = chash_entry_index(*self, entry), j = i, k, cap = self->bucket_count; \
    ctype##entry_##tag* slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    ctype##_##tag##_rawkey_t r; \
    if (! hashx[i]) \
        return false; \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
        if (! hashx[j]) \
            break; \
        r = keyGetRaw(&slot[j].key); \
        k = chash_reduce(keyHashRaw(&r, sizeof(ctype##_##tag##_rawkey_t)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
    } while (true); \
    hashx[i] = 0; \
    ctype##entry_##tag##_destroy(&slot[i]); \
    --self->size; \
    return true; \
} \
 \
STC_API bool \
ctype##_##tag##_erase(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) { \
    if (self->size == 0) \
        return false; \
    size_t cap = self->bucket_count; \
    if (self->size < cap * self->shrink_limit_factor && cap * sizeof(ctype##entry_##tag) > 1024) \
        ctype##_##tag##_reserve(self, self->size * 6 / 5); \
    uint32_t hx; \
    size_t i = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    return ctype##_##tag##_erase_entry(self, self->table + i); \
} \
 \
STC_API ctype##_##tag##_iter_t \
ctype##_##tag##_begin(ctype##_##tag* map) { \
    uint8_t* hx = map->_hashx; \
    ctype##entry_##tag* e = map->table, *end = e + map->bucket_count; \
    while (e != end && !*hx) ++e, ++hx; \
    ctype##_##tag##_iter_t it = {e == end ? NULL : e, end, hx}; return it; \
} \
 \
STC_API ctype##_##tag##_iter_t \
ctype##_##tag##_next(ctype##_##tag##_iter_t it) { \
    do { ++it.item, ++it._hx; } while (it.item != it._end && !*it._hx); \
    if (it.item == it._end) it.item = NULL; \
    return it; \
}

#else
#define implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyGetRaw, keyInitRaw)
#endif

#endif
