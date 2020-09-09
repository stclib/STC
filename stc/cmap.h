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
    cset_sx s = cset_ini;
    cset_sx_put(&s, 5);
    cset_sx_put(&s, 8);
    c_foreach (i, cset_sx, s) printf("set %d\n", i.item->key);
    cset_sx_destroy(&s);

    cmap_mx m = cmap_ini;
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

#define cmap_ini                      {NULL, NULL, 0, 0, 0.85f, 0.15f}
#define cmap_empty(m)                 ((m).size == 0)
#define cmap_size(m)                  ((size_t) (m).size)
#define cmap_bucket_count(m)          ((size_t) (m).bucket_count)
#define cset_ini                      cmap_ini
#define cset_size(s)                  cmap_size(s)
#define cset_bucket_count(s)          cmap_bucket_count(s)
#define cmap_try_emplace(X, self, k, v) do { \
    cmap_##X##_result_t __r = cmap_##X##_insert_key_(self, k); \
    if (__r.inserted) __r.item->value = v; \
} while (false)

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};
typedef struct {size_t idx; uint32_t hx;} cmap_bucket_t, cset_bucket_t;

#define declare_cmap(...) \
    c_MACRO_OVERLOAD(declare_cmap, __VA_ARGS__)

#define declare_cmap_3(X, Key, Value) \
    declare_cmap_4(X, Key, Value, c_default_destroy)

#define declare_cmap_4(X, Key, Value, valueDestroy) \
    declare_cmap_6(X, Key, Value, valueDestroy, c_default_equals, c_default_hash16)

#define declare_cmap_6(X, Key, Value, valueDestroy, keyEquals, keyHash) \
    declare_cmap_10(X, Key, Value, valueDestroy, keyEquals, keyHash, \
                         c_default_destroy, Key, c_default_to_raw, c_default_from_raw)

#define declare_cmap_10(X, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw) \
    declare_CHASH(X, cmap, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyToRaw, keyFromRaw, Value, c_default_from_raw)

/* cset: */
#define declare_cset(...) \
    c_MACRO_OVERLOAD(declare_cset, __VA_ARGS__)

#define declare_cset_2(X, Key) \
    declare_cset_4(X, Key, c_default_equals, c_default_hash16)

#define declare_cset_4(X, Key, keyEquals, keyHash) \
    declare_cset_5(X, Key, keyEquals, keyHash, c_default_destroy)

#define declare_cset_5(X, Key, keyEquals, keyHash, keyDestroy) \
    declare_cset_8(X, Key, keyEquals, keyHash, keyDestroy, \
                        Key, c_default_to_raw, c_default_from_raw)

#define declare_cset_8(X, Key, keyEqualsRaw, keyHashRaw, keyDestroy, \
                            RawKey, keyToRaw, keyFromRaw) \
    declare_CHASH(X, cset, Key, Key, void, keyEqualsRaw, keyHashRaw, \
                       keyDestroy, RawKey, keyToRaw, keyFromRaw, void, c_default_from_raw)

/* cset_str, cmap_str, cmap_strkey, cmap_strval: */
#define declare_cset_str() \
    declare_CHASH_strkey(str, cset, cstr_t, void)

#define declare_cmap_str() \
    declare_CHASH(str, cmap, cstr_t, cstr_t, cstr_destroy, cstr_equals_raw, cstr_hash_raw, \
                       cstr_destroy, const char*, cstr_to_raw, cstr_make, const char*, cstr_make)

#define declare_cmap_strkey(...) \
    c_MACRO_OVERLOAD(declare_cmap_strkey, __VA_ARGS__)

#define declare_cmap_strkey_2(X, Value) \
    declare_CHASH_strkey(X, cmap, Value, c_default_destroy)

#define declare_cmap_strkey_3(X, Value, ValueDestroy) \
    declare_CHASH_strkey(X, cmap, Value, ValueDestroy)

#define declare_cmap_strval(...) \
    c_MACRO_OVERLOAD(declare_cmap_strval, __VA_ARGS__)

#define declare_cmap_strval_2(X, Key) \
    declare_cmap_strval_4(X, Key, c_default_equals, c_default_hash16)

#define declare_cmap_strval_4(X, Key, keyEquals, keyHash) \
    declare_CHASH(X, cmap, Key, cstr_t, cstr_destroy, keyEquals, keyHash, \
                       c_default_destroy, Key, c_default_to_raw, c_default_from_raw, const char*, cstr_make)

#define declare_CHASH_strkey(X, ctype, Value, valueDestroy) \
    declare_CHASH(X, ctype, cstr_t, Value, valueDestroy, cstr_equals_raw, cstr_hash_raw, \
                       cstr_destroy, const char*, cstr_to_raw, cstr_make, Value, c_default_from_raw)

#define CSET_ONLY_cset(x) x
#define CSET_ONLY_cmap(x)
#define CMAP_ONLY_cset(x)
#define CMAP_ONLY_cmap(x) x
#define CMAP_ARGS_cset(x, y) x
#define CMAP_ARGS_cmap(x, y) x, y

/* CHASH full: use 'void' for Value if ctype is cset */
#define declare_CHASH(X, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                           keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw) \
typedef struct { \
    Key key; \
    CMAP_ONLY_##ctype(Value value;) \
} ctype##_##X##_entry_t; \
 \
STC_INLINE void \
ctype##_##X##_entry_destroy(ctype##_##X##_entry_t* e) { \
    keyDestroy(&e->key); \
    CMAP_ONLY_##ctype(valueDestroy(&e->value);) \
} \
typedef struct { \
    RawKey key; \
    CMAP_ONLY_##ctype(RawValue value;) \
} ctype##_##X##_input_t; \
 \
typedef struct { \
    ctype##_##X##_entry_t *item; \
    bool inserted; \
} ctype##_##X##_result_t; \
 \
typedef Key ctype##_##X##_key_t; \
typedef Value ctype##_##X##_value_t; \
typedef RawKey ctype##_##X##_rawkey_t; \
typedef RawValue ctype##_##X##_rawvalue_t; \
 \
typedef struct ctype##_##X { \
    ctype##_##X##_entry_t* table; \
    uint8_t* _hashx; \
    uint32_t size, bucket_count; \
    float max_load_factor; \
    float shrink_limit_factor; \
} ctype##_##X; \
 \
typedef struct { \
    ctype##_##X##_entry_t *item; \
    uint8_t* _hx; \
} ctype##_##X##_iter_t; \
 \
STC_INLINE ctype##_##X \
ctype##_##X##_init(void) {ctype##_##X m = cmap_ini; return m;} \
STC_INLINE bool \
ctype##_##X##_empty(ctype##_##X m) {return m.size == 0;} \
STC_INLINE size_t \
ctype##_##X##_size(ctype##_##X m) {return (size_t) m.size;} \
STC_INLINE size_t \
ctype##_##X##_capacity(ctype##_##X m) {return (size_t) (m.bucket_count * m.max_load_factor);} \
STC_INLINE void \
ctype##_##X##_swap(ctype##_##X* a, ctype##_##X* b) {c_swap(ctype##_##X, *a, *b);} \
STC_INLINE void \
ctype##_##X##_set_load_factors(ctype##_##X* self, float max, float shrink) { \
    self->max_load_factor = max; self->shrink_limit_factor = shrink; \
} \
STC_API ctype##_##X \
ctype##_##X##_with_capacity(size_t cap); \
STC_API void \
ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_input_t in[], size_t size); \
STC_API void \
ctype##_##X##_destroy(ctype##_##X* self); \
STC_API void \
ctype##_##X##_clear(ctype##_##X* self); \
STC_API ctype##_##X##_entry_t* \
ctype##_##X##_find(const ctype##_##X* self, ctype##_##X##_rawkey_t rawKey); \
STC_API bool \
ctype##_##X##_contains(const ctype##_##X* self, ctype##_##X##_rawkey_t rawKey); \
 \
STC_API ctype##_##X##_result_t \
ctype##_##X##_insert_key_(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey); \
 \
STC_INLINE ctype##_##X##_result_t \
ctype##_##X##_emplace(ctype##_##X* self, CMAP_ARGS_##ctype(ctype##_##X##_rawkey_t rawKey, RawValue rawValue)) { \
    ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
    CMAP_ONLY_##ctype( if (res.inserted) res.item->value = valueFromRaw(rawValue); ) \
    return res; \
} \
STC_INLINE ctype##_##X##_result_t \
ctype##_##X##_insert(ctype##_##X* self, CMAP_ARGS_##ctype(ctype##_##X##_rawkey_t rawKey, RawValue rawValue)) { \
    return ctype##_##X##_emplace(self, CMAP_ARGS_##ctype(rawKey, rawValue)); \
} \
CMAP_ONLY_##ctype( \
STC_INLINE ctype##_##X##_result_t \
ctype##_##X##_insert_or_assign(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey, RawValue rawValue) { \
    ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
    if (!res.inserted) valueDestroy(&res.item->value); \
    res.item->value = valueFromRaw(rawValue); return res; \
} \
STC_INLINE ctype##_##X##_result_t \
ctype##_##X##_putv(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey, Value value) { \
    ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
    if (!res.inserted) valueDestroy(&res.item->value); \
    res.item->value = value; return res; \
}) \
 \
STC_INLINE ctype##_##X##_result_t \
ctype##_##X##_put(ctype##_##X* self, CMAP_ARGS_##ctype(ctype##_##X##_rawkey_t rawKey, RawValue rawValue)) { \
    CMAP_ONLY_##ctype( return ctype##_##X##_insert_or_assign(self, rawKey, rawValue); ) \
    CSET_ONLY_##ctype( return ctype##_##X##_insert_key_(self, rawKey); ) \
} \
 \
STC_API size_t \
ctype##_##X##_reserve(ctype##_##X* self, size_t size); \
STC_API bool \
ctype##_##X##_erase_entry(ctype##_##X* self, ctype##_##X##_entry_t* item); \
STC_API bool \
ctype##_##X##_erase(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey); \
 \
STC_INLINE ctype##_##X##_iter_t \
ctype##_##X##_begin(ctype##_##X* self) { \
    ctype##_##X##_iter_t it = {self->table, self->_hashx}; \
    if (it._hx) while (*it._hx == 0) ++it.item, ++it._hx; \
    return it; \
} \
STC_INLINE ctype##_##X##_iter_t \
ctype##_##X##_end(ctype##_##X* self) {\
    ctype##_##X##_iter_t it = {self->table + self->bucket_count}; return it; \
} \
STC_INLINE void \
ctype##_##X##_next(ctype##_##X##_iter_t* it) { \
    while ((++it->item, *++it->_hx == 0)) ; \
} \
CMAP_ONLY_##ctype( STC_INLINE ctype##_##X##_value_t* \
ctype##_##X##_itval(ctype##_##X##_iter_t it) {return &it.item->value;} ) \
 \
STC_API uint32_t c_default_hash16(const void *data, size_t len); \
STC_API uint32_t c_default_hash32(const void* data, size_t len); \
 \
implement_CHASH(X, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                     keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CHASH(X, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw, RawValue, valueFromRaw) \
STC_API ctype##_##X \
ctype##_##X##_with_capacity(size_t cap) { \
    ctype##_##X h = ctype##_ini; \
    ctype##_##X##_reserve(&h, cap); \
    return h; \
} \
STC_API void \
ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_input_t in[], size_t size) { \
    for (size_t i=0; i<size; ++i) ctype##_##X##_put(self, CMAP_ARGS_##ctype(in[i].key, in[i].value)); \
} \
 \
STC_INLINE void ctype##_##X##_wipe_(ctype##_##X* self) { \
    if (self->size == 0) return; \
    ctype##_##X##_entry_t* e = self->table, *end = e + self->bucket_count; \
    uint8_t *hx = self->_hashx; \
    for (; e != end; ++e) if (*hx++) ctype##_##X##_entry_destroy(e); \
} \
 \
STC_API void ctype##_##X##_destroy(ctype##_##X* self) { \
    ctype##_##X##_wipe_(self); \
    free(self->_hashx); \
    free(self->table); \
} \
 \
STC_API void ctype##_##X##_clear(ctype##_##X* self) { \
    ctype##_##X##_wipe_(self); \
    self->size = 0; \
    memset(self->_hashx, 0, self->bucket_count); \
} \
 \
STC_API ctype##_bucket_t \
ctype##_##X##_bucket(const ctype##_##X* self, const ctype##_##X##_rawkey_t* rawKeyPtr) { \
    uint32_t sx, hash = keyHashRaw(rawKeyPtr, sizeof(ctype##_##X##_rawkey_t)); \
    size_t cap = self->bucket_count; \
    ctype##_bucket_t b = {chash_reduce(hash, cap), (hash & chash_HASH) | chash_USED}; \
    uint8_t* hashx = self->_hashx; \
    while ((sx = hashx[b.idx])) { \
        if (sx == b.hx) { \
            ctype##_##X##_rawkey_t r = keyToRaw(&self->table[b.idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++b.idx == cap) b.idx = 0; \
    } \
    return b; \
} \
 \
STC_API ctype##_##X##_entry_t* \
ctype##_##X##_find(const ctype##_##X* self, ctype##_##X##_rawkey_t rawKey) { \
    if (self->size == 0) return NULL; \
    ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
    return self->_hashx[b.idx] ? &self->table[b.idx] : NULL; \
} \
 \
STC_INLINE void ctype##_##X##_reserve_expand(ctype##_##X* self) { \
    if (self->size + 1 >= self->bucket_count * self->max_load_factor) \
        ctype##_##X##_reserve(self, 5 + self->size * 3 / 2); \
} \
STC_API bool \
ctype##_##X##_contains(const ctype##_##X* self, ctype##_##X##_rawkey_t rawKey) { \
    return self->size && self->_hashx[ctype##_##X##_bucket(self, &rawKey).idx]; \
} \
 \
STC_API ctype##_##X##_result_t \
ctype##_##X##_insert_key_(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey) { \
    ctype##_##X##_reserve_expand(self); \
    ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
    ctype##_##X##_result_t res = {&self->table[b.idx], !self->_hashx[b.idx]}; \
    if (res.inserted) { \
        res.item->key = keyFromRaw(rawKey); \
        self->_hashx[b.idx] = (uint8_t) b.hx; \
        ++self->size; \
    } \
    return res; \
} \
 \
STC_API size_t \
ctype##_##X##_reserve(ctype##_##X* self, size_t newcap) { \
    size_t oldcap = self->bucket_count; \
    if (self->size > newcap) return oldcap; \
    newcap = (size_t) (newcap / self->max_load_factor) | 1; \
    ctype##_##X tmp = { \
        c_new_n(ctype##_##X##_entry_t, newcap), \
        (uint8_t *) calloc(newcap + 1, sizeof(uint8_t)), \
        self->size, (uint32_t) newcap, \
        self->max_load_factor, self->shrink_limit_factor \
    }; \
    /* Rehash: */ \
    tmp._hashx[newcap] = 0xff; c_swap(ctype##_##X, *self, tmp); \
    ctype##_##X##_entry_t* e = tmp.table, *slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (tmp._hashx[i]) { \
            ctype##_##X##_rawkey_t r = keyToRaw(&e->key); \
            ctype##_bucket_t b = ctype##_##X##_bucket(self, &r); \
            slot[b.idx] = *e, \
            hashx[b.idx] = (uint8_t) b.hx; \
        } \
    free(tmp._hashx); \
    free(tmp.table); \
    return newcap; \
} \
 \
STC_API bool \
ctype##_##X##_erase_entry(ctype##_##X* self, ctype##_##X##_entry_t* item) { \
    size_t i = chash_entry_index(*self, item), j = i, k, cap = self->bucket_count; \
    ctype##_##X##_entry_t* slot = self->table; \
    uint8_t* hashx = self->_hashx; \
    ctype##_##X##_rawkey_t r; \
    if (! hashx[i]) \
        return false; \
    ctype##_##X##_entry_destroy(&slot[i]); \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
        if (! hashx[j]) \
            break; \
        r = keyToRaw(&slot[j].key); \
        k = chash_reduce(keyHashRaw(&r, sizeof(ctype##_##X##_rawkey_t)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
    } while (true); \
    hashx[i] = 0; \
    --self->size; \
    return true; \
} \
 \
STC_API bool \
ctype##_##X##_erase(ctype##_##X* self, ctype##_##X##_rawkey_t rawKey) { \
    if (self->size == 0) \
        return false; \
    if (self->size < self->bucket_count * self->shrink_limit_factor && self->bucket_count * sizeof(ctype##_##X##_entry_t) > 1024) \
        ctype##_##X##_reserve(self, self->size * 6 / 5); \
    ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
    return ctype##_##X##_erase_entry(self, self->table + b.idx); \
} \
typedef int ctype##_##X##_dud

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
#define implement_CHASH(X, ctype, Key, Value, valueDestroy, keyEqualsRaw, keyHashRaw, \
                             keyDestroy, RawKey, keyToRaw, keyFromRaw)
#endif

#endif
