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
    cmap_mx_entry_t *e = cmap_mx_find(&m, 10); // = NULL
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
#define cmap_empty(m)                 ((m).size == 0)
#define cmap_size(m)                  ((size_t) (m).size)
#define cmap_bucket_count(m)          ((size_t) (m).bucket_count)
#define cset_init                     cmap_init
#define cset_size(s)                  cmap_size(s)
#define cset_bucket_count(s)          cmap_bucket_count(s)
#define cmap_try_emplace(tag, self, k, v) do { \
    struct cmap_##tag##_result __r = cmap_##tag##_insert_key(self, k); \
    if (__r.inserted) __r.entry->value = v; \
} while (false)

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};

#define declare_cmap(...) \
    c_MACRO_OVERLOAD(declare_cmap, __VA_ARGS__)

#define declare_cmap_3(tag, Key, Value) \
    declare_cmap_4(tag, Key, Value, c_default_destroy)

#define declare_cmap_4(tag, Key, Value, valueDestroy) \
    declare_cmap_6(tag, Key, Value, valueDestroy, c_default_equals, c_default_hash16)

#define declare_cmap_6(tag, Key, Value, valueDestroy, keyEquals, keyHash) \
    declare_cmap_10(tag, Key, Value, valueDestroy, keyEquals, keyHash, \
                         c_default_destroy, Key, c_default_to_raw, c_default_from_raw)

#define declare_cmap_10(tag, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw) \
    declare_CHASH(tag, cmap, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyToRaw, keyFromRaw, Value, c_default_from_raw)

/* cset: */
#define declare_cset(...) \
    c_MACRO_OVERLOAD(declare_cset, __VA_ARGS__)

#define declare_cset_2(tag, Key) \
    declare_cset_4(tag, Key, c_default_equals, c_default_hash16)

#define declare_cset_4(tag, Key, keyEquals, keyHash) \
    declare_cset_5(tag, Key, keyEquals, keyHash, c_default_destroy)

#define declare_cset_5(tag, Key, keyEquals, keyHash, keyDestroy) \
    declare_cset_8(tag, Key, keyEquals, keyHash, keyDestroy, \
                        Key, c_default_to_raw, c_default_from_raw)

#define declare_cset_8(tag, Key, keyEqualsRaw, keyHashRaw, keyDestroy, \
                            RawKey, keyToRaw, keyFromRaw) \
    declare_CHASH(tag, cset, Key, Key, void, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyToRaw, keyFromRaw, void, c_default_from_raw)

/* cset_str, cmap_str, cmap_strkey, cmap_strval: */
#define declare_cset_str() \
    declare_CHASH_strkey(str, cset, cstr_t, void)

#define declare_cmap_str() \
    declare_CHASH(str, cmap, cstr_t, cstr_t, cstr_destroy, cstr_equals_raw, cstr_hash_raw, \
                       cstr_destroy, const char*, cstr_to_raw, cstr_make, const char*, cstr_make)

#define declare_cmap_strkey(...) \
    c_MACRO_OVERLOAD(declare_cmap_strkey, __VA_ARGS__)

#define declare_cmap_strkey_2(tag, Value) \
    declare_CHASH_strkey(tag, cmap, Value, c_default_destroy)

#define declare_cmap_strkey_3(tag, Value, ValueDestroy) \
    declare_CHASH_strkey(tag, cmap, Value, ValueDestroy)

#define declare_cmap_strval(...) \
    c_MACRO_OVERLOAD(declare_cmap_strval, __VA_ARGS__)

#define declare_cmap_strval_2(tag, Key) \
    declare_cmap_strval_4(tag, Key, c_default_equals, c_default_hash16)

#define declare_cmap_strval_4(tag, Key, keyEquals, keyHash) \
    declare_CHASH(tag, cmap, Key, cstr_t, cstr_destroy, keyEquals, keyHash, \
                       c_default_destroy, Key, c_default_to_raw, c_default_from_raw, const char*, cstr_make)

#define declare_CHASH_strkey(tag, ctype, Value, valueDestroy) \
    declare_CHASH(tag, ctype, cstr_t, Value, valueDestroy, cstr_equals_raw, cstr_hash_raw, \
                       cstr_destroy, const char*, cstr_to_raw, cstr_make, Value, c_default_from_raw)

#define CMAP_ONLY_cset(x)
#define CMAP_ONLY_cmap(x) x
#define CMAP_BOTH_cset(x, y) x
#define CMAP_BOTH_cmap(x, y) x, y

/* CHASH full: use 'void' for Value if ctype is cset */
#define declare_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                           keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw) \
typedef struct { \
    Key key; \
    CMAP_ONLY_##ctype(Value value;) \
} ctype##_##tag##_entry_t; \
 \
STC_INLINE void \
ctype##_##tag##_entry_destroy(ctype##_##tag##_entry_t* e) { \
    keyDestroy(&e->key); \
    CMAP_ONLY_##ctype(valueDestroy(&e->value);) \
} \
typedef struct { \
    RawKey key; \
    CMAP_ONLY_##ctype(RawValue value;) \
} ctype##_##tag##_input_t; \
 \
typedef RawKey ctype##_##tag##_rawkey_t; \
typedef RawValue ctype##_##tag##_rawvalue_t; \
 \
typedef struct ctype##_##tag { \
    ctype##_##tag##_entry_t* table; \
    uint8_t* _hashx; \
    uint32_t size, bucket_count; \
    float max_load_factor; \
    float shrink_limit_factor; \
} ctype##_##tag; \
 \
typedef struct { \
    ctype##_##tag##_entry_t *item, *end; \
    uint8_t* _hx; \
} ctype##_##tag##_iter_t; \
 \
STC_INLINE ctype##_##tag \
ctype##_##tag##_init(void) {ctype##_##tag m = cmap_init; return m;} \
STC_INLINE bool \
ctype##_##tag##_empty(ctype##_##tag m) {return m.size == 0;} \
STC_INLINE size_t \
ctype##_##tag##_size(ctype##_##tag m) {return (size_t) m.size;} \
STC_INLINE size_t \
ctype##_##tag##_capacity(ctype##_##tag m) {return (size_t) (m.bucket_count * m.max_load_factor);} \
STC_INLINE void \
ctype##_##tag##_swap(ctype##_##tag* a, ctype##_##tag* b) {c_swap(ctype##_##tag, *a, *b);} \
STC_INLINE void \
ctype##_##tag##_set_load_factors(ctype##_##tag* self, float max, float shrink) { \
    self->max_load_factor = max; self->shrink_limit_factor = shrink; \
} \
STC_API ctype##_##tag \
ctype##_##tag##_with_capacity(size_t cap); \
STC_API void \
ctype##_##tag##_push_n(ctype##_##tag* self, const ctype##_##tag##_input_t in[], size_t size); \
STC_API void \
ctype##_##tag##_destroy(ctype##_##tag* self); \
STC_API void \
ctype##_##tag##_clear(ctype##_##tag* self); \
STC_API ctype##_##tag##_entry_t* \
ctype##_##tag##_find(const ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey); \
 \
struct ctype##_##tag##_result {ctype##_##tag##_entry_t *entry; bool inserted;}; \
STC_API struct ctype##_##tag##_result \
ctype##_##tag##_insert_key(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey); \
 \
STC_INLINE ctype##_##tag##_entry_t* /* like c++ std::map.insert(): */ \
ctype##_##tag##_insert(ctype##_##tag* self, CMAP_BOTH_##ctype(ctype##_##tag##_rawkey_t rawKey, RawValue rawValue)) { \
    struct ctype##_##tag##_result res = ctype##_##tag##_insert_key(self, rawKey); \
    CMAP_ONLY_##ctype( if (res.inserted) res.entry->value = valueFromRaw(rawValue); ) \
    return res.entry; \
} \
 \
STC_INLINE ctype##_##tag##_entry_t* /* like c++ std::map.insert_or_assign(): */ \
ctype##_##tag##_put(ctype##_##tag* self, CMAP_BOTH_##ctype(ctype##_##tag##_rawkey_t rawKey, RawValue rawValue)) { \
    struct ctype##_##tag##_result res = ctype##_##tag##_insert_key(self, rawKey); \
    CMAP_ONLY_##ctype( if (!res.inserted) valueDestroy(&res.entry->value); \
                       res.entry->value = valueFromRaw(rawValue); ) \
    return res.entry; \
} \
CMAP_ONLY_##ctype( \
STC_INLINE ctype##_##tag##_entry_t* /* cmap_put_v(key, move(value)) */ \
ctype##_##tag##_put_v(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey, Value value) { \
    struct ctype##_##tag##_result res = ctype##_##tag##_insert_key(self, rawKey); \
    if (!res.inserted) valueDestroy(&res.entry->value); \
    res.entry->value = value; \
    return res.entry; \
} \
) /* end CMAP_ONLY */ \
STC_API size_t \
ctype##_##tag##_reserve(ctype##_##tag* self, size_t size); \
STC_API bool \
ctype##_##tag##_erase_entry(ctype##_##tag* self, ctype##_##tag##_entry_t* entry); \
STC_API bool \
ctype##_##tag##_erase(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey); \
 \
STC_FORCE_INLINE ctype##_##tag##_iter_t \
ctype##_##tag##_begin(ctype##_##tag* map) { \
    ctype##_##tag##_iter_t it = {map->table, map->table + map->bucket_count, map->_hashx}; \
    if (it._hx) while (*it._hx == 0) ++it.item, ++it._hx; \
    return it; \
} \
STC_FORCE_INLINE void \
ctype##_##tag##_next(ctype##_##tag##_iter_t* it) { \
    while ((++it->item, *++it->_hx == 0)) ; \
} \
 \
STC_API uint32_t c_default_hash16(const void *data, size_t len); \
STC_API uint32_t c_default_hash32(const void* data, size_t len); \
 \
implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                     keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw) \
typedef Key ctype##_##tag##_key_t; \
typedef Value ctype##_##tag##_value_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw) \
 STC_API ctype##_##tag \
ctype##_##tag##_with_capacity(size_t cap) { \
    ctype##_##tag h = ctype##_init; \
    ctype##_##tag##_reserve(&h, cap); \
    return h; \
} \
STC_API void \
ctype##_##tag##_push_n(ctype##_##tag* self, const ctype##_##tag##_input_t in[], size_t size) { \
    for (size_t i=0; i<size; ++i) ctype##_##tag##_put(self, CMAP_BOTH_##ctype(in[i].key, in[i].value)); \
} \
 \
STC_INLINE void ctype##_##tag##_wipe_(ctype##_##tag* self) { \
    if (self->size == 0) return; \
    ctype##_##tag##_entry_t* e = self->table, *end = e + self->bucket_count; \
    uint8_t *hx = self->_hashx; \
    for (; e != end; ++e) if (*hx++) ctype##_##tag##_entry_destroy(e); \
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
            ctype##_##tag##_rawkey_t r = keyToRaw(&self->table[idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
STC_API ctype##_##tag##_entry_t* \
ctype##_##tag##_find(const ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) { \
    if (self->size == 0) return NULL; \
    uint32_t hx; \
    size_t idx = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    return self->_hashx[idx] ? &self->table[idx] : NULL; \
} \
 \
STC_INLINE void ctype##_##tag##_reserve_expand(ctype##_##tag* self) { \
    if (self->size + 1 >= self->bucket_count * self->max_load_factor) \
        ctype##_##tag##_reserve(self, 5 + self->size * 3 / 2); \
} \
 \
STC_API struct ctype##_##tag##_result \
ctype##_##tag##_insert_key(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) { \
    ctype##_##tag##_reserve_expand(self); \
    uint32_t hx; \
    size_t idx = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    struct ctype##_##tag##_result res = {&self->table[idx], !self->_hashx[idx]}; \
    if (res.inserted) { \
        res.entry->key = keyFromRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->size; \
    } \
    return res; \
} \
 \
STC_API size_t \
ctype##_##tag##_reserve(ctype##_##tag* self, size_t newcap) { \
    size_t oldcap = self->bucket_count; \
    if (self->size > newcap) return oldcap; \
    newcap = (size_t) (newcap / self->max_load_factor) | 1; \
    ctype##_##tag tmp = { \
        c_new_n(ctype##_##tag##_entry_t, newcap), \
        (uint8_t *) calloc(newcap + 1, sizeof(uint8_t)), \
        self->size, (uint32_t) newcap, \
        self->max_load_factor, self->shrink_limit_factor \
    }; \
    /* Rehash: */ \
    tmp._hashx[newcap] = 0xff; c_swap(ctype##_##tag, *self, tmp); \
    ctype##_##tag##_entry_t* e = tmp.table, *slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (tmp._hashx[i]) { \
            ctype##_##tag##_rawkey_t r = keyToRaw(&e->key); \
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
ctype##_##tag##_erase_entry(ctype##_##tag* self, ctype##_##tag##_entry_t* entry) { \
    size_t i = chash_entry_index(*self, entry), j = i, k, cap = self->bucket_count; \
    ctype##_##tag##_entry_t* slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    ctype##_##tag##_rawkey_t r; \
    if (! hashx[i]) \
        return false; \
    ctype##_##tag##_entry_destroy(&slot[i]); \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
        if (! hashx[j]) \
            break; \
        r = keyToRaw(&slot[j].key); \
        k = chash_reduce(keyHashRaw(&r, sizeof(ctype##_##tag##_rawkey_t)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
    } while (true); \
    hashx[i] = 0; \
    --self->size; \
    return true; \
} \
 \
STC_API bool \
ctype##_##tag##_erase(ctype##_##tag* self, ctype##_##tag##_rawkey_t rawKey) { \
    if (self->size == 0) \
        return false; \
    if (self->size < self->bucket_count * self->shrink_limit_factor && self->bucket_count * sizeof(ctype##_##tag##_entry_t) > 1024) \
        ctype##_##tag##_reserve(self, self->size * 6 / 5); \
    uint32_t hx; \
    size_t i = ctype##_##tag##_bucket(self, &rawKey, &hx); \
    return ctype##_##tag##_erase_entry(self, self->table + i); \
}

/* https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/ */

STC_API uint32_t c_default_hash16(const void *data, size_t len) {
    const volatile uint16_t *key = (const uint16_t *) data;
    uint64_t x = 0xc613fc15u;
    while (len -= 2) x = ((*key++ + x) * 2654435769ull) >> 13;
    return (uint32_t) x;
}
STC_API uint32_t c_default_hash32(const void* data, size_t len) {
    const volatile uint32_t *key = (const uint32_t *) data;
    uint64_t x = *key++ * 2654435769ull;
    while (len -= 4) x ^= *key++ * 2654435769ull;
    return (uint32_t) (x >> 24);
}

#else
#define implement_CHASH(tag, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw)
#endif

#endif
