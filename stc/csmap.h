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
#ifndef CSMAP_H_INCLUDED
#define CSMAP_H_INCLUDED

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
        printf("set %d\n", *i.ref);
    csset_sx_del(&s);
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_csmap(...) c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
    using_csmap_4(X, Key, Mapped, c_default_compare)

#define using_csmap_4(X, Key, Mapped, keyCompare) \
    using_csmap_8(X, Key, Mapped, keyCompare, c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Mapped)

#define using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, mappedClone) \
    using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, c_trivial_toraw, Mapped)

#define using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    using_csmap_12(X, Key, Mapped, keyCompare, \
                      mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                      c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Key)

#define using_csmap_12(X, Key, Mapped, keyCompareRaw, \
                          mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                          keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap_, Key, Mapped, keyCompareRaw, \
                     mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_csmap_keydef(...) c_MACRO_OVERLOAD(using_csmap_keydef, __VA_ARGS__)
#define using_csmap_keydef_6(X, Key, Mapped, keyCompare, keyDel, keyClone) \
    using_csmap_keydef_8(X, Key, Mapped, keyCompare, \
                            keyDel, keyClone, c_trivial_toraw, Key)
#define using_csmap_keydef_8(X, Key, Mapped, keyCompareRaw, \
                                keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap_, Key, Mapped, keyCompareRaw, \
                     c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Mapped, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

/* csset: */
#define using_csset(...) \
    c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
    using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
    using_csset_7(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Key)

#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csset_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csset_, Key, Key, keyCompareRaw, \
                     @@, @@, @@, void, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
    _using_AATREE_strkey(str, csset_, cstr_t, @@, @@, @@, void)
#define using_csmap_str() \
    _using_AATREE(str, csmap_, cstr_t, cstr_t, cstr_compare_raw, \
                       cstr_del, cstr_from, cstr_c_str, const char*, \
                       cstr_del, cstr_from, cstr_c_str, const char*)

#define using_csmap_strkey(...) \
    c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
    _using_AATREE_strkey(X, csmap_, Mapped, c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Mapped)

#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
    _using_AATREE_strkey(X, csmap_, Mapped, mappedDel, mappedClone, c_trivial_toraw, Mapped)

#define using_csmap_strkey_6(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_AATREE_strkey(X, csmap_, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped)

#define _using_AATREE_strkey(X, C, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_AATREE(X, C, cstr_t, Mapped, cstr_compare_raw, \
                     mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                     cstr_del, cstr_from, cstr_c_str, const char*)

#define using_csmap_strval(...) \
    c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
    using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
    using_csmap_strval_7(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Key)

#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csmap_strval_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap_, Key, cstr_t, keyCompareRaw, \
                     cstr_del, cstr_from, cstr_c_str, const char*, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

#define SET_ONLY_csset_(...) __VA_ARGS__
#define SET_ONLY_csmap_(...)
#define MAP_ONLY_csset_(...)
#define MAP_ONLY_csmap_(...) __VA_ARGS__
#define KEY_REF_csset_(vp)   (vp)
#define KEY_REF_csmap_(vp)   (&(vp)->first)
#ifndef CSMAP_SIZE_T
#define CSMAP_SIZE_T uint32_t
#endif

struct csmap_rep { size_t root, disp, head, size, cap; void* nodes[]; };
#define _csmap_rep(self) c_container_of((self)->nodes, struct csmap_rep, nodes)

#define _using_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                         mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                         keyDel, keyFromRaw, keyToRaw, RawKey) \
    typedef Key C##X##_key_t; \
    typedef Mapped C##X##_mapped_t; \
    typedef RawKey C##X##_rawkey_t; \
    typedef RawMapped C##X##_rawmapped_t; \
    typedef CSMAP_SIZE_T C##X##_size_t; \
\
    typedef SET_ONLY_##C( C##X##_key_t ) \
            MAP_ONLY_##C( struct {C##X##_key_t first; \
                                  C##X##_mapped_t second;} ) \
    C##X##_value_t; \
\
    typedef SET_ONLY_##C( C##X##_rawkey_t ) \
            MAP_ONLY_##C( struct {C##X##_rawkey_t first; \
                                  C##X##_rawmapped_t second;} ) \
    C##X##_rawvalue_t; \
\
    typedef struct { \
        C##X##_value_t *ref; \
        bool inserted; \
    } C##X##_result_t; \
\
    typedef struct C##X##_node { \
        C##X##_size_t link[2]; \
        int8_t level; \
        C##X##_value_t value; \
    } C##X##_node_t; \
\
    typedef struct { \
        C##X##_node_t* nodes; \
    } C##X; \
\
    typedef struct { \
        C##X##_value_t *ref; \
        C##X##_node_t *_d; \
        int _top; \
        C##X##_size_t _tn, _st[48]; \
    } C##X##_iter_t; \
\
    STC_API C##X C##X##_init(void); \
    STC_API C##X C##X##_clone(C##X tree); \
\
    STC_API void \
    C##X##_reserve(C##X* self, size_t cap); \
    STC_INLINE C##X \
    C##X##_with_capacity(size_t size) { \
        C##X x = C##X##_init(); \
        C##X##_reserve(&x, size); \
        return x; \
    } \
    STC_INLINE bool \
    C##X##_empty(C##X tree) {return _csmap_rep(&tree)->size == 0;} \
    STC_INLINE size_t \
    C##X##_size(C##X tree) {return _csmap_rep(&tree)->size;} \
    STC_INLINE size_t \
    C##X##_capacity(C##X tree) {return _csmap_rep(&tree)->cap;} \
    STC_API void \
    C##X##_del(C##X* self); \
    STC_INLINE void \
    C##X##_clear(C##X* self) {C##X##_del(self); *self = C##X##_init();} \
    STC_INLINE void \
    C##X##_swap(C##X* a, C##X* b) {c_swap(C##X, *a, *b);} \
\
    STC_INLINE void \
    C##X##_value_del(C##X##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE C##X##_value_t \
    C##X##_value_clone(C##X##_value_t val) { \
        *KEY_REF_##C(&val) = keyFromRaw(keyToRaw(KEY_REF_##C(&val))); \
        MAP_ONLY_##C( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
\
    STC_API C##X##_value_t* \
    C##X##_find_it(const C##X* self, RawKey rkey, C##X##_iter_t* out); \
\
    STC_API C##X##_iter_t \
    C##X##_lower_bound(const C##X* self, RawKey rkey); \
\
    STC_INLINE C##X##_iter_t \
    C##X##_find(const C##X* self, RawKey rkey) { \
        C##X##_iter_t it; \
        C##X##_find_it(self, rkey, &it); \
        return it; \
    } \
\
    STC_INLINE bool \
    C##X##_contains(const C##X* self, RawKey rkey) { \
        C##X##_iter_t it; \
        return C##X##_find_it(self, rkey, &it) != NULL; \
    } \
\
    STC_API C##X##_result_t \
    C##X##_insert_entry_(C##X* self, RawKey rkey); \
\
    STC_INLINE C##X##_result_t \
    C##X##_emplace(C##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        C##X##_result_t res = C##X##_insert_entry_(self, rkey); \
        if (res.inserted) { \
            *KEY_REF_##C(res.ref) = keyFromRaw(rkey); \
            MAP_ONLY_##C(res.ref->second = mappedFromRaw(rmapped);) \
        } \
        return res; \
    } \
    STC_INLINE void \
    C##X##_emplace_n(C##X* self, const C##X##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) SET_ONLY_##C( C##X##_emplace(self, arr[i]); ) \
                                   MAP_ONLY_##C( C##X##_emplace(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE C##X##_result_t \
    C##X##_insert(C##X* self, Key key MAP_ONLY_##C(, Mapped mapped)) { \
        C##X##_result_t res = C##X##_insert_entry_(self, keyToRaw(&key)); \
        if (res.inserted) {*KEY_REF_##C(res.ref) = key; MAP_ONLY_##C( res.ref->second = mapped; )} \
        else              {keyDel(&key); MAP_ONLY_##C( mappedDel(&mapped); )} \
        return res; \
    } \
\
    MAP_ONLY_##C( \
        STC_INLINE C##X##_result_t \
        C##X##_insert_or_assign(C##X* self, Key key, Mapped mapped) { \
            C##X##_result_t res = C##X##_insert_entry_(self, keyToRaw(&key)); \
            if (res.inserted) res.ref->first = key; \
            else {keyDel(&key); mappedDel(&res.ref->second);} \
            res.ref->second = mapped; return res; \
        } \
        STC_INLINE C##X##_result_t \
        C##X##_put(C##X* self, Key k, Mapped m) { \
            return C##X##_insert_or_assign(self, k, m); \
        } \
        STC_INLINE C##X##_result_t \
        C##X##_emplace_or_assign(C##X* self, RawKey rkey, RawMapped rmapped) { \
            C##X##_result_t res = C##X##_insert_entry_(self, rkey); \
            if (res.inserted) res.ref->first = keyFromRaw(rkey); \
            else mappedDel(&res.ref->second); \
            res.ref->second = mappedFromRaw(rmapped); return res; \
        } \
        STC_INLINE C##X##_mapped_t* \
        C##X##_at(const C##X* self, RawKey rkey) { \
            C##X##_iter_t it; \
            return &C##X##_find_it(self, rkey, &it)->second; \
        }) \
\
    STC_API C##X##_value_t* C##X##_front(const C##X* self); \
    STC_API C##X##_value_t* C##X##_back(const C##X* self); \
    STC_API void C##X##_next(C##X##_iter_t* it); \
\
    STC_INLINE C##X##_iter_t \
    C##X##_begin(const C##X* self) { \
        C##X##_iter_t it = {NULL, self->nodes, 0, (C##X##_size_t) _csmap_rep(self)->root}; \
        if (it._tn) C##X##_next(&it); \
        return it; \
    } \
    STC_INLINE C##X##_iter_t \
    C##X##_end(const C##X* self) {\
        C##X##_iter_t it = {NULL}; return it; \
    } \
    STC_INLINE C##X##_mapped_t* \
    C##X##_itval(C##X##_iter_t it) {return SET_ONLY_##C( it.ref ) \
                                                 MAP_ONLY_##C( &it.ref->second );} \
    STC_API int \
    C##X##_erase(C##X* self, RawKey rkey); \
\
    STC_INLINE int \
    C##X##_erase_at(C##X* self, C##X##_iter_t pos) { \
        return C##X##_erase(self, keyToRaw(KEY_REF_##C(pos.ref))); \
    } \
\
    _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                         mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                         keyDel, keyFromRaw, keyToRaw, RawKey) \
    typedef C##X C##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct csmap_rep _smap_inits = {0, 0, 0, 0};

#define _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                             mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                             keyDel, keyFromRaw, keyToRaw, RawKey) \
    STC_DEF C##X \
    C##X##_init(void) { \
        C##X tree = {(C##X##_node_t *) _smap_inits.nodes}; \
        return tree; \
    } \
\
    STC_DEF C##X##_value_t* \
    C##X##_front(const C##X* self) { \
        C##X##_node_t *d = self->nodes; \
        C##X##_size_t tn = (C##X##_size_t) _csmap_rep(self)->root; \
        while (d[tn].link[0]) tn = d[tn].link[0]; \
        return &d[tn].value; \
    } \
    STC_DEF C##X##_value_t* \
    C##X##_back(const C##X* self) { \
        C##X##_node_t *d = self->nodes; \
        C##X##_size_t tn = (C##X##_size_t) _csmap_rep(self)->root; \
        while (d[tn].link[1]) tn = d[tn].link[1]; \
        return &d[tn].value; \
    } \
\
    STC_DEF void \
    C##X##_reserve(C##X* self, size_t cap) { \
        struct csmap_rep* rep = _csmap_rep(self); \
        C##X##_size_t oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct csmap_rep*) c_realloc(oldcap ? rep : NULL, \
                                                sizeof(struct csmap_rep) + (cap + 1)*sizeof(C##X##_node_t)); \
            if (oldcap == 0) \
                memset(rep, 0, sizeof(struct csmap_rep) + sizeof(C##X##_node_t)); \
            rep->cap = cap; \
            self->nodes = (C##X##_node_t *) rep->nodes; \
        } \
    } \
\
    STC_DEF C##X##_size_t \
    C##X##_node_new_(C##X* self, int level) { \
        size_t tn; struct csmap_rep *rep = _csmap_rep(self); \
        if (rep->disp) { \
            tn = rep->disp; \
            rep->disp = self->nodes[tn].link[1]; \
        } else { \
            if ((tn = rep->head + 1) > rep->cap) C##X##_reserve(self, 4 + tn*3/2); \
            ++_csmap_rep(self)->head; /* do after reserve */ \
        } \
        C##X##_node_t* dn = &self->nodes[tn]; \
        dn->link[0] = dn->link[1] = 0; dn->level = level; \
        return (C##X##_size_t) tn; \
    } \
\
    STC_DEF C##X##_value_t* \
    C##X##_find_it(const C##X* self, C##X##_rawkey_t rkey, C##X##_iter_t* out) { \
        C##X##_size_t tn = _csmap_rep(self)->root; \
        C##X##_node_t *d = out->_d = self->nodes; \
        out->_top = 0; \
        while (tn) { \
            int c; C##X##_rawkey_t rx = keyToRaw(KEY_REF_##C(&d[tn].value)); \
            if ((c = keyCompareRaw(&rx, &rkey)) < 0) \
                tn = d[tn].link[1]; \
            else if (c > 0) \
                { out->_st[out->_top++] = tn; tn = d[tn].link[0]; } \
            else \
                { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF C##X##_iter_t \
    C##X##_lower_bound(const C##X* self, RawKey rkey) { \
        C##X##_iter_t it; \
        C##X##_find_it(self, rkey, &it); \
        if (!it.ref && it._top) { \
            C##X##_size_t tn = it._st[--it._top]; \
            it._tn = it._d[tn].link[1]; \
            it.ref = &it._d[tn].value; \
        } \
        return it; \
    } \
\
    STC_DEF void \
    C##X##_next(C##X##_iter_t *it) { \
        C##X##_size_t tn = it->_tn; \
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
    static C##X##_size_t \
    C##X##_skew_(C##X##_node_t *d, C##X##_size_t tn) { \
        if (tn && d[d[tn].link[0]].level == d[tn].level) { \
            C##X##_size_t tmp = d[tn].link[0]; \
            d[tn].link[0] = d[tmp].link[1]; \
            d[tmp].link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
    static C##X##_size_t \
    C##X##_split_(C##X##_node_t *d, C##X##_size_t tn) { \
        if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) { \
            C##X##_size_t tmp = d[tn].link[1]; \
            d[tn].link[1] = d[tmp].link[0]; \
            d[tmp].link[0] = tn; \
            tn = tmp; \
            ++d[tn].level; \
        } \
        return tn; \
    } \
\
    static inline C##X##_size_t \
    C##X##_insert_entry_i_(C##X* self, C##X##_size_t tn, const C##X##_rawkey_t* rkey, C##X##_result_t* res) { \
        C##X##_size_t up[64], it = tn; \
        C##X##_node_t* d = self->nodes; \
        int c, top = 0, dir = 0; \
        while (it) { \
            up[top++] = it; \
            C##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[it].value)); \
            if ((c = keyCompareRaw(&raw, rkey)) == 0) {res->ref = &d[it].value; return tn;} \
            dir = (c == -1); \
            it = d[it].link[dir]; \
        } \
        it = C##X##_node_new_(self, 1); d = self->nodes; \
        res->ref = &d[it].value, res->inserted = true; \
        if (top == 0) return it; \
        d[up[top - 1]].link[dir] = it; \
        while (top--) { \
            if (top) dir = (d[up[top - 1]].link[1] == up[top]); \
            up[top] = C##X##_skew_(d, up[top]); \
            up[top] = C##X##_split_(d, up[top]); \
            if (top) d[up[top - 1]].link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF C##X##_result_t \
    C##X##_insert_entry_(C##X* self, RawKey rkey) { \
        C##X##_result_t res = {NULL, false}; \
        C##X##_size_t tn = C##X##_insert_entry_i_(self, (C##X##_size_t) _csmap_rep(self)->root, &rkey, &res); \
        _csmap_rep(self)->root = tn; \
        _csmap_rep(self)->size += res.inserted; \
        return res; \
    } \
\
    static C##X##_size_t \
    C##X##_erase_r_(C##X##_node_t *d, C##X##_size_t tn, const C##X##_rawkey_t* rkey, int *erased) { \
        if (tn == 0) return 0; \
        C##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
        C##X##_size_t tx; int c = keyCompareRaw(&raw, rkey); \
        if (c != 0) \
            d[tn].link[c == -1] = C##X##_erase_r_(d, d[tn].link[c == -1], rkey, erased); \
        else { \
            if (!*erased) {C##X##_value_del(&d[tn].value); *erased = 1;} \
            if (d[tn].link[0] && d[tn].link[1]) { \
                tx = d[tn].link[0]; \
                while (d[tx].link[1]) \
                    tx = d[tx].link[1]; \
                d[tn].value = d[tx].value; /* move */ \
                raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
                d[tn].link[0] = C##X##_erase_r_(d, d[tn].link[0], &raw, erased); \
            } else { /* unlink node */ \
                tx = tn; \
                tn = d[tn].link[ d[tn].link[0] == 0 ]; \
                /* move it to disposed nodes list */ \
                struct csmap_rep *rep = c_container_of(d, struct csmap_rep, nodes); \
                d[tx].link[1] = (C##X##_size_t) rep->disp; \
                rep->disp = tx; \
            } \
        } \
        tx = d[tn].link[1]; \
        if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) { \
            if (d[tx].level > --d[tn].level) \
                d[tx].level = d[tn].level; \
                            tn = C##X##_skew_(d, tn); \
            tx = d[tn].link[1] = C##X##_skew_(d, d[tn].link[1]); \
                 d[tx].link[1] = C##X##_skew_(d, d[tx].link[1]); \
                            tn = C##X##_split_(d, tn); \
                 d[tn].link[1] = C##X##_split_(d, d[tn].link[1]); \
        } \
        return tn; \
    } \
\
    STC_DEF int \
    C##X##_erase(C##X* self, RawKey rkey) { \
        int erased = 0; \
        C##X##_size_t root = C##X##_erase_r_(self->nodes, (C##X##_size_t) _csmap_rep(self)->root, &rkey, &erased); \
        if (erased) {_csmap_rep(self)->root = root; --_csmap_rep(self)->size;} \
        return erased; \
    } \
\
    static C##X##_size_t \
    C##X##_clone_r_(C##X* self, const C##X##_node_t* src, C##X##_size_t sn) { \
        if (sn == 0) return 0; \
        C##X##_size_t tx, tn = C##X##_node_new_(self, src[sn].level); \
        self->nodes[tn].value = C##X##_value_clone(src[sn].value); \
        tx = C##X##_clone_r_(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx; \
        tx = C##X##_clone_r_(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx; \
        return tn; \
    } \
    STC_DEF C##X \
    C##X##_clone(C##X tree) { \
        C##X clone = C##X##_with_capacity(_csmap_rep(&tree)->size); \
        C##X##_size_t root = C##X##_clone_r_(&clone, tree.nodes, (C##X##_size_t) _csmap_rep(&tree)->root); \
        _csmap_rep(&clone)->root = root; \
        _csmap_rep(&clone)->size = _csmap_rep(&tree)->size; \
        return clone; \
    } \
\
    static void \
    C##X##_del_r_(C##X##_node_t* d, C##X##_size_t tn) { \
        if (tn) { \
            C##X##_del_r_(d, d[tn].link[0]); \
            C##X##_del_r_(d, d[tn].link[1]); \
            C##X##_value_del(&d[tn].value); \
        } \
    } \
    STC_DEF void \
    C##X##_del(C##X* self) { \
        if (_csmap_rep(self)->root) { \
            C##X##_del_r_(self->nodes, (C##X##_size_t) _csmap_rep(self)->root); \
            c_free(_csmap_rep(self)); \
        } \
    }

#else
#define _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                             mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                             keyDel, keyFromRaw, keyToRaw, RawKey)
#endif

#endif
