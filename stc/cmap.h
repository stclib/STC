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
        printf("set %d\n", i.ref->second);
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
        printf("map %d: %c\n", i.ref->first, i.ref->second);
    cmap_mx_del(&m);
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define cmap_inits                    {NULL, NULL, 0, 0, 0.15f, 0.85f}
#define cset_inits                    cmap_inits

#define c_try_emplace(self, ctype, key, val) do { \
    ctype##_result_t __r = ctype##_insert_key_(self, key); \
    if (__r.second) __r.first->second = val; \
} while (0)

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};
typedef struct {size_t idx; uint32_t hx;} cmap_bucket_t, cset_bucket_t;

#define using_cmap(...) \
    c_MACRO_OVERLOAD(using_cmap, __VA_ARGS__)

#define using_cmap_3(X, Key, Mapped) \
    using_cmap_5(X, Key, Mapped, c_default_del, c_default_clone)

#define using_cmap_5(X, Key, Mapped, mappedDel, mappedClone) \
    using_cmap_7(X, Key, Mapped, mappedDel, mappedClone, c_default_equals, c_default_hash)

#define using_cmap_7(X, Key, Mapped, mappedDel, mappedClone, keyEquals, keyHash) \
    using_cmap_9(X, Key, Mapped, mappedDel, mappedClone, keyEquals, keyHash, c_default_del, c_default_clone)

#define using_cmap_9(X, Key, Mapped, mappedDel, mappedClone, keyEquals, keyHash, keyDel, keyClone) \
    using_cmap_11(X, Key, Mapped, mappedDel, mappedClone, keyEquals, keyHash, keyDel, \
                     keyClone, c_default_to_raw, Key)

#define using_cmap_11(X, Key, Mapped, mappedDel, mappedClone, keyEqualsRaw, keyHashRaw, keyDel, \
                         keyFromRaw, keyToRaw, RawKey) \
    _using_CHASH(X, cmap, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                    keyFromRaw, keyToRaw, RawKey, mappedClone, c_default_to_raw, Mapped)

#define using_cmap_13(X, Key, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                         keyEqualsRaw, keyHashRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CHASH(X, cmap, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                    keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)

/* cset: */
#define using_cset(...) \
    c_MACRO_OVERLOAD(using_cset, __VA_ARGS__)

#define using_cset_2(X, Key) \
    using_cset_4(X, Key, c_default_equals, c_default_hash)

#define using_cset_4(X, Key, keyEquals, keyHash) \
    using_cset_6(X, Key, keyEquals, keyHash, c_default_del, c_default_clone)

#define using_cset_6(X, Key, keyEquals, keyHash, keyDel, keyClone) \
    using_cset_8(X, Key, keyEquals, keyHash, keyDel, keyClone, c_default_to_raw, Key)

#define using_cset_8(X, Key, keyEqualsRaw, keyHashRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CHASH(X, cset, Key, Key, _UNUSED_, keyEqualsRaw, keyHashRaw, keyDel, \
                    keyFromRaw, keyToRaw, RawKey, _UNUSED_, _UNUSED_, void)

/* cset_str, cmap_str, cmap_strkey, cmap_strval: */
#define using_cset_str() \
    _using_CHASH_strkey(str, cset, cstr_t, _UNUSED_, _UNUSED_)
#define using_cmap_str() \
    _using_CHASH(str, cmap, cstr_t, cstr_t, cstr_del, cstr_equals_raw, cstr_hash_raw, cstr_del, \
                      cstr_from, cstr_to_raw, const char*, cstr_from, cstr_to_raw, const char*)

#define using_cmap_strkey(...) \
    c_MACRO_OVERLOAD(using_cmap_strkey, __VA_ARGS__)
#define using_cmap_strkey_2(X, Mapped) \
    _using_CHASH_strkey(X, cmap, Mapped, c_default_del, c_default_clone)
#define using_cmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
    _using_CHASH_strkey(X, cmap, Mapped, mappedDel, mappedClone)

#define using_cmap_strval(...) \
    c_MACRO_OVERLOAD(using_cmap_strval, __VA_ARGS__)

#define using_cmap_strval_2(X, Key) \
    using_cmap_strval_4(X, Key, c_default_equals, c_default_hash)

#define using_cmap_strval_4(X, Key, keyEquals, keyHash) \
    using_cmap_strval_6(X, Key, keyEquals, keyHash, c_default_del, c_default_clone)

#define using_cmap_strval_6(X, Key, keyEquals, keyHash, keyDel, keyClone) \
    using_cmap_strval_8(X, Key, keyEquals, keyHash, keyDel, keyClone, c_default_to_raw, Key)

#define using_cmap_strval_8(X, Key, keyEquals, keyHash, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CHASH(X, cmap, Key, cstr_t, cstr_del, keyEquals, keyHash, keyDel, \
                    keyFromRaw, keyToRaw, RawKey, cstr_from, cstr_to_raw, const char*)

#define _using_CHASH_strkey(X, ctype, Mapped, mappedDel, mappedClone) \
    _using_CHASH(X, ctype, cstr_t, Mapped, mappedDel, cstr_equals_raw, cstr_hash_raw, cstr_del, \
                    cstr_from, cstr_to_raw, const char*, mappedClone, c_default_to_raw, Mapped)

#define CSET_ONLY_cset(...) __VA_ARGS__
#define CSET_ONLY_cmap(...)
#define CMAP_ONLY_cset(...)
#define CMAP_ONLY_cmap(...) __VA_ARGS__
#define KEY_REF_cset(e)     (*(e))
#define KEY_REF_cmap(e)     (e)->first

#define _using_CHASH(X, ctype, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef Key ctype##_##X##_key_t; \
    typedef Mapped ctype##_##X##_mapped_t; \
    typedef RawKey ctype##_##X##_rawkey_t; \
    typedef RawMapped ctype##_##X##_rawmapped_t; \
\
    typedef CSET_ONLY_##ctype( ctype##_##X##_key_t ) \
            CMAP_ONLY_##ctype( struct {ctype##_##X##_key_t first; \
                                      ctype##_##X##_mapped_t second;} ) \
    ctype##_##X##_value_t; \
\
    STC_INLINE void \
    ctype##_##X##_entry_del(ctype##_##X##_value_t* e) { \
        keyDel(&KEY_REF_##ctype(e)); \
        CMAP_ONLY_##ctype(mappedDel(&e->second);) \
    } \
    typedef CSET_ONLY_##ctype( ctype##_##X##_rawkey_t ) \
            CMAP_ONLY_##ctype( struct {ctype##_##X##_rawkey_t first; \
                                       ctype##_##X##_rawmapped_t second;} ) \
    ctype##_##X##_rawvalue_t; \
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
        float min_load_factor; \
        float max_load_factor; \
    } ctype##_##X; \
\
    typedef struct { \
        ctype##_##X##_value_t *ref; \
        uint8_t* _hx; \
    } ctype##_##X##_iter_t; \
\
    STC_INLINE ctype##_##X \
    ctype##_##X##_init(void) {ctype##_##X m = cmap_inits; return m;} \
    STC_INLINE bool \
    ctype##_##X##_empty(ctype##_##X m) {return m.size == 0;} \
    STC_INLINE size_t \
    ctype##_##X##_size(ctype##_##X m) {return (size_t) m.size;} \
    STC_INLINE ctype##_##X##_value_t \
    ctype##_##X##_value_clone(ctype##_##X##_value_t val) { \
        KEY_REF_##ctype(&val) = keyFromRaw(keyToRaw(&KEY_REF_##ctype(&val))); \
        CMAP_ONLY_##ctype( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
    STC_INLINE void \
    ctype##_##X##_value_del(ctype##_##X##_value_t* val) { \
        keyDel(&KEY_REF_##ctype(val)); \
        CMAP_ONLY_##ctype( mappedDel(&val->second); ) \
    } \
    STC_INLINE size_t \
    ctype##_##X##_bucket_count(ctype##_##X m) {return (size_t) m.bucket_count;} \
    STC_INLINE size_t \
    ctype##_##X##_capacity(ctype##_##X m) {return (size_t) (m.bucket_count * m.max_load_factor);} \
    STC_INLINE void \
    ctype##_##X##_swap(ctype##_##X* a, ctype##_##X* b) {c_swap(ctype##_##X, *a, *b);} \
    STC_INLINE void \
    ctype##_##X##_set_load_factors(ctype##_##X* self, float min_load, float max_load) { \
        self->min_load_factor = min_load; \
        self->max_load_factor = max_load; \
    } \
    STC_API ctype##_##X \
    ctype##_##X##_with_capacity(size_t cap); \
    STC_API ctype##_##X \
    ctype##_##X##_clone(ctype##_##X m); \
    STC_API void \
    ctype##_##X##_reserve(ctype##_##X* self, size_t size); \
    STC_API void \
    ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_rawvalue_t arr[], size_t size); \
    STC_API void \
    ctype##_##X##_del(ctype##_##X* self); \
    STC_API void \
    ctype##_##X##_clear(ctype##_##X* self); \
    STC_API ctype##_##X##_value_t* \
    ctype##_##X##_find(const ctype##_##X* self, RawKey rkey); \
    STC_API bool \
    ctype##_##X##_contains(const ctype##_##X* self, RawKey rkey); \
\
    STC_API ctype##_##X##_result_t \
    ctype##_##X##_insert_key_(ctype##_##X* self, RawKey rkey); \
    STC_API ctype##_bucket_t \
    ctype##_##X##_bucket(const ctype##_##X* self, const ctype##_##X##_rawkey_t* rkeyptr); \
\
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_emplace(ctype##_##X* self, RawKey rkey CMAP_ONLY_##ctype(, RawMapped rmapped)) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rkey); \
        CMAP_ONLY_##ctype( if (res.second) res.first->second = mappedFromRaw(rmapped); ) \
        return res; \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_insert(ctype##_##X* self, ctype##_##X##_rawvalue_t raw) { \
        return CSET_ONLY_##ctype(ctype##_##X##_insert_key_(self, raw)) \
               CMAP_ONLY_##ctype(ctype##_##X##_emplace(self, raw.first, raw.second)); \
    } \
\
    CMAP_ONLY_##ctype( \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_put(ctype##_##X* self, RawKey rkey, RawMapped rmapped) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mappedFromRaw(rmapped); return res; \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_insert_or_assign(ctype##_##X* self, RawKey rkey, RawMapped rmapped) { \
        return ctype##_##X##_put(self, rkey, rmapped); \
    } \
    STC_INLINE ctype##_##X##_result_t \
    ctype##_##X##_put_mapped(ctype##_##X* self, RawKey rkey, Mapped mapped) { \
        ctype##_##X##_result_t res = ctype##_##X##_insert_key_(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mapped; return res; \
    } \
    STC_INLINE ctype##_##X##_mapped_t* \
    ctype##_##X##_at(const ctype##_##X* self, RawKey rkey) { \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rkey); \
        assert(self->_hashx[b.idx]); \
        return &self->table[b.idx].second; \
    }) \
\
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_begin(ctype##_##X* self) { \
        ctype##_##X##_iter_t it = {self->table, self->_hashx}; \
        if (it._hx) while (*it._hx == 0) ++it.ref, ++it._hx; \
        return it; \
    } \
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_end(ctype##_##X* self) {\
        ctype##_##X##_iter_t it = {self->table + self->bucket_count}; return it; \
    } \
    STC_INLINE void \
    ctype##_##X##_next(ctype##_##X##_iter_t* it) { \
        while ((++it->ref, *++it->_hx == 0)) ; \
    } \
    STC_INLINE ctype##_##X##_mapped_t* \
    ctype##_##X##_itval(ctype##_##X##_iter_t it) {return CMAP_ONLY_##ctype(&it.ref->second) \
                                                         CSET_ONLY_##ctype(it.ref);} \
\
    STC_API void \
    ctype##_##X##_erase_entry(ctype##_##X* self, ctype##_##X##_value_t* val); \
    STC_INLINE size_t \
    ctype##_##X##_erase(ctype##_##X* self, RawKey rkey) { \
        if (self->size == 0) return 0; \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rkey); \
        return self->_hashx[b.idx] ? ctype##_##X##_erase_entry(self, self->table + b.idx), 1 : 0; \
    } \
    STC_INLINE ctype##_##X##_iter_t \
    ctype##_##X##_erase_at(ctype##_##X* self, ctype##_##X##_iter_t pos) { \
        ctype##_##X##_erase_entry(self, pos.ref); \
        ctype##_##X##_next(&pos); return pos; \
    } \
\
    STC_API uint32_t c_default_hash(const void *data, size_t len); \
    STC_API uint32_t c_default_hash32(const void* data, size_t len); \
\
    _c_implement_CHASH(X, ctype, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                          keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef ctype##_##X ctype##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_CHASH(X, ctype, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                              keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    STC_DEF ctype##_##X \
    ctype##_##X##_with_capacity(size_t cap) { \
        ctype##_##X h = ctype##_inits; \
        ctype##_##X##_reserve(&h, cap); \
        return h; \
    } \
    STC_DEF void \
    ctype##_##X##_push_n(ctype##_##X* self, const ctype##_##X##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) CMAP_ONLY_##ctype(ctype##_##X##_put(self, arr[i].first, arr[i].second)) \
                                   CSET_ONLY_##ctype(ctype##_##X##_insert(self, arr[i])) ; \
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
    ctype##_##X##_bucket(const ctype##_##X* self, const ctype##_##X##_rawkey_t* rkeyptr) { \
        uint32_t sx, hash = keyHashRaw(rkeyptr, sizeof(ctype##_##X##_rawkey_t)); \
        size_t cap = self->bucket_count; \
        ctype##_bucket_t b = {chash_reduce(hash, cap), (hash & chash_HASH) | chash_USED}; \
        uint8_t* hashx = self->_hashx; \
        while ((sx = hashx[b.idx])) { \
            if (sx == b.hx) { \
                ctype##_##X##_rawkey_t r = keyToRaw(&KEY_REF_##ctype(self->table + b.idx)); \
                if (keyEqualsRaw(&r, rkeyptr)) break; \
            } \
            if (++b.idx == cap) b.idx = 0; \
        } \
        return b; \
    } \
\
    STC_DEF ctype##_##X##_value_t* \
    ctype##_##X##_find(const ctype##_##X* self, RawKey rkey) { \
        if (self->size == 0) return NULL; \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rkey); \
        return self->_hashx[b.idx] ? &self->table[b.idx] : NULL; \
    } \
\
    STC_DEF bool \
    ctype##_##X##_contains(const ctype##_##X* self, RawKey rkey) { \
        return self->size && self->_hashx[ctype##_##X##_bucket(self, &rkey).idx]; \
    } \
\
    STC_INLINE void ctype##_##X##_reserve_expand_(ctype##_##X* self) { \
        if (self->size + 1 >= self->bucket_count * self->max_load_factor) \
            ctype##_##X##_reserve(self, 5 + self->size * 3 / 2); \
    } \
    STC_DEF ctype##_##X##_result_t \
    ctype##_##X##_insert_key_(ctype##_##X* self, RawKey rkey) { \
        ctype##_##X##_reserve_expand_(self); \
        ctype##_bucket_t b = ctype##_##X##_bucket(self, &rkey); \
        ctype##_##X##_result_t res = {&self->table[b.idx], !self->_hashx[b.idx]}; \
        if (res.second) { \
            KEY_REF_##ctype(res.first) = keyFromRaw(rkey); \
            self->_hashx[b.idx] = (uint8_t) b.hx; \
            ++self->size; \
        } \
        return res; \
    } \
\
    STC_DEF ctype##_##X \
    ctype##_##X##_clone(ctype##_##X m) { \
        ctype##_##X clone = { \
            c_new_2(ctype##_##X##_value_t, m.bucket_count), \
            (uint8_t *) memcpy(c_malloc(m.bucket_count + 1), m._hashx, m.bucket_count + 1), \
            m.size, m.bucket_count, m.min_load_factor, m.max_load_factor \
        }; \
        ctype##_##X##_value_t *e = m.table, *end = e + m.bucket_count, *dst = clone.table; \
        for (uint8_t *hx = m._hashx; e != end; ++hx, ++e, ++dst) \
            if (*hx) *dst = ctype##_##X##_value_clone(*e); \
        return clone; \
    } \
\
    STC_DEF void \
    ctype##_##X##_reserve(ctype##_##X* self, size_t newcap) { \
        if (newcap < self->size) return; \
        size_t oldcap = self->bucket_count; \
        newcap = (size_t) (newcap / self->max_load_factor) | 1; \
        ctype##_##X tmp = { \
            c_new_2 (ctype##_##X##_value_t, newcap), \
            (uint8_t *) c_calloc(newcap + 1, sizeof(uint8_t)), \
            self->size, (uint32_t) newcap, \
            self->min_load_factor, self->max_load_factor \
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
        hashx[i] = 0, k = --self->size; \
        if ((float) k / cap < self->min_load_factor && k > 512) \
            ctype##_##X##_reserve(self, k*1.2); \
    }

/* https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/ */

STC_DEF uint32_t c_default_hash(const void *data, size_t len) {
    const volatile uint16_t *key = (const uint16_t *) data;
    uint64_t x = *key++ * 11400714819323198485llu;
    while (len -= 2) x = (*key++ + x) * 11400714819323198485llu;
    return (uint32_t) x;
}
STC_DEF uint32_t c_default_hash32(const void* data, size_t len) {
    const volatile uint32_t *key = (const uint32_t *) data;
    uint64_t x = *key++ * 2654435769ull;
    while (len -= 4) x = (*key++ + x) * 2654435769ull;
    return (uint32_t) x;
}

#else
#define _c_implement_CHASH(X, ctype, Key, Mapped, mappedDel, keyEqualsRaw, keyHashRaw, keyDel, \
                              keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

#endif
