/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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

// Unordered set/map - implemented as open hashing with linear probing and no tombstones.
/*
#include <stdio.h>
#include <stc/cmap.h>
using_cset(sx, int);       // Set of int
using_cmap(mx, int, char); // Map of int -> char

int main(void) {
    cset_sx s = cset_sx_init();
    cset_sx_insert(&s, 5);
    cset_sx_insert(&s, 8);
    c_foreach (i, cset_sx, s)
        printf("set %d\n", i.ref->second);
    cset_sx_del(&s);

    cmap_mx m = cmap_mx_init();
    cmap_mx_put(&m, 5, 'a');
    cmap_mx_put(&m, 8, 'b');
    cmap_mx_put(&m, 12, 'c');
    cmap_mx_iter_t it = cmap_mx_find(&m, 10); // none
    char val = cmap_mx_find(&m, 5).ref->second;
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

#define _cmap_inits   {NULL, NULL, 0, 0, 0.15f, 0.85f}

/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define chash_reduce(x, N)            ((uint32_t) (((uint64_t) (x) * (N)) >> 32))
#define chash_entry_index(h, entryPtr) ((entryPtr) - (h).table)

enum {chash_HASH = 0x7f, chash_USED = 0x80};
typedef struct {size_t idx; uint32_t hx;} cmap_bucket_t, cset_bucket_t;

#define using_cmap(...) \
    c_MACRO_OVERLOAD(using_cmap, __VA_ARGS__)

#define using_cmap_3(X, Key, Mapped) \
    using_cmap_5(X, Key, Mapped, c_default_equals, c_default_hash)

#define using_cmap_5(X, Key, Mapped, keyEquals, keyHash) \
    using_cmap_7(X, Key, Mapped, keyEquals, keyHash, c_default_del, c_default_clone)

#define using_cmap_7(X, Key, Mapped, keyEquals, keyHash, mappedDel, mappedClone) \
    using_cmap_9(X, Key, Mapped, keyEquals, keyHash, mappedDel, mappedClone, c_default_del, c_default_clone)

#define using_cmap_9(X, Key, Mapped, keyEquals, keyHash, mappedDel, mappedClone, keyDel, keyClone) \
    using_cmap_11(X, Key, Mapped, keyEquals, keyHash, mappedDel, mappedClone, \
                     keyDel, keyClone, c_default_to_raw, Key)

#define using_cmap_11(X, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, mappedClone, \
                         keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CHASH(X, cmap, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, mappedClone, \
                    keyDel, keyFromRaw, keyToRaw, RawKey, c_default_to_raw, Mapped)

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
    _using_CHASH(X, cset, Key, Key, keyEqualsRaw, keyHashRaw, _UNUSED_, _UNUSED_, \
                    keyDel, keyFromRaw, keyToRaw, RawKey, _UNUSED_, void)

/* cset_str, cmap_str, cmap_strkey, cmap_strval: */
#define using_cset_str() \
    _using_CHASH_strkey(str, cset, cstr_t, _UNUSED_, _UNUSED_)
#define using_cmap_str() \
    _using_CHASH(str, cmap, cstr_t, cstr_t, cstr_equals_raw, cstr_hash_raw, cstr_del, cstr_from, \
                      cstr_del, cstr_from, cstr_to_raw, const char*, cstr_to_raw, const char*)

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
    _using_CHASH(X, cmap, Key, cstr_t, keyEquals, keyHash, cstr_del, cstr_from, \
                    keyDel, keyFromRaw, keyToRaw, RawKey, cstr_to_raw, const char*)

#define _using_CHASH_strkey(X, C, Mapped, mappedDel, mappedClone) \
    _using_CHASH(X, C, cstr_t, Mapped, cstr_equals_raw, cstr_hash_raw, mappedDel, mappedClone, \
                    cstr_del, cstr_from, cstr_to_raw, const char*, c_default_to_raw, Mapped)

#define SET_ONLY_cset(...) __VA_ARGS__
#define SET_ONLY_cmap(...)
#define MAP_ONLY_cset(...)
#define MAP_ONLY_cmap(...) __VA_ARGS__
#define KEY_REF_cset(vp)   (vp)
#define KEY_REF_cmap(vp)   (&(vp)->first)

#define _using_CHASH(X, C, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, mappedFromRaw, \
                        keyDel, keyFromRaw, keyToRaw, RawKey, mappedToRaw, RawMapped) \
    typedef Key C##_##X##_key_t; \
    typedef Mapped C##_##X##_mapped_t; \
    typedef RawKey C##_##X##_rawkey_t; \
    typedef RawMapped C##_##X##_rawmapped_t; \
\
    typedef SET_ONLY_##C( C##_##X##_key_t ) \
            MAP_ONLY_##C( struct {C##_##X##_key_t first; \
                                      C##_##X##_mapped_t second;} ) \
    C##_##X##_value_t; \
\
    STC_INLINE void \
    C##_##X##_entry_del(C##_##X##_value_t* e) { \
        keyDel(KEY_REF_##C(e)); \
        MAP_ONLY_##C(mappedDel(&e->second);) \
    } \
    typedef SET_ONLY_##C( C##_##X##_rawkey_t ) \
            MAP_ONLY_##C( struct {C##_##X##_rawkey_t first; \
                                      C##_##X##_rawmapped_t second;} ) \
    C##_##X##_rawvalue_t; \
\
    typedef struct { \
        C##_##X##_value_t *first; \
        bool second; /* inserted */ \
    } C##_##X##_result_t; \
\
    typedef struct { \
        C##_##X##_value_t* table; \
        uint8_t* _hashx; \
        uint32_t size, bucket_count; \
        float min_load_factor; \
        float max_load_factor; \
    } C##_##X; \
\
    typedef struct { \
        C##_##X##_value_t *ref; \
        uint8_t* _hx; \
    } C##_##X##_iter_t; \
\
    STC_INLINE C##_##X \
    C##_##X##_init(void) {C##_##X m = _cmap_inits; return m;} \
    STC_INLINE bool \
    C##_##X##_empty(C##_##X m) {return m.size == 0;} \
    STC_INLINE size_t \
    C##_##X##_size(C##_##X m) {return (size_t) m.size;} \
    STC_INLINE C##_##X##_value_t \
    C##_##X##_value_clone(C##_##X##_value_t val) { \
        *KEY_REF_##C(&val) = keyFromRaw(keyToRaw(KEY_REF_##C(&val))); \
        MAP_ONLY_##C( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
    STC_INLINE void \
    C##_##X##_value_del(C##_##X##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE size_t \
    C##_##X##_bucket_count(C##_##X m) {return (size_t) m.bucket_count;} \
    STC_INLINE size_t \
    C##_##X##_capacity(C##_##X m) {return (size_t) (m.bucket_count * m.max_load_factor);} \
    STC_INLINE void \
    C##_##X##_swap(C##_##X* a, C##_##X* b) {c_swap(C##_##X, *a, *b);} \
    STC_INLINE void \
    C##_##X##_set_load_factors(C##_##X* self, float min_load, float max_load) { \
        self->min_load_factor = min_load; \
        self->max_load_factor = max_load; \
    } \
    STC_API C##_##X \
    C##_##X##_with_capacity(size_t cap); \
    STC_API C##_##X \
    C##_##X##_clone(C##_##X m); \
    STC_API void \
    C##_##X##_reserve(C##_##X* self, size_t size); \
    STC_API void \
    C##_##X##_del(C##_##X* self); \
    STC_API void \
    C##_##X##_clear(C##_##X* self); \
    STC_API C##_##X##_iter_t \
    C##_##X##_find(const C##_##X* self, RawKey rkey); \
    STC_API bool \
    C##_##X##_contains(const C##_##X* self, RawKey rkey); \
\
    STC_API C##_##X##_result_t \
    C##_##X##_insert_key(C##_##X* self, RawKey rkey); \
    STC_API C##_bucket_t \
    C##_##X##_bucket(const C##_##X* self, const C##_##X##_rawkey_t* rkeyptr); \
\
    STC_INLINE C##_##X##_result_t \
    C##_##X##_emplace(C##_##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        C##_##X##_result_t res = C##_##X##_insert_key(self, rkey); \
        MAP_ONLY_##C( if (res.second) res.first->second = mappedFromRaw(rmapped); ) \
        return res; \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_insert(C##_##X* self, C##_##X##_rawvalue_t raw) { \
        return SET_ONLY_##C( C##_##X##_insert_key(self, raw) ) \
               MAP_ONLY_##C( C##_##X##_emplace(self, raw.first, raw.second) ); \
    } \
    STC_INLINE void \
    C##_##X##_push_n(C##_##X* self, const C##_##X##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) C##_##X##_insert(self, arr[i]); \
    } \
\
    MAP_ONLY_##C( \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_put(C##_##X* self, RawKey rkey, RawMapped rmapped) { \
        C##_##X##_result_t res = C##_##X##_insert_key(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mappedFromRaw(rmapped); return res; \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_insert_or_assign(C##_##X* self, RawKey rkey, RawMapped rmapped) { \
        return C##_##X##_put(self, rkey, rmapped); \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_put_mapped(C##_##X* self, RawKey rkey, Mapped mapped) { \
        C##_##X##_result_t res = C##_##X##_insert_key(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mapped; return res; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_at(const C##_##X* self, RawKey rkey) { \
        C##_bucket_t b = C##_##X##_bucket(self, &rkey); \
        assert(self->_hashx[b.idx]); \
        return &self->table[b.idx].second; \
    }) \
\
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_begin(C##_##X* self) { \
        C##_##X##_iter_t it = {self->table, self->_hashx}; \
        if (it._hx) while (*it._hx == 0) ++it.ref, ++it._hx; \
        return it; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_end(C##_##X* self) {\
        C##_##X##_iter_t it = {self->table + self->bucket_count}; return it; \
    } \
    STC_INLINE void \
    C##_##X##_next(C##_##X##_iter_t* it) { \
        while ((++it->ref, *++it->_hx == 0)) ; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_itval(C##_##X##_iter_t it) {return SET_ONLY_##C( it.ref ) \
                                                 MAP_ONLY_##C( &it.ref->second );} \
\
    STC_API void \
    C##_##X##_erase_entry(C##_##X* self, C##_##X##_value_t* val); \
    STC_INLINE size_t \
    C##_##X##_erase(C##_##X* self, RawKey rkey) { \
        if (self->size == 0) return 0; \
        C##_bucket_t b = C##_##X##_bucket(self, &rkey); \
        return self->_hashx[b.idx] ? C##_##X##_erase_entry(self, self->table + b.idx), 1 : 0; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_erase_at(C##_##X* self, C##_##X##_iter_t pos) { \
        C##_##X##_erase_entry(self, pos.ref); \
        C##_##X##_next(&pos); return pos; \
    } \
\
    STC_API uint32_t c_default_hash(const void *data, size_t len); \
    STC_API uint32_t c_default_hash32(const void* data, size_t len); \
\
    _implement_CHASH(X, C, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef C##_##X C##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _implement_CHASH(X, C, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    STC_DEF C##_##X \
    C##_##X##_with_capacity(size_t cap) { \
        C##_##X h = _cmap_inits; \
        C##_##X##_reserve(&h, cap); \
        return h; \
    } \
\
    STC_INLINE void C##_##X##_wipe_(C##_##X* self) { \
        if (self->size == 0) return; \
        C##_##X##_value_t* e = self->table, *end = e + self->bucket_count; \
        uint8_t *hx = self->_hashx; \
        for (; e != end; ++e) if (*hx++) C##_##X##_entry_del(e); \
    } \
\
    STC_DEF void C##_##X##_del(C##_##X* self) { \
        C##_##X##_wipe_(self); \
        c_free(self->_hashx); \
        c_free(self->table); \
    } \
\
    STC_DEF void C##_##X##_clear(C##_##X* self) { \
        C##_##X##_wipe_(self); \
        self->size = 0; \
        memset(self->_hashx, 0, self->bucket_count); \
    } \
\
    STC_DEF C##_bucket_t \
    C##_##X##_bucket(const C##_##X* self, const C##_##X##_rawkey_t* rkeyptr) { \
        uint32_t sx, hash = keyHashRaw(rkeyptr, sizeof(C##_##X##_rawkey_t)); \
        size_t cap = self->bucket_count; \
        C##_bucket_t b = {chash_reduce(hash, cap), (hash & chash_HASH) | chash_USED}; \
        uint8_t* hashx = self->_hashx; \
        while ((sx = hashx[b.idx])) { \
            if (sx == b.hx) { \
                C##_##X##_rawkey_t r = keyToRaw(KEY_REF_##C(self->table + b.idx)); \
                if (keyEqualsRaw(&r, rkeyptr)) break; \
            } \
            if (++b.idx == cap) b.idx = 0; \
        } \
        return b; \
    } \
\
    STC_DEF C##_##X##_iter_t \
    C##_##X##_find(const C##_##X* self, RawKey rkey) { \
        C##_##X##_iter_t it = {NULL}; \
        if (self->size == 0) return it; \
        C##_bucket_t b = C##_##X##_bucket(self, &rkey); \
        if (*(it._hx = self->_hashx+b.idx)) it.ref = self->table+b.idx; \
        return it; \
    } \
\
    STC_DEF bool \
    C##_##X##_contains(const C##_##X* self, RawKey rkey) { \
        return self->size && self->_hashx[C##_##X##_bucket(self, &rkey).idx]; \
    } \
\
    STC_INLINE void C##_##X##_reserve_expand_(C##_##X* self) { \
        if (self->size + 1 >= self->bucket_count * self->max_load_factor) \
            C##_##X##_reserve(self, 5 + self->size * 3 / 2); \
    } \
    STC_DEF C##_##X##_result_t \
    C##_##X##_insert_key(C##_##X* self, RawKey rkey) { \
        C##_##X##_reserve_expand_(self); \
        C##_bucket_t b = C##_##X##_bucket(self, &rkey); \
        C##_##X##_result_t res = {&self->table[b.idx], !self->_hashx[b.idx]}; \
        if (res.second) { \
            *KEY_REF_##C(res.first) = keyFromRaw(rkey); \
            self->_hashx[b.idx] = (uint8_t) b.hx; \
            ++self->size; \
        } \
        return res; \
    } \
\
    STC_DEF C##_##X \
    C##_##X##_clone(C##_##X m) { \
        C##_##X clone = { \
            c_new_2(C##_##X##_value_t, m.bucket_count), \
            (uint8_t *) memcpy(c_malloc(m.bucket_count + 1), m._hashx, m.bucket_count + 1), \
            m.size, m.bucket_count, m.min_load_factor, m.max_load_factor \
        }; \
        C##_##X##_value_t *e = m.table, *end = e + m.bucket_count, *dst = clone.table; \
        for (uint8_t *hx = m._hashx; e != end; ++hx, ++e, ++dst) \
            if (*hx) *dst = C##_##X##_value_clone(*e); \
        return clone; \
    } \
\
    STC_DEF void \
    C##_##X##_reserve(C##_##X* self, size_t newcap) { \
        if (newcap < self->size) return; \
        size_t oldcap = self->bucket_count; \
        newcap = (size_t) (newcap / self->max_load_factor) | 1; \
        C##_##X tmp = { \
            c_new_2 (C##_##X##_value_t, newcap), \
            (uint8_t *) c_calloc(newcap + 1, sizeof(uint8_t)), \
            self->size, (uint32_t) newcap, \
            self->min_load_factor, self->max_load_factor \
        }; \
        /* Rehash: */ \
        tmp._hashx[newcap] = 0xff; c_swap(C##_##X, *self, tmp); \
        C##_##X##_value_t* e = tmp.table, *slot = self->table; \
        uint8_t* hashx = self->_hashx; \
        for (size_t i = 0; i < oldcap; ++i, ++e) \
            if (tmp._hashx[i]) { \
                RawKey r = keyToRaw(KEY_REF_##C(e)); \
                C##_bucket_t b = C##_##X##_bucket(self, &r); \
                slot[b.idx] = *e, \
                hashx[b.idx] = (uint8_t) b.hx; \
            } \
        c_free(tmp._hashx); \
        c_free(tmp.table); \
    } \
\
    STC_DEF void \
    C##_##X##_erase_entry(C##_##X* self, C##_##X##_value_t* val) { \
        size_t i = chash_entry_index(*self, val), j = i, k, cap = self->bucket_count; \
        C##_##X##_value_t* slot = self->table; \
        uint8_t* hashx = self->_hashx; \
        C##_##X##_entry_del(&slot[i]); \
        do { /* deletion from hash table without tombstone */ \
            if (++j == cap) j = 0; /* ++j; j %= cap; is slow */ \
            if (! hashx[j]) \
                break; \
            RawKey r = keyToRaw(KEY_REF_##C(slot + j)); \
            k = chash_reduce(keyHashRaw(&r, sizeof(RawKey)), cap); \
            if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
                slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
        } while (true); \
        hashx[i] = 0, k = --self->size; \
        if ((float) k / cap < self->min_load_factor && k > 512) \
            C##_##X##_reserve(self, k*1.2); \
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
#define _implement_CHASH(X, C, Key, Mapped, keyEqualsRaw, keyHashRaw, mappedDel, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

#endif
