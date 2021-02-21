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

#define using_csmap(...) c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
    using_csmap_4(X, Key, Mapped, c_default_compare)

#define using_csmap_4(X, Key, Mapped, keyCompare) \
    using_csmap_8(X, Key, Mapped, keyCompare, c_default_del, c_default_fromraw, c_default_toraw, Mapped)

#define using_csmap_5(X, Key, Mapped, keyCompare, mappedDel) \
    using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, c_no_fromraw, c_default_toraw, Mapped)

#define using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    using_csmap_12(X, Key, Mapped, keyCompare, \
                      mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                      c_default_del, c_default_fromraw, c_default_toraw, Key)

#define using_csmap_12(X, Key, Mapped, keyCompareRaw, \
                          mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                          keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap, Key, Mapped, keyCompareRaw, \
                     mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_csmap_keyarg(...) c_MACRO_OVERLOAD(using_csmap_keyarg, __VA_ARGS__)
#define using_csmap_keyarg_5(X, Key, Mapped, keyCompare, keyDel) \
    using_csmap_keyarg_8(X, Key, Mapped, keyCompare, \
                            keyDel, c_no_fromraw, c_default_toraw, Key)
#define using_csmap_keyarg_8(X, Key, Mapped, keyCompareRaw, \
                                keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap, Key, Mapped, keyCompareRaw, \
                     c_default_del, c_default_fromraw, c_default_toraw, Mapped, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

/* csset: */
#define using_csset(...) \
    c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
    using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
    using_csset_7(X, Key, keyCompare, c_default_del, c_default_fromraw, c_default_toraw, Key)

#define using_csset_4(X, Key, keyCompare, keyDel) \
    using_csset_7(X, Key, keyCompare, keyDel, c_no_fromraw, c_default_toraw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csset, Key, Key, keyCompareRaw, \
                     _UNUSED_, _UNUSED_, _UNUSED_, void, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
    _using_AATREE_strkey(str, csset, cstr_t, _UNUSED_, _UNUSED_, _UNUSED_, void)
#define using_csmap_str() \
    _using_AATREE(str, csmap, cstr_t, cstr_t, cstr_compare_raw, \
                       cstr_del, cstr_from, cstr_c_str, const char*, \
                       cstr_del, cstr_from, cstr_c_str, const char*)

#define using_csmap_strkey(...) \
    c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
    _using_AATREE_strkey(X, csmap, Mapped, c_default_del, c_default_fromraw, c_default_toraw, Mapped)

#define using_csmap_strkey_3(X, Mapped, mappedDel) \
    _using_AATREE_strkey(X, csmap, Mapped, mappedDel, c_no_fromraw, c_default_toraw, Mapped)

#define using_csmap_strkey_6(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_AATREE_strkey(X, csmap, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped)

#define _using_AATREE_strkey(X, C, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_AATREE(X, C, cstr_t, Mapped, cstr_compare_raw, \
                     mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                     cstr_del, cstr_from, cstr_c_str, const char*)

#define using_csmap_strval(...) \
    c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
    using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
    using_csmap_strval_7(X, Key, keyCompare, c_default_del, c_default_fromraw, c_default_toraw, Key)

#define using_csmap_strval_4(X, Key, keyCompare, keyDel) \
    using_csmap_strval_7(X, Key, keyCompare, keyDel, c_no_fromraw, c_default_toraw, Key)

#define using_csmap_strval_7(X, Key, keyCompare, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_AATREE(X, csmap, Key, cstr_t, keyCompare, \
                     cstr_del, cstr_from, cstr_c_str, const char*, \
                     keyDel, keyFromRaw, keyToRaw, RawKey)

#define SET_ONLY_csset(...) __VA_ARGS__
#define SET_ONLY_csmap(...)
#define MAP_ONLY_csset(...)
#define MAP_ONLY_csmap(...) __VA_ARGS__
#define KEY_REF_csset(vp)   (vp)
#define KEY_REF_csmap(vp)   (&(vp)->first)
#ifndef CSMAP_SIZE_T
#define CSMAP_SIZE_T uint32_t
#endif

struct csmap_rep { size_t root, disp, head, size, cap; void* nodes[]; };
#define csmap_rep_(self) c_container_of((self)->nodes, struct csmap_rep, nodes)

#define _using_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                         mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                         keyDel, keyFromRaw, keyToRaw, RawKey) \
    typedef Key C##_##X##_key_t; \
    typedef Mapped C##_##X##_mapped_t; \
    typedef RawKey C##_##X##_rawkey_t; \
    typedef RawMapped C##_##X##_rawmapped_t; \
    typedef CSMAP_SIZE_T C##_##X##_size_t; \
\
    typedef SET_ONLY_##C( C##_##X##_key_t ) \
            MAP_ONLY_##C( struct {C##_##X##_key_t first; \
                                  C##_##X##_mapped_t second;} ) \
    C##_##X##_value_t; \
\
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
    typedef struct C##_##X##_node { \
        C##_##X##_size_t link[2]; \
        int8_t level; \
        C##_##X##_value_t value; \
    } C##_##X##_node_t; \
\
    typedef struct { \
        C##_##X##_node_t* nodes; \
    } C##_##X; \
\
    typedef struct { \
        C##_##X##_value_t *ref; \
        C##_##X##_node_t *_d; \
        int _top; \
        C##_##X##_size_t _tn, _st[48]; \
    } C##_##X##_iter_t; \
\
    STC_API C##_##X C##_##X##_init(void); \
    STC_API C##_##X C##_##X##_clone(C##_##X tree); \
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
    C##_##X##_empty(C##_##X tree) {return csmap_rep_(&tree)->size == 0;} \
    STC_INLINE size_t \
    C##_##X##_size(C##_##X tree) {return csmap_rep_(&tree)->size;} \
    STC_INLINE size_t \
    C##_##X##_capacity(C##_##X tree) {return csmap_rep_(&tree)->cap;} \
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
    C##_##X##_insert_entry_(C##_##X* self, RawKey rkey); \
\
    STC_INLINE C##_##X##_result_t \
    C##_##X##_emplace(C##_##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        C##_##X##_result_t res = C##_##X##_insert_entry_(self, rkey); \
        if (res.second) { \
            *KEY_REF_##C(res.first) = keyFromRaw(rkey); \
            MAP_ONLY_##C(res.first->second = mappedFromRaw(rmapped);) \
        } \
        return res; \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_emplace_or_assign(C##_##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        C##_##X##_result_t res = C##_##X##_insert_entry_(self, rkey); \
                      if (res.second) *KEY_REF_##C(res.first) = keyFromRaw(rkey); \
        MAP_ONLY_##C( else mappedDel(&res.first->second); \
                      res.first->second = mappedFromRaw(rmapped); ) \
        return res; \
    } \
    STC_INLINE void \
    C##_##X##_emplace_n(C##_##X* self, const C##_##X##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) SET_ONLY_##C( C##_##X##_emplace(self, arr[i]); ) \
                                   MAP_ONLY_##C( C##_##X##_emplace(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE C##_##X##_result_t \
    C##_##X##_insert(C##_##X* self, Key key MAP_ONLY_##C(, Mapped mapped)) { \
        C##_##X##_result_t res = C##_##X##_insert_entry_(self, keyToRaw(&key)); \
        if (res.second) {*KEY_REF_##C(res.first) = key; MAP_ONLY_##C( res.first->second = mapped; )} \
        else            {keyDel(&key); MAP_ONLY_##C( mappedDel(&mapped); )} \
        return res; \
    } \
\
    MAP_ONLY_##C( \
        STC_INLINE C##_##X##_result_t \
        C##_##X##_insert_or_assign(C##_##X* self, Key key, Mapped mapped) { \
            C##_##X##_result_t res = C##_##X##_insert_entry_(self, keyToRaw(&key)); \
            if (res.second) res.first->first = key; else {keyDel(&key); mappedDel(&res.first->second);} \
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
        C##_##X##_iter_t it = {NULL, self->nodes, 0, (C##_##X##_size_t) csmap_rep_(self)->root}; \
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
    _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                         mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                         keyDel, keyFromRaw, keyToRaw, RawKey) \
    typedef C##_##X C##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct csmap_rep _smap_inits = {0, 0, 0, 0};

#define _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                             mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                             keyDel, keyFromRaw, keyToRaw, RawKey) \
    STC_DEF C##_##X \
    C##_##X##_init(void) { \
        C##_##X tree = {(C##_##X##_node_t *) _smap_inits.nodes}; \
        return tree; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_front(C##_##X* self) { \
        C##_##X##_node_t *d = self->nodes; \
        C##_##X##_size_t tn = (C##_##X##_size_t) csmap_rep_(self)->root; \
        while (d[tn].link[0]) tn = d[tn].link[0]; \
        return &d[tn].value; \
    } \
    STC_DEF C##_##X##_value_t* \
    C##_##X##_back(C##_##X* self) { \
        C##_##X##_node_t *d = self->nodes; \
        C##_##X##_size_t tn = (C##_##X##_size_t) csmap_rep_(self)->root; \
        while (d[tn].link[1]) tn = d[tn].link[1]; \
        return &d[tn].value; \
    } \
\
    STC_DEF void \
    C##_##X##_reserve(C##_##X* self, size_t cap) { \
        struct csmap_rep* rep = csmap_rep_(self); \
        C##_##X##_size_t oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct csmap_rep*) c_realloc(oldcap ? rep : NULL, \
                                                sizeof(struct csmap_rep) + (cap + 1)*sizeof(C##_##X##_node_t)); \
            if (oldcap == 0) \
                memset(rep, 0, sizeof(struct csmap_rep) + sizeof(C##_##X##_node_t)); \
            rep->cap = cap; \
            self->nodes = (C##_##X##_node_t *) rep->nodes; \
        } \
    } \
\
    STC_DEF C##_##X##_size_t \
    C##_##X##_node_new_(C##_##X* self, int level) { \
        size_t tn; struct csmap_rep *rep = csmap_rep_(self); \
        if (rep->disp) { \
            tn = rep->disp; \
            rep->disp = self->nodes[tn].link[1]; \
        } else { \
            if ((tn = rep->head + 1) > rep->cap) C##_##X##_reserve(self, 4 + tn*3/2); \
            ++csmap_rep_(self)->head; /* do after reserve */ \
        } \
        C##_##X##_node_t* dn = &self->nodes[tn]; \
        dn->link[0] = dn->link[1] = 0; dn->level = level; \
        return (C##_##X##_size_t) tn; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_find_it(const C##_##X* self, C##_##X##_rawkey_t rkey, C##_##X##_iter_t* out) { \
        C##_##X##_size_t tn = csmap_rep_(self)->root; \
        C##_##X##_node_t *d = out->_d = self->nodes; \
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
    C##_##X##insert_entry_i_(C##_##X* self, C##_##X##_size_t tn, const C##_##X##_rawkey_t* rkey, C##_##X##_result_t* res) { \
        C##_##X##_size_t up[64], it = tn; \
        C##_##X##_node_t* d = self->nodes; \
        int c, top = 0, dir = 0; \
        while (it) { \
            up[top++] = it; \
            C##_##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[it].value)); \
            if ((c = keyCompareRaw(&raw, rkey)) == 0) {res->first = &d[it].value; return tn;} \
            dir = (c == -1); \
            it = d[it].link[dir]; \
        } \
        it = C##_##X##_node_new_(self, 1); d = self->nodes; \
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
    C##_##X##_insert_entry_(C##_##X* self, RawKey rkey) { \
        C##_##X##_result_t res = {NULL, false}; \
        C##_##X##_size_t tn = C##_##X##insert_entry_i_(self, (C##_##X##_size_t) csmap_rep_(self)->root, &rkey, &res); \
        csmap_rep_(self)->root = tn; \
        csmap_rep_(self)->size += res.second; \
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
                /* move it to disposed nodes list */ \
                struct csmap_rep *rep = c_container_of(d, struct csmap_rep, nodes); \
                d[tx].link[1] = (C##_##X##_size_t) rep->disp; \
                rep->disp = tx; \
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
        C##_##X##_size_t root = C##_##X##_erase_r_(self->nodes, (C##_##X##_size_t) csmap_rep_(self)->root, &rkey, &erased); \
        if (erased) {csmap_rep_(self)->root = root; --csmap_rep_(self)->size;} \
        return erased; \
    } \
\
    static C##_##X##_size_t \
    C##_##X##_clone_r_(C##_##X* self, const C##_##X##_node_t* src, C##_##X##_size_t sn) { \
        if (sn == 0) return 0; \
        C##_##X##_size_t tx, tn = C##_##X##_node_new_(self, src[sn].level); \
        self->nodes[tn].value = C##_##X##_value_clone(src[sn].value); \
        tx = C##_##X##_clone_r_(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx; \
        tx = C##_##X##_clone_r_(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx; \
        return tn; \
    } \
    STC_DEF C##_##X \
    C##_##X##_clone(C##_##X tree) { \
        C##_##X clone = C##_##X##_with_capacity(csmap_rep_(&tree)->size); \
        C##_##X##_size_t root = C##_##X##_clone_r_(&clone, tree.nodes, (C##_##X##_size_t) csmap_rep_(&tree)->root); \
        csmap_rep_(&clone)->root = root; \
        csmap_rep_(&clone)->size = csmap_rep_(&tree)->size; \
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
        if (csmap_rep_(self)->root) { \
            C##_##X##_del_r_(self->nodes, (C##_##X##_size_t) csmap_rep_(self)->root); \
            c_free(csmap_rep_(self)); \
        } \
    }

#else
#define _implement_AATREE(X, C, Key, Mapped, keyCompareRaw, \
                             mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                             keyDel, keyFromRaw, keyToRaw, RawKey)
#endif

#endif
