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
#ifndef CMAP_H_INCLUDED
#define CMAP_H_INCLUDED

// Unordered set/map - implemented as closed hashing with linear probing and no tombstones.
/*
#include <stc/cmap.h>
#include <stdio.h>

using_cmap(mx, int, char); // Map of int -> char

int main(void) {
    c_forvar (cmap_mx m = cmap_mx_init(), cmap_mx_del(&m))
    {
        cmap_mx_emplace(&m, 5, 'a');
        cmap_mx_emplace(&m, 8, 'b');
        cmap_mx_emplace(&m, 12, 'c');

        cmap_mx_iter_t it = cmap_mx_find(&m, 10); // none
        char val = cmap_mx_find(&m, 5).ref->second;
        cmap_mx_emplace_or_assign(&m, 5, 'd'); // update
        cmap_mx_erase(&m, 8);

        c_foreach (i, cmap_mx, m)
            printf("map %d: %c\n", i.ref->first, i.ref->second);
    }
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cmap(...) c_MACRO_OVERLOAD(using_cmap, __VA_ARGS__)

#define using_cmap_3(X, Key, Mapped) \
            using_cmap_5(X, Key, Mapped, c_default_equals, c_default_hash)
#define using_cmap_5(X, Key, Mapped, keyEquals, keyHash) \
            using_cmap_7(X, Key, Mapped, keyEquals, keyHash, \
                         c_default_del, c_default_fromraw)
#define using_cmap_6(X, Key, Mapped, keyEquals, keyHash, mappedDel) \
            using_cmap_7(X, Key, Mapped, keyEquals, keyHash, \
                         mappedDel, c_no_clone)
#define using_cmap_7(X, Key, Mapped, keyEquals, keyHash, mappedDel, mappedClone) \
            using_cmap_10(X, Key, Mapped, keyEquals, keyHash, \
                         mappedDel, mappedClone, c_default_toraw, Mapped, c_true)
#define using_cmap_10(X, Key, Mapped, keyEquals, keyHash, \
                      mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            using_cmap_14(X, Key, Mapped, keyEquals, keyHash, \
                          mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                          c_default_del, c_default_fromraw, c_default_toraw, Key)
#define using_cmap_14(X, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                      mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                      keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_chash(cmap_##X, cmap_, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                           keyDel, keyFromRaw, keyToRaw, RawKey)


#define using_cmap_keydef(...) c_MACRO_OVERLOAD(using_cmap_keydef, __VA_ARGS__)

#define using_cmap_keydef_7(X, Key, Mapped, keyEquals, keyHash, keyDel, keyClone) \
            using_cmap_keydef_10(X, Key, Mapped, keyEquals, keyHash, \
                                keyDel, keyClone, c_default_toraw, Key, c_true)
#define using_cmap_keydef_10(X, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                            keyDel, keyFromRaw, keyToRaw, RawKey, defTypes) \
            _c_using_chash(cmap_##X, cmap_, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                           c_default_del, c_default_fromraw, c_default_toraw, Mapped, defTypes, \
                           keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_cmap_str() \
            _c_using_chash(cmap_str, cmap_, cstr, cstr, c_rawstr_equals, c_rawstr_hash, \
                           cstr_del, cstr_from, cstr_str, const char*, c_true, \
                           cstr_del, cstr_from, cstr_str, const char*)


#define using_cmap_strkey(...) c_MACRO_OVERLOAD(using_cmap_strkey, __VA_ARGS__)

#define using_cmap_strkey_2(X, Mapped) \
            using_cmap_strkey_4(X, Mapped, c_default_del, c_default_fromraw)
#define using_cmap_strkey_3(X, Mapped, mappedDel) \
            using_cmap_strkey_4(X, Mapped, mappedDel, c_no_clone)
#define using_cmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
            using_cmap_strkey_7(X, Mapped, mappedDel, mappedClone, c_default_toraw, Mapped, c_true)
#define using_cmap_strkey_7(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            _c_using_chash_strkey(X, cmap_, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes)
#define _c_using_chash_strkey(X, C, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            _c_using_chash(C##X, C, cstr, Mapped, c_rawstr_equals, c_rawstr_hash, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                           cstr_del, cstr_from, cstr_str, const char*)


#define using_cmap_strval(...) c_MACRO_OVERLOAD(using_cmap_strval, __VA_ARGS__)

#define using_cmap_strval_2(X, Key) \
            using_cmap_strval_4(X, Key, c_default_equals, c_default_hash)
#define using_cmap_strval_4(X, Key, keyEquals, keyHash) \
            using_cmap_strval_6(X, Key, keyEquals, keyHash, c_default_del, c_default_fromraw)
#define using_cmap_strval_5(X, Key, keyEquals, keyHash, keyDel) \
            using_cmap_strval_6(X, Key, keyEquals, keyHash, keyDel, c_no_clone)
#define using_cmap_strval_6(X, Key, keyEquals, keyHash, keyDel, keyClone) \
            using_cmap_strval_9(X, Key, keyEquals, keyHash, keyDel, keyClone, c_default_toraw, Key, c_true)
#define using_cmap_strval_9(X, Key, keyEqualsRaw, keyHashRaw, keyDel, keyFromRaw, keyToRaw, RawKey, defTypes) \
            _c_using_chash(cmap_##X, cmap_, Key, cstr, keyEqualsRaw, keyHashRaw, \
                           cstr_del, cstr_from, cstr_str, const char*, defTypes, \
                           keyDel, keyFromRaw, keyToRaw, RawKey)

#define SET_ONLY_cmap_(...)
#define MAP_ONLY_cmap_(...) __VA_ARGS__
#define KEY_REF_cmap_(vp)   (&(vp)->first)
#ifndef CMAP_SIZE_T
#define CMAP_SIZE_T uint32_t
#endif
#define _cmap_inits {NULL, NULL, 0, 0, 0.85f}
typedef struct      {size_t idx; uint_fast8_t hx;} chash_bucket_t; \

STC_API    uint64_t c_default_hash(const void *data, size_t len);
STC_INLINE uint64_t c_string_hash(const char *s)
    {return c_default_hash(s, strlen(s));}
STC_INLINE uint64_t c_default_hash32(const void* data, size_t ignored)
    {return *(const uint32_t *)data * 0xc6a4a7935bd1e99d;}
STC_INLINE uint64_t c_default_hash64(const void* data, size_t ignored)
    {return *(const uint64_t *)data * 0xc6a4a7935bd1e99d;}

#define _c_chash_types(CX, C, Key, Mapped) \
    typedef Key CX##_key_t; \
    typedef Mapped CX##_mapped_t; \
    typedef CMAP_SIZE_T CX##_size_t; \
\
    typedef SET_ONLY_##C( CX##_key_t ) \
            MAP_ONLY_##C( struct CX##_value_t ) \
    CX##_value_t; \
\
    typedef struct { \
        CX##_value_t *ref; \
        bool inserted; \
    } CX##_result_t; \
\
    typedef struct { \
        CX##_value_t *ref; \
        uint8_t* _hx; \
    } CX##_iter_t; \
\
    typedef struct { \
        CX##_value_t* table; \
        uint8_t* _hashx; \
        CX##_size_t size, bucket_count; \
        float max_load_factor; \
    } CX

#define _c_using_chash(CX, C, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                       mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
    defTypes( _c_chash_types(CX, C, Key, Mapped); ) \
\
    MAP_ONLY_##C( struct CX##_value_t { \
        CX##_key_t first; \
        CX##_mapped_t second; \
    }; ) \
\
    typedef RawKey CX##_rawkey_t; \
    typedef RawMapped CX##_rawmapped_t; \
    typedef SET_ONLY_##C( RawKey ) \
            MAP_ONLY_##C( struct { RawKey first; \
                                   RawMapped second; } ) \
    CX##_rawvalue_t; \
\
    STC_API CX               CX##_with_capacity(size_t cap); \
    STC_API CX               CX##_clone(CX map); \
    STC_API void             CX##_del(CX* self); \
    STC_API void             CX##_clear(CX* self); \
    STC_API void             CX##_reserve(CX* self, size_t capacity); \
    STC_API chash_bucket_t   CX##_bucket_(const CX* self, const CX##_rawkey_t* rkeyptr); \
    STC_API CX##_result_t    CX##_insert_entry_(CX* self, RawKey rkey); \
    STC_API void             CX##_erase_entry(CX* self, CX##_value_t* val); \
\
    STC_INLINE CX            CX##_init(void) {return c_make(CX)_cmap_inits;} \
    STC_INLINE void          CX##_shrink_to_fit(CX* self) {CX##_reserve(self, self->size);} \
    STC_INLINE void          CX##_max_load_factor(CX* self, float ml) {self->max_load_factor = ml;} \
    STC_INLINE bool          CX##_empty(CX m) {return m.size == 0;} \
    STC_INLINE size_t        CX##_size(CX m) {return m.size;} \
    STC_INLINE size_t        CX##_bucket_count(CX map) {return map.bucket_count;} \
    STC_INLINE size_t        CX##_capacity(CX map) \
                                {return (size_t) (map.bucket_count * map.max_load_factor);} \
    STC_INLINE void          CX##_swap(CX *map1, CX *map2) {c_swap(CX, *map1, *map2);} \
    STC_INLINE bool          CX##_contains(const CX* self, RawKey rkey) \
                                {return self->size && self->_hashx[CX##_bucket_(self, &rkey).idx];} \
\
    STC_INLINE void \
    CX##_value_clone(CX##_value_t* _dst, CX##_value_t* _val) { \
        *KEY_REF_##C(_dst) = keyFromRaw(keyToRaw(KEY_REF_##C(_val))); \
        MAP_ONLY_##C( _dst->second = mappedFromRaw(mappedToRaw(&_val->second)); ) \
    } \
\
    STC_INLINE CX##_rawvalue_t \
    CX##_value_toraw(CX##_value_t* val) { \
        return SET_ONLY_##C( keyToRaw(val) ) \
               MAP_ONLY_##C( c_make(CX##_rawvalue_t){keyToRaw(&val->first), mappedToRaw(&val->second)} ); \
    } \
\
    STC_INLINE void \
    CX##_value_del(CX##_value_t* _val) { \
        keyDel(KEY_REF_##C(_val)); \
        MAP_ONLY_##C( mappedDel(&_val->second); ) \
    } \
\
    STC_INLINE CX##_result_t \
    CX##_emplace(CX* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        CX##_result_t _res = CX##_insert_entry_(self, rkey); \
        if (_res.inserted) { \
            *KEY_REF_##C(_res.ref) = keyFromRaw(rkey); \
            MAP_ONLY_##C(_res.ref->second = mappedFromRaw(rmapped);) \
        } \
        return _res; \
    } \
\
    STC_INLINE void \
    CX##_emplace_items(CX* self, const CX##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) SET_ONLY_##C( CX##_emplace(self, arr[i]); ) \
                                   MAP_ONLY_##C( CX##_emplace(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE CX##_result_t \
    CX##_insert(CX* self, Key _key MAP_ONLY_##C(, Mapped _mapped)) { \
        CX##_result_t _res = CX##_insert_entry_(self, keyToRaw(&_key)); \
        if (_res.inserted) {*KEY_REF_##C(_res.ref) = _key; MAP_ONLY_##C( _res.ref->second = _mapped; )} \
        else              {keyDel(&_key); MAP_ONLY_##C( mappedDel(&_mapped); )} \
        return _res; \
    } \
\
    MAP_ONLY_##C( \
        STC_INLINE CX##_result_t \
        CX##_insert_or_assign(CX* self, Key _key, Mapped _mapped) { \
            CX##_result_t _res = CX##_insert_entry_(self, keyToRaw(&_key)); \
            if (_res.inserted) _res.ref->first = _key; \
            else {keyDel(&_key); mappedDel(&_res.ref->second);} \
            _res.ref->second = _mapped; return _res; \
        } \
    \
        STC_INLINE CX##_result_t \
        CX##_put(CX* self, Key k, Mapped m) { /* shorter, like operator[] */ \
            return CX##_insert_or_assign(self, k, m); \
        } \
    \
        STC_INLINE CX##_result_t \
        CX##_emplace_or_assign(CX* self, RawKey rkey, RawMapped rmapped) { \
            CX##_result_t _res = CX##_insert_entry_(self, rkey); \
            if (_res.inserted) _res.ref->first = keyFromRaw(rkey); \
            else mappedDel(&_res.ref->second); \
            _res.ref->second = mappedFromRaw(rmapped); return _res; \
        } \
    \
        STC_INLINE CX##_mapped_t* \
        CX##_at(const CX* self, RawKey rkey) { \
            chash_bucket_t b = CX##_bucket_(self, &rkey); \
            return &self->table[b.idx].second; \
        }) \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawKey rkey) { \
        CX##_iter_t it = {NULL}; \
        if (self->size == 0) return it; \
        chash_bucket_t b = CX##_bucket_(self, &rkey); \
        if (*(it._hx = self->_hashx+b.idx)) it.ref = self->table+b.idx; \
        return it; \
    } \
\
    STC_INLINE CX##_value_t* \
    CX##_get(const CX* self, RawKey rkey) \
        {return CX##_find(self, rkey).ref;} \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_iter_t it = {self->table, self->_hashx}; \
        if (it._hx) while (*it._hx == 0) ++it.ref, ++it._hx; \
        return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) \
        {return c_make(CX##_iter_t){self->table + self->bucket_count};} \
\
    STC_INLINE void \
    CX##_next(CX##_iter_t* it) \
        {while ((++it->ref, *++it->_hx == 0)) ;} \
\
    STC_INLINE size_t \
    CX##_erase(CX* self, RawKey rkey) { \
        if (self->size == 0) return 0; \
        chash_bucket_t b = CX##_bucket_(self, &rkey); \
        return self->_hashx[b.idx] ? CX##_erase_entry(self, self->table + b.idx), 1 : 0; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t it) { \
        CX##_erase_entry(self, it.ref); \
        if (*it._hx == 0) CX##_next(&it); \
        return it; \
    } \
\
    _c_implement_chash(CX, C, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                       mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#ifdef c_umul128
STC_INLINE size_t fastrange_uint64_t(uint64_t x, uint64_t n) \
    {uint64_t l, h; c_umul128(x, n, &l, &h); return h;}
#endif
#define fastrange_uint32_t(x, n) ((size_t) (((uint32_t)(x)*(uint64_t)(n)) >> 32))
#define chash_index_(h, entryPtr) ((entryPtr) - (h).table)


#define _c_implement_chash(CX, C, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                           keyDel, keyFromRaw, keyToRaw, RawKey) \
    STC_DEF CX \
    CX##_with_capacity(size_t cap) { \
        CX h = _cmap_inits; \
        CX##_reserve(&h, cap); \
        return h; \
    } \
\
    STC_INLINE void CX##_wipe_(CX* self) { \
        if (self->size == 0) return; \
        CX##_value_t* e = self->table, *end = e + self->bucket_count; \
        uint8_t *hx = self->_hashx; \
        for (; e != end; ++e) if (*hx++) CX##_value_del(e); \
    } \
\
    STC_DEF void CX##_del(CX* self) { \
        CX##_wipe_(self); \
        c_free(self->_hashx); \
        c_free((void *) self->table); \
    } \
\
    STC_DEF void CX##_clear(CX* self) { \
        CX##_wipe_(self); \
        self->size = 0; \
        memset(self->_hashx, 0, self->bucket_count); \
    } \
\
    STC_DEF chash_bucket_t \
    CX##_bucket_(const CX* self, const CX##_rawkey_t* rkeyptr) { \
        const uint64_t _hash = keyHashRaw(rkeyptr, sizeof *rkeyptr); \
        uint_fast8_t _hx; size_t _cap = self->bucket_count; \
        chash_bucket_t b = {c_SELECT(fastrange,CMAP_SIZE_T)(_hash, _cap), (uint_fast8_t)(_hash | 0x80)}; \
        const uint8_t* _hashx = self->_hashx; \
        while ((_hx = _hashx[b.idx])) { \
            if (_hx == b.hx) { \
                CX##_rawkey_t _raw = keyToRaw(KEY_REF_##C(self->table + b.idx)); \
                if (keyEqualsRaw(&_raw, rkeyptr)) break; \
            } \
            if (++b.idx == _cap) b.idx = 0; \
        } \
        return b; \
    } \
\
    STC_DEF CX##_result_t \
    CX##_insert_entry_(CX* self, RawKey rkey) { \
        if (self->size + 1 >= (CX##_size_t) (self->bucket_count * self->max_load_factor)) \
            CX##_reserve(self, 8 + (self->size*13ull >> 3)); \
        chash_bucket_t b = CX##_bucket_(self, &rkey); \
        CX##_result_t res = {&self->table[b.idx], !self->_hashx[b.idx]}; \
        if (res.inserted) { \
            self->_hashx[b.idx] = b.hx; \
            ++self->size; \
        } \
        return res; \
    } \
\
    STC_DEF CX \
    CX##_clone(CX m) { \
        CX clone = { \
            c_new_n(CX##_value_t, m.bucket_count), \
            (uint8_t *) memcpy(c_malloc(m.bucket_count + 1), m._hashx, m.bucket_count + 1), \
            m.size, m.bucket_count, \
            m.max_load_factor \
        }; \
        CX##_value_t *e = m.table, *end = e + m.bucket_count, *dst = clone.table; \
        for (uint8_t *hx = m._hashx; e != end; ++hx, ++e, ++dst) \
            if (*hx) CX##_value_clone(dst, e); \
        return clone; \
    } \
\
    STC_DEF void \
    CX##_reserve(CX* self, size_t _newcap) { \
        if (_newcap < self->size) return; \
        size_t _oldcap = self->bucket_count; \
        _newcap = (size_t) (2 + _newcap / self->max_load_factor) | 1; \
        CX _tmp = { \
            c_new_n(CX##_value_t, _newcap), \
            (uint8_t *) c_calloc(_newcap + 1, sizeof(uint8_t)), \
            self->size, (CX##_size_t) _newcap, \
            self->max_load_factor \
        }; \
        /* Rehash: */ \
        _tmp._hashx[_newcap] = 0xff; c_swap(CX, *self, _tmp); \
        CX##_value_t* e = _tmp.table, *_slot = self->table; \
        uint8_t* _hashx = self->_hashx; \
        for (size_t i = 0; i < _oldcap; ++i, ++e) \
            if (_tmp._hashx[i]) { \
                CX##_rawkey_t _raw = keyToRaw(KEY_REF_##C(e)); \
                chash_bucket_t b = CX##_bucket_(self, &_raw); \
                memcpy((void *) &_slot[b.idx], e, sizeof *e); \
                _hashx[b.idx] = (uint8_t) b.hx; \
            } \
        c_free(_tmp._hashx); \
        c_free((void *) _tmp.table); \
    } \
\
    STC_DEF void \
    CX##_erase_entry(CX* self, CX##_value_t* _val) { \
        size_t i = chash_index_(*self, _val), j = i, k, _cap = self->bucket_count; \
        CX##_value_t* _slot = self->table; \
        uint8_t* _hashx = self->_hashx; \
        CX##_value_del(&_slot[i]); \
        for (;;) { /* delete without leaving tombstone */ \
            if (++j == _cap) j = 0; \
            if (! _hashx[j]) \
                break; \
            CX##_rawkey_t _raw = keyToRaw(KEY_REF_##C(_slot+j)); \
            k = c_SELECT(fastrange,CMAP_SIZE_T)(keyHashRaw(&_raw, sizeof _raw), _cap); \
            if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
                memcpy((void *) &_slot[i], &_slot[j], sizeof *_slot), _hashx[i] = _hashx[j], i = j; \
        } \
        _hashx[i] = 0; \
        --self->size; \
    }


STC_DEF uint64_t c_default_hash(const void *key, size_t len) {
    const uint64_t m = 0xb5ad4eceda1ce2a9;
    uint64_t k, h = m + len;
    const uint8_t *p = (const uint8_t *)key, *end = p + (len & ~7ull);
    for (; p != end; p += 8) {memcpy(&k, p, 8); h ^= m*k;}
    switch (len & 7) {
        case 7: h ^= (uint64_t) p[6] << 48;
        case 6: h ^= (uint64_t) p[5] << 40;
        case 5: h ^= (uint64_t) p[4] << 32;
        case 4: h ^= (uint64_t) p[3] << 24;
        case 3: h ^= (uint64_t) p[2] << 16;
        case 2: h ^= (uint64_t) p[1] << 8;
        case 1: h ^= (uint64_t) p[0]; h *= m;
    }
    return h ^ (h >> 15);
}

#else
#define _c_implement_chash(CX, C, Key, Mapped, keyEqualsRaw, keyHashRaw, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                           keyDel, keyFromRaw, keyToRaw, RawKey)
#endif

#endif
