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
#ifndef CSMAP__H__
#define CSMAP__H__

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#include <stc/csmap.h>
using_csset(i, int);        // Set of int
using_csmap(ic, int, char); // Map of int -> char

int main(void) {
    csset_sx s = csset_sx_init();
    csset_sx_insert(&s, 5);
    csset_sx_insert(&s, 8);
    c_foreach (i, csset_sx, s)
        printf("set %d\n", i.ref->second);
    csset_sx_del(&s);
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_csmap(...) \
    c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
    using_csmap_4(X, Key, Mapped, c_default_compare)

#define using_csmap_4(X, Key, Mapped, keyCompare) \
    using_csmap_6(X, Key, Mapped, keyCompare, c_default_del, c_default_clone)

#define using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, mappedClone) \
    using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, c_default_del, c_default_clone)

#define using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, keyDel, keyClone) \
    using_csmap_10(X, Key, Mapped, keyCompare, mappedDel, mappedClone, \
                      keyDel, keyClone, c_default_to_raw, Key)

#define using_csmap_10(X, Key, Mapped, keyCompareRaw, mappedDel, mappedClone, \
                          keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csmap, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, mappedClone, c_default_to_raw, Mapped)

/* csset: */
#define using_csset(...) \
    c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
    using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
    using_csset_5(X, Key, keyCompare, c_default_del, c_default_clone)

#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csset_7(X, Key, keyCompare, keyDel, keyClone, c_default_to_raw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csset, Key, Key, keyCompareRaw, _UNUSED_, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, _UNUSED_, _UNUSED_, void)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
    _using_CBST_strkey(str, csset, cstr_t, _UNUSED_, _UNUSED_)
#define using_csmap_str() \
    _using_CBST(str, csmap, cstr_t, cstr_t, cstr_compare_raw, cstr_del, cstr_del, \
                     cstr_from, cstr_to_raw, const char*, cstr_from, cstr_to_raw, const char*)

#define using_csmap_strkey(...) \
    c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
    _using_CBST_strkey(X, csmap, Mapped, c_default_del, c_default_clone)

#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
    _using_CBST_strkey(X, csmap, Mapped, mappedDel, mappedClone)

#define _using_CBST_strkey(X, C, Mapped, mappedDel, mappedClone) \
    _using_CBST(X, C, cstr_t, Mapped, cstr_compare_raw, mappedDel, cstr_del, \
                   cstr_from, cstr_to_raw, const char*, mappedClone, c_default_to_raw, Mapped)

#define using_csmap_strval(...) \
    c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
    using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
    using_csmap_strval_5(X, Key, keyCompare, c_default_del, c_default_clone)

#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_default_to_raw, Key)

#define using_csmap_strval_7(X, Key, keyCompare, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csmap, Key, cstr_t, keyCompare, cstr_del, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, cstr_from, cstr_to_raw, const char*)

#define SET_ONLY_csset(...) __VA_ARGS__
#define SET_ONLY_csmap(...)
#define MAP_ONLY_csset(...)
#define MAP_ONLY_csmap(...) __VA_ARGS__
#define KEY_REF_csset(vp)   (vp)
#define KEY_REF_csmap(vp)   (&(vp)->first)
#ifndef CSMAP_SIZE_T
#define CSMAP_SIZE_T uint32_t
#endif

#define _using_CBST_types(X, C, Key, Mapped) \
    typedef Key C##_##X##_key_t; \
    typedef Mapped C##_##X##_mapped_t; \
    typedef CSMAP_SIZE_T C##_##X##_size_t; \
\
    typedef SET_ONLY_##C( C##_##X##_key_t ) \
            MAP_ONLY_##C( struct {C##_##X##_key_t first; \
                                  C##_##X##_mapped_t second;} ) \
    C##_##X##_value_t; \
\
    typedef struct C##_##X##_node { \
        C##_##X##_size_t link[2]; \
        int8_t level; \
        C##_##X##_value_t value; \
    } C##_##X##_node_t; \
\
    typedef struct { \
        C##_##X##_value_t *ref; \
        C##_##X##_node_t *_d; \
        int _top; \
        C##_##X##_size_t _tn, _st[48]; \
    } C##_##X##_iter_t

#define _using_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                       keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_CBST_types(X, C, Key, Mapped); \
\
    typedef struct { \
        C##_##X##_node_t* data; \
    } C##_##X; \
\
    typedef RawKey C##_##X##_rawkey_t; \
    typedef RawMapped C##_##X##_rawmapped_t; \
    typedef SET_ONLY_##C( C##_##X##_rawkey_t ) \
            MAP_ONLY_##C( struct {C##_##X##_rawkey_t first; \
                                  C##_##X##_rawmapped_t second;} ) \
    C##_##X##_rawvalue_t; \
\
    typedef struct { \
        C##_##X##_value_t *first; \
        bool second; \
    } C##_##X##_result_t; \
\
    STC_API C##_##X C##_##X##_init(void); \
    STC_API C##_##X C##_##X##_clone(C##_##X bst); \
\
    STC_API void \
    C##_##X##_reserve(C##_##X* self, size_t cap); \
    STC_INLINE C##_##X \
    C##_##X##_with_capacity(size_t size) { \
        C##_##X x = C##_##X##_init(); \
        C##_##X##_reserve(&x, size); \
        return x; \
    } \
    STC_INLINE bool \
    C##_##X##_empty(C##_##X m) {return _smap_size(&m) == 0;} \
    STC_INLINE size_t \
    C##_##X##_size(C##_##X m) {return _smap_size(&m);} \
    STC_API void \
    C##_##X##_del(C##_##X* self); \
    STC_INLINE void \
    C##_##X##_clear(C##_##X* self) {C##_##X##_del(self); *self = C##_##X##_init();} \
    STC_INLINE void \
    C##_##X##_swap(C##_##X* a, C##_##X* b) {c_swap(C##_##X, *a, *b);} \
\
    STC_INLINE void \
    C##_##X##_value_del(C##_##X##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE C##_##X##_value_t \
    C##_##X##_value_clone(C##_##X##_value_t val) { \
        *KEY_REF_##C(&val) = keyFromRaw(keyToRaw(KEY_REF_##C(&val))); \
        MAP_ONLY_##C( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
\
    STC_API C##_##X##_value_t* \
    C##_##X##_find_it(const C##_##X* self, RawKey rkey, C##_##X##_iter_t* out); \
\
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_find(const C##_##X* self, RawKey rkey) { \
        C##_##X##_iter_t it; \
        C##_##X##_find_it(self, rkey, &it); \
        return it; \
    } \
    STC_INLINE bool \
    C##_##X##_contains(const C##_##X* self, RawKey rkey) { \
        C##_##X##_iter_t it; \
        return C##_##X##_find_it(self, rkey, &it) != NULL; \
    } \
\
    STC_API C##_##X##_result_t \
    C##_##X##_insert_key(C##_##X* self, RawKey rkey); \
\
    STC_INLINE C##_##X##_result_t \
    C##_##X##_emplace(C##_##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped) ) { \
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
        C##_##X##_iter_t it; \
        return &C##_##X##_find_it(self, rkey, &it)->second; \
    }) \
\
    STC_API C##_##X##_value_t* C##_##X##_front(C##_##X* self); \
    STC_API C##_##X##_value_t* C##_##X##_back(C##_##X* self); \
    STC_API void C##_##X##_next(C##_##X##_iter_t* it); \
\
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_begin(C##_##X* self) { \
        C##_##X##_iter_t it = {NULL, self->data, 0, (C##_##X##_size_t) _smap_root(self)}; \
        if (it._tn) C##_##X##_next(&it); \
        return it; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_end(C##_##X* self) {\
        C##_##X##_iter_t it = {NULL}; return it; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_itval(C##_##X##_iter_t it) {return SET_ONLY_##C( it.ref ) \
                                                 MAP_ONLY_##C( &it.ref->second );} \
    STC_API int \
    C##_##X##_erase(C##_##X* self, RawKey rkey); \
\
    STC_INLINE int \
    C##_##X##_erase_at(C##_##X* self, C##_##X##_iter_t pos) { \
        return C##_##X##_erase(self, keyToRaw(KEY_REF_##C(pos.ref))); \
    } \
\
    _implement_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                       keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef C##_##X C##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _implement_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                           keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    STC_DEF C##_##X \
    C##_##X##_init(void) { \
        C##_##X m = {(C##_##X##_node_t *) (_smap_inits + 4)}; \
        return m; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_front(C##_##X* self) { \
        C##_##X##_node_t *d = self->data; \
        C##_##X##_size_t tn = (C##_##X##_size_t) _smap_root(self); \
        while (d[tn].link[0]) tn = d[tn].link[0]; \
        return &d[tn].value; \
    } \
    STC_DEF C##_##X##_value_t* \
    C##_##X##_back(C##_##X* self) { \
        C##_##X##_node_t *d = self->data; \
        C##_##X##_size_t tn = (C##_##X##_size_t) _smap_root(self); \
        while (d[tn].link[1]) tn = d[tn].link[1]; \
        return &d[tn].value; \
    } \
\
    STC_DEF void \
    C##_##X##_reserve(C##_##X* self, size_t cap) { \
        C##_##X##_size_t oldcap = _smap_cap(self); \
        if (cap > oldcap) { \
            size_t* rep = (size_t *) c_realloc(oldcap ? _smap_rep(self) : NULL, \
                                               4*sizeof(size_t) + (cap + 1)*sizeof(C##_##X##_node_t)); \
            if (oldcap == 0) \
                memset(rep, 0, sizeof(size_t)*4 + sizeof(C##_##X##_node_t)); \
            rep[_smap_CAP] = cap; \
            self->data = (C##_##X##_node_t *) (rep + 4); \
        } \
    } \
\
    STC_DEF C##_##X##_size_t \
    C##_##X##_node_new_(C##_##X* self) { \
        size_t tn, *rep = _smap_rep(self); \
        if (rep[_smap_DISP]) { \
            tn = rep[_smap_DISP]; \
            rep[_smap_DISP] = self->data[tn].link[1]; \
        } else if ((tn = rep[_smap_SIZE] + 1) > rep[_smap_CAP]) \
            C##_##X##_reserve(self, 4 + tn*3/2); \
        C##_##X##_node_t* dn = &self->data[tn]; \
        dn->link[0] = dn->link[1] = 0; dn->level = 1; \
        return (C##_##X##_size_t) tn; \
    } \
\
    STC_DEF void \
    C##_##X##_node_del_(C##_##X##_node_t *d, C##_##X##_size_t tn) { \
        size_t *rep = ((size_t *) d) - 4; \
        keyDel(KEY_REF_##C(&d[tn].value)); \
        d[tn].link[1] = (C##_##X##_size_t) rep[_smap_DISP]; \
        rep[_smap_DISP] = tn; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_find_it(const C##_##X* self, C##_##X##_rawkey_t rkey, C##_##X##_iter_t* out) { \
        C##_##X##_size_t tn = _smap_root(self); \
        C##_##X##_node_t *d = out->_d = self->data; \
        out->_top = 0; \
        while (tn) { \
            C##_##X##_rawkey_t rx = keyToRaw(KEY_REF_##C(&d[tn].value)); \
            switch (keyCompareRaw(&rx, &rkey)) { \
                case -1: tn = d[tn].link[1]; break; \
                case 1: out->_st[out->_top++] = tn; tn = d[tn].link[0]; break; \
                case 0: out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); \
            } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF void \
    C##_##X##_next(C##_##X##_iter_t *it) { \
        C##_##X##_size_t tn = it->_tn; \
        if (it->_top || tn) { \
            while (tn) { \
                it->_st[it->_top++] = tn; \
                tn = it->_d[tn].link[0]; \
            } \
            tn = it->_st[--it->_top]; \
            it->_tn = it->_d[tn].link[1]; \
            it->ref = &it->_d[tn].value; \
        } else \
            it->ref = NULL; \
    } \
\
    static C##_##X##_size_t \
    C##_##X##_skew_(C##_##X##_node_t *d, C##_##X##_size_t tn) { \
        if (tn && d[d[tn].link[0]].level == d[tn].level) { \
            C##_##X##_size_t tmp = d[tn].link[0]; \
            d[tn].link[0] = d[tmp].link[1]; \
            d[tmp].link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
    static C##_##X##_size_t \
    C##_##X##_split_(C##_##X##_node_t *d, C##_##X##_size_t tn) { \
        if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) { \
            C##_##X##_size_t tmp = d[tn].link[1]; \
            d[tn].link[1] = d[tmp].link[0]; \
            d[tmp].link[0] = tn; \
            tn = tmp; \
            ++d[tn].level; \
        } \
        return tn; \
    } \
\
    static inline C##_##X##_size_t \
    C##_##X##_insert_key_i_(C##_##X* self, C##_##X##_size_t tn, const C##_##X##_rawkey_t* rkey, C##_##X##_result_t* res) { \
        C##_##X##_size_t up[64], it = tn; \
        C##_##X##_node_t* d = self->data; \
        int c, top = 0, dir = 0; \
        while (it) { \
            up[top++] = it; \
            C##_##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[it].value)); \
            if ((c = keyCompareRaw(&raw, rkey)) == 0) {res->first = &d[it].value; return tn;} \
            dir = (c == -1); \
            it = d[it].link[dir]; \
        } \
        it = C##_##X##_node_new_(self); d = self->data; \
        *KEY_REF_##C(&d[it].value) = keyFromRaw(*rkey); \
        res->first = &d[it].value, res->second = true; \
        if (top == 0) return it; \
        d[up[top - 1]].link[dir] = it; \
        while (top--) { \
            if (top) dir = (d[up[top - 1]].link[1] == up[top]); \
            up[top] = C##_##X##_skew_(d, up[top]); \
            up[top] = C##_##X##_split_(d, up[top]); \
            if (top) d[up[top - 1]].link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF C##_##X##_result_t \
    C##_##X##_insert_key(C##_##X* self, RawKey rkey) { \
        C##_##X##_result_t res = {NULL, false}; \
        C##_##X##_size_t tn = C##_##X##_insert_key_i_(self, (C##_##X##_size_t) _smap_root(self), &rkey, &res); \
        _smap_root(self) = tn; \
        _smap_size(self) += res.second; \
        return res; \
    } \
\
    static C##_##X##_size_t \
    C##_##X##_erase_r_(C##_##X##_node_t *d, C##_##X##_size_t tn, const C##_##X##_rawkey_t* rkey, int *erased) { \
        if (tn == 0) return 0; \
        C##_##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
        C##_##X##_size_t tx; int c = keyCompareRaw(&raw, rkey); \
        if (c != 0) \
            d[tn].link[c == -1] = C##_##X##_erase_r_(d, d[tn].link[c == -1], rkey, erased); \
        else { \
            if (!*erased) {C##_##X##_value_del(&d[tn].value); *erased = 1;} \
            if (d[tn].link[0] && d[tn].link[1]) { \
                tx = d[tn].link[0]; \
                while (d[tx].link[1]) \
                    tx = d[tx].link[1]; \
                d[tn].value = d[tx].value; /* move */ \
                raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
                d[tn].link[0] = C##_##X##_erase_r_(d, d[tn].link[0], &raw, erased); \
            } else { /* unlink node */ \
                tx = tn; \
                tn = d[tn].link[ d[tn].link[0] == 0 ]; \
                C##_##X##_node_del_(d, tx); \
            } \
        } \
        tx = d[tn].link[1]; \
        if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) { \
            if (d[tx].level > --d[tn].level) \
                d[tx].level = d[tn].level; \
                            tn = C##_##X##_skew_(d, tn); \
            tx = d[tn].link[1] = C##_##X##_skew_(d, d[tn].link[1]); \
                 d[tx].link[1] = C##_##X##_skew_(d, d[tx].link[1]); \
                            tn = C##_##X##_split_(d, tn); \
                 d[tn].link[1] = C##_##X##_split_(d, d[tn].link[1]); \
        } \
        return tn; \
    } \
\
    STC_DEF int \
    C##_##X##_erase(C##_##X* self, RawKey rkey) { \
        int erased = 0; \
        C##_##X##_size_t root = C##_##X##_erase_r_(self->data, (C##_##X##_size_t) _smap_root(self), &rkey, &erased); \
        if (erased) {_smap_root(self) = root; --_smap_size(self);} \
        return erased; \
    } \
\
    static C##_##X##_size_t \
    C##_##X##_clone_r_(C##_##X* self, const C##_##X##_node_t* src, C##_##X##_size_t tn) { \
        if (tn == 0) return 0; \
        C##_##X##_size_t cn = C##_##X##_node_new_(self); \
        C##_##X##_node_t* d = self->data; \
        d[cn].link[0] = C##_##X##_clone_r_(self, src, src[tn].link[0]); \
        d[cn].link[1] = C##_##X##_clone_r_(self, src, src[tn].link[1]); \
        d[cn].level = src[tn].level; \
        d[cn].value = C##_##X##_value_clone(src[tn].value); \
        return cn; \
    } \
    STC_DEF C##_##X \
    C##_##X##_clone(C##_##X bst) { \
        C##_##X clone = C##_##X##_with_capacity(_smap_size(&bst)); \
        C##_##X##_size_t root = C##_##X##_clone_r_(&clone, bst.data, (C##_##X##_size_t) _smap_root(&bst)); \
        _smap_root(&clone) = root; \
        return clone; \
    } \
\
    static void \
    C##_##X##_del_r_(C##_##X##_node_t* d, C##_##X##_size_t tn) { \
        if (tn) { \
            C##_##X##_del_r_(d, d[tn].link[0]); \
            C##_##X##_del_r_(d, d[tn].link[1]); \
            C##_##X##_value_del(&d[tn].value); \
        } \
    } \
    STC_DEF void \
    C##_##X##_del(C##_##X* self) { \
        if (_smap_root(self)) { \
            C##_##X##_del_r_(self->data, (C##_##X##_size_t) _smap_root(self)); \
            c_free(_smap_rep(self)); \
        } \
    }

_using_CBST_types(_, csmap, int, int);
static size_t _smap_inits[4] = {0, 0, 0, 0};

#else
#define _implement_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                           keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

enum {_smap_ROOT=0, _smap_DISP=1, _smap_SIZE=2, _smap_CAP=3};
#define _smap_rep(self) (((size_t *)(self)->data) - 4)
#define _smap_root(self) _smap_rep(self)[_smap_ROOT]
#define _smap_disp(self) _smap_rep(self)[_smap_DISP]
#define _smap_size(self) _smap_rep(self)[_smap_SIZE]
#define _smap_cap(self) _smap_rep(self)[_smap_CAP]

#endif
