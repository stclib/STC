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

/*
#include <stdio.h>
#include <stc/cmap.h>
using_cset(sx, int);       // Set of int
using_cmap(mx, int, char); // Map of int -> char

int main(void) {
    cset_sx s = cset_inits;
    cset_sx_insert(&s, 5);
    cset_sx_insert(&s, 8);
    c_foreach (i, cset_sx, s)
        printf("set %d\n", i.val->second);
    cset_sx_del(&s);

    cmap_mx m = cmap_inits;
    cmap_mx_put(&m, 5, 'a');
    cmap_mx_put(&m, 8, 'b');
    cmap_mx_put(&m, 12, 'c');
    cmap_mx_value_t *e = cmap_mx_find(&m, 10); // = NULL
    char val = cmap_mx_find(&m, 5)->second;
    cmap_mx_put(&m, 5, 'd'); // update
    cmap_mx_erase(&m, 8);
    c_foreach (i, cmap_mx, m)
        printf("map %d: %c\n", i.val->first, i.val->second);
    cmap_mx_del(&m);
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define cmap_inits                    {NULL, NULL, 0, 0, 0.85f, 0.15f}
#define cmap_empty(m)                 ((m).size == 0)
#define cmap_size(m)                  ((size_t) (m).size)
#define cset_inits                    cmap_inits
#define cset_empty(s)                 cmap_empty(s)
#define cset_size(s)                  cmap_size(s)

#define c_try_emplace(self, ctype, key, val) do { \
    ctype##_result_t __r = ctype##_insert_key_(self, key); \
    if (__r.second) __r.first->second = val; \
} while (0)

#define c_insert_items(self, ctype, ...) do { \
    const ctype##_input_t __arr[] = __VA_ARGS__; \
    for (size_t __i=0;__i<sizeof __arr/sizeof *__arr; ++__i) \
        ctype##_insert(self, __arr[__i]); \
} while (0)

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};
typedef struct {size_t idx; uint32_t hx;} cmap_bucket_t, cset_bucket_t;

#define using_cmap(...) \
    c_MACRO_OVERLOAD(using_cmap, __VA_ARGS__)

#define using_cmap_3(X, Key, Mapped) \
    using_cmap_4(X, Key, Mapped, c_default_del)

#define using_cmap_4(X, Key, Mapped, valueDestroy) \
    using_cmap_6(X, Key, Mapped, valueDestroy, c_default_equals, c_default_hash16)

#define using_cmap_6(X, Key, Mapped, valueDestroy, keyEquals, keyHash) \
    using_cmap_10(X, Key, Mapped, valueDestroy, keyEquals, keyHash, \
                    c_default_del, Key, c_default_to_raw, c_default_from_raw)

#define using_cmap_10(X, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw) \
    _c_typedef_CHASH(X, cmap, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw, Mapped, c_default_from_raw)

#define using_cmap_12(X, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw) \
    _c_typedef_CHASH(X, cmap, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw)

/* cset: */
#define using_cset(...) \
    c_MACRO_OVERLOAD(using_cset, __VA_ARGS__)

#define using_cset_2(X, Key) \
    using_cset_4(X, Key, c_default_equals, c_default_hash16)

#define using_cset_4(X, Key, keyEquals, keyHash) \
    using_cset_5(X, Key, keyEquals, keyHash, c_default_del)

#define using_cset_5(X, Key, keyEquals, keyHash, keyDestroy) \
    using_cset_8(X, Key, keyEquals, keyHash, keyDestroy, \
                Key, c_default_to_raw, c_default_from_raw)

#define using_cset_8(X, Key, keyEqualsRaw, keyHashRaw, keyDestroy, \
                    RawKey, keyToRaw, keyFromRaw) \
    _c_typedef_CHASH(X, cset, Key, Key, void, keyEqualsRaw, keyHashRaw, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw, void, c_default_from_raw)

/* cset_str, cmap_str, cmap_strkey, cmap_strval: */
#define using_cset_str() \
    _c_declare_CHASH_strkey(str, cset, cstr_t, void)

#define using_cmap_str() \
    _c_typedef_CHASH(str, cmap, cstr_t, cstr_t, cstr_del, cstr_equals_raw, cstr_hash_raw, \
                          cstr_del, const char*, cstr_to_raw, cstr_from, const char*, cstr_from)

#define using_cmap_strkey(...) \
    c_MACRO_OVERLOAD(using_cmap_strkey, __VA_ARGS__)

#define using_cmap_strkey_2(X, Mapped) \
    _c_declare_CHASH_strkey(X, cmap, Mapped, c_default_del)

#define using_cmap_strkey_3(X, Mapped, ValueDestroy) \
    _c_declare_CHASH_strkey(X, cmap, Mapped, ValueDestroy)

#define using_cmap_strval(...) \
    c_MACRO_OVERLOAD(using_cmap_strval, __VA_ARGS__)

#define using_cmap_strval_2(X, Key) \
    using_cmap_strval_4(X, Key, c_default_equals, c_default_hash16)

#define using_cmap_strval_4(X, Key, keyEquals, keyHash) \
    using_cmap_strval_8(X, Key, keyEquals, keyHash, \
                             c_default_del, Key, c_default_to_raw, c_default_from_raw)

#define using_cmap_strval_8(X, Key, keyEquals, keyHash, keyDestroy, RawKey, keyToRaw, keyFromRaw) \
    _c_typedef_CHASH(X, cmap, Key, cstr_t, cstr_del, keyEquals, keyHash, \
                        keyDestroy, RawKey, keyToRaw, keyFromRaw, const char*, cstr_from)


#define _c_declare_CHASH_strkey(X, ctype, Mapped, valueDestroy) \
    _c_typedef_CHASH(X, ctype, cstr_t, Mapped, valueDestroy, cstr_equals_raw, cstr_hash_raw, \
                        cstr_del, const char*, cstr_to_raw, cstr_from, Mapped, c_default_from_raw)

#define CSET_ONLY_cset(...) __VA_ARGS__
#define CSET_ONLY_cmap(...)
#define CMAP_ONLY_cset(...)
#define CMAP_ONLY_cmap(...) __VA_ARGS__
#define KEY_REF_cset(e)     (*(e))
#define KEY_REF_cmap(e)     (e)->first

/* CHASH full: use 'void' for Mapped if ctype is cset */
#define _c_typedef_CHASH(X, ctype, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                            keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw) \
    typedef Key ctype##_##X##_key_t; \
    typedef Mapped ctype##_##X##_mapped_t; \
    typedef RawKey ctype##_##X##_rawkey_t; \
    typedef RawVal ctype##_##X##_rawval_t; \
\
    typedef CSET_ONLY_##ctype( ctype##_##X##_key_t ) \
            CMAP_ONLY_##ctype( struct {ctype##_##X##_key_t first; \
                                      ctype##_##X##_mapped_t second;} ) \
    ctype##_##X##_value_t, ctype##_##X##_entry_t; \
\
    STC_INLINE void \
    ctype##_##X##_entry_del(ctype##_##X##_value_t* e) { \
        keyDestroy(&KEY_REF_##ctype(e)); \
        CMAP_ONLY_##ctype(valueDestroy(&e->second);) \
    } \
    typedef CSET_ONLY_##ctype( ctype##_##X##_rawkey_t ) \
            CMAP_ONLY_##ctype( struct {ctype##_##X##_rawkey_t first; \
                                       ctype##_##X##_rawval_t second;} ) \
    ctype##_##X##_input_t; \
\
    typedef struct { \
        ctype##_##X##_value_t *first; \
        bool second; /* inserted */ \
    } ctype##_##X##_result_t; \
\
    typedef struct { \
        ctype##_##X##_value_t* table; \
        uint8_t* _hashx; \
        uint32_t size, bucket_count; \
        float max_load_factor; \
        float shrink_limit_factor; \
    } ctype##_##X; \
\
    typedef struct { \
        ctype##_##X##_value_t *val; \
        uint8_t* _hx; \
    } ctype##_##X##_iter_t; \
\
    STC_INLINE ctype##_##X \
    ctype##_##X##_init(void) {ctype##_##X m = cmap_inits; return m;} \
    STC_INLINE bool \
    ctype##_##X##_empty(ctype##_##X m) {return m.size == 0;} \
    STC_INLINE size_t \
    ctype##_##X##_size(ctype##_##X m) {return (size_t) m.size;} \
    STC_INLINE size_t \
    ctype##_##X##_bucket_count(ctype##_##X m) {return (size_t) m.bucket_count;} \
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
    ctype##_##X##_reserve(ctype##_##X* self, size_t size); \
    STC_API void \
    ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_input_t in[], size_t size); \
    STC_API void \
    ctype##_##X##_del(ctype##_##X* self); \
    STC_API void \
    ctype##_##X##_clear(ctype##_##X* self); \
    STC_API ctype##_##X##_value_t* \
    ctype##_##X##_find(const ctype##_##X* self, RawKey rawKey); \
    STC_API bool \
    ctype##_##X##_contains(const ctype##_##X* self, RawKey rawKey); \
\
    STC_API ctype##_##X##_result_t \
    ctype##_##X##_insert_key_(ctype##_##X* self, RawKey rawKey); \
    STC_API ctype##_bucket_t \
    ctype##_##X##_bucket(const ctype##_##X* self, const ctype##_##X##_rawkey_t* rawKeyPtr); \
\
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_emplace(ctype##_##X* self, RawKey rawKey CMAP_ONLY_##ctype(, RawVal rawVal)) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
        CMAP_ONLY_##ctype( if (res.second) res.first->second = valueFromRaw(rawVal); ) \
        return res; \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_insert(ctype##_##X* self, ctype##_##X##_input_t in) { \
        return CSET_ONLY_##ctype(ctype##_##X##_insert_key_(self, in)) \
               CMAP_ONLY_##ctype(ctype##_##X##_emplace(self, in.first, in.second)); \
    } \
\
    CMAP_ONLY_##ctype( \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_insert_or_assign(ctype##_##X* self, RawKey rawKey, RawVal rawVal) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
        if (!res.second) valueDestroy(&res.first->second); \
        res.first->second = valueFromRaw(rawVal); return res; \
    } \
    STC_INLINE ctype##_##X##_mapped_t* \
    ctype##_##X##_at(const ctype##_##X* self, RawKey rawKey) { \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
        assert(self->_hashx[b.idx]); \
        return &self->table[b.idx].second; \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_put(ctype##_##X* self, RawKey rawKey, RawVal rawVal) { \
        return ctype##_##X##_insert_or_assign(self, rawKey, rawVal); \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_putv(ctype##_##X* self, RawKey rawKey, Mapped mapped) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rawKey); \
        if (!res.second) valueDestroy(&res.first->second); \
        res.first->second = mapped; return res; \
    }) \
\
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_begin(ctype##_##X* self) { \
        ctype##_##X##_iter_t it = {self->table, self->_hashx}; \
        if (it._hx) while (*it._hx == 0) ++it.val, ++it._hx; \
        return it; \
    } \
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_end(ctype##_##X* self) {\
        ctype##_##X##_iter_t it = {self->table + self->bucket_count}; return it; \
    } \
    STC_INLINE void \
    ctype##_##X##_next(ctype##_##X##_iter_t* it) { \
        while ((++it->val, *++it->_hx == 0)) ; \
    } \
    CMAP_ONLY_##ctype( STC_INLINE ctype##_##X##_mapped_t* \
    ctype##_##X##_itval(ctype##_##X##_iter_t it) {return &it.val->second;} ) \
\
    STC_API void \
    ctype##_##X##_erase_entry(ctype##_##X* self, ctype##_##X##_value_t* val); \
    STC_INLINE size_t \
    ctype##_##X##_erase(ctype##_##X* self, RawKey rawKey) { \
        if (self->size == 0) return 0; \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
        return self->_hashx[b.idx] ? ctype##_##X##_erase_entry(self, self->table + b.idx), 1 : 0; \
    } \
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_erase_at(ctype##_##X* self, ctype##_##X##_iter_t pos) { \
        ctype##_##X##_erase_entry(self, pos.val); \
        ctype##_##X##_next(&pos); return pos; \
    } \
\
    STC_API uint32_t c_default_hash16(const void *data, size_t len); \
    STC_API uint32_t c_default_hash32(const void* data, size_t len); \
\
    _c_implement_CHASH(X, ctype, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                          keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw) \
    typedef ctype##_##X ctype##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_CHASH(X, ctype, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                              keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw) \
    STC_DEF ctype##_##X \
    ctype##_##X##_with_capacity(size_t cap) { \
        ctype##_##X h = ctype##_inits; \
        ctype##_##X##_reserve(&h, cap); \
        return h; \
    } \
    STC_DEF void \
    ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_input_t in[], size_t n) { \
        for (size_t i=0; i<n; ++i) CMAP_ONLY_##ctype(ctype##_##X##_put(self, in[i].first, in[i].second)) \
                                   CSET_ONLY_##ctype(ctype##_##X##_insert(self, in[i])) ; \
    } \
\
    STC_INLINE void ctype##_##X##_wipe_(ctype##_##X* self) { \
        if (self->size == 0) return; \
        ctype##_##X##_value_t* e = self->table, *end = e + self->bucket_count; \
        uint8_t *hx = self->_hashx; \
        for (; e != end; ++e) if (*hx++) ctype##_##X##_entry_del(e); \
    } \
\
    STC_DEF void ctype##_##X##_del(ctype##_##X* self) { \
        ctype##_##X##_wipe_(self); \
        c_free(self->_hashx); \
        c_free(self->table); \
    } \
\
    STC_DEF void ctype##_##X##_clear(ctype##_##X* self) { \
        ctype##_##X##_wipe_(self); \
        self->size = 0; \
        memset(self->_hashx, 0, self->bucket_count); \
    } \
\
    STC_DEF ctype##_bucket_t \
    ctype##_##X##_bucket(const ctype##_##X* self, const ctype##_##X##_rawkey_t* rawKeyPtr) { \
        uint32_t sx, hash = keyHashRaw(rawKeyPtr, sizeof(ctype##_##X##_rawkey_t)); \
        size_t cap = self->bucket_count; \
        ctype##_bucket_t b = {chash_reduce(hash, cap), (hash & chash_HASH) | chash_USED}; \
        uint8_t* hashx = self->_hashx; \
        while ((sx = hashx[b.idx])) { \
            if (sx == b.hx) { \
                ctype##_##X##_rawkey_t r = keyToRaw(&KEY_REF_##ctype(self->table + b.idx)); \
                if (keyEqualsRaw(&r, rawKeyPtr)) break; \
            } \
            if (++b.idx == cap) b.idx = 0; \
        } \
        return b; \
    } \
\
    STC_DEF ctype##_##X##_value_t* \
    ctype##_##X##_find(const ctype##_##X* self, RawKey rawKey) { \
        if (self->size == 0) return NULL; \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
        return self->_hashx[b.idx] ? &self->table[b.idx] : NULL; \
    } \
\
    STC_INLINE void ctype##_##X##_reserve_expand(ctype##_##X* self) { \
        if (self->size + 1 >= self->bucket_count * self->max_load_factor) \
            ctype##_##X##_reserve(self, 5 + self->size * 3 / 2); \
    } \
    STC_DEF bool \
    ctype##_##X##_contains(const ctype##_##X* self, RawKey rawKey) { \
        return self->size && self->_hashx[ctype##_##X##_bucket(self, &rawKey).idx]; \
    } \
\
    STC_DEF ctype##_##X##_result_t \
    ctype##_##X##_insert_key_(ctype##_##X* self, RawKey rawKey) { \
        ctype##_##X##_reserve_expand(self); \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rawKey); \
        ctype##_##X##_result_t res = {&self->table[b.idx], !self->_hashx[b.idx]}; \
        if (res.second) { \
            KEY_REF_##ctype(res.first) = keyFromRaw(rawKey); \
            self->_hashx[b.idx] = (uint8_t) b.hx; \
            ++self->size; \
        } \
        return res; \
    } \
\
    STC_DEF void \
    ctype##_##X##_reserve(ctype##_##X* self, size_t newcap) { \
        size_t oldcap = self->bucket_count; \
        if (self->size > newcap) return; \
        newcap = (size_t) (newcap / self->max_load_factor) | 1; \
        ctype##_##X tmp = { \
            c_new_2 (ctype##_##X##_value_t, newcap), \
            (uint8_t *) c_calloc(newcap + 1, sizeof(uint8_t)), \
            self->size, (uint32_t) newcap, \
            self->max_load_factor, self->shrink_limit_factor \
        }; \
        /* Rehash: */ \
        tmp._hashx[newcap] = 0xff; c_swap(ctype##_##X, *self, tmp); \
        ctype##_##X##_value_t* e = tmp.table, *slot = self->table; \
        uint8_t* hashx = self->_hashx; \
        for (size_t i = 0; i < oldcap; ++i, ++e) \
            if (tmp._hashx[i]) { \
                RawKey r = keyToRaw(&KEY_REF_##ctype(e)); \
                ctype##_bucket_t b = ctype##_##X##_bucket(self, &r); \
                slot[b.idx] = *e, \
                hashx[b.idx] = (uint8_t) b.hx; \
            } \
        c_free(tmp._hashx); \
        c_free(tmp.table); \
    } \
\
    STC_DEF void \
    ctype##_##X##_erase_entry(ctype##_##X* self, ctype##_##X##_value_t* val) { \
        size_t i = chash_entry_index(*self, val), j = i, k, cap = self->bucket_count; \
        ctype##_##X##_value_t* slot = self->table; \
        uint8_t* hashx = self->_hashx; \
        ctype##_##X##_entry_del(&slot[i]); \
        do { /* deletion from hash table without tombstone */ \
            if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
            if (! hashx[j]) \
                break; \
            RawKey r = keyToRaw(&KEY_REF_##ctype(slot + j)); \
            k = chash_reduce(keyHashRaw(&r, sizeof(RawKey)), cap); \
            if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
                slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
        } while (true); \
        hashx[i] = 0; \
        --self->size; \
    }

/* https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/ */

STC_DEF uint32_t c_default_hash16(const void *data, size_t len) {
    const volatile uint16_t *key = (const uint16_t *) data;
    uint64_t x = *key++ * 0xc613fc15u;
    while (len -= 2) x = (*key++ + x) * 2654435769ull;
    return (uint32_t) x;
}
STC_DEF uint32_t c_default_hash32(const void* data, size_t len) {
    const volatile uint32_t *key = (const uint32_t *) data;
    uint64_t x = *key++ * 2654435769ull;
    while (len -= 4) x = (*key++ + x) * 2654435769ull;
    return (uint32_t) x;
}

#else
#define _c_implement_CHASH(X, ctype, Key, Mapped, valueDestroy, keyEqualsRaw, keyHashRaw, \
                              keyDestroy, RawKey, keyToRaw, keyFromRaw, RawVal, valueFromRaw)
#endif

#endif
