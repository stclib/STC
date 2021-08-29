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
using_csmap(mx, int, char); // Sorted map<int, char>

int main(void) {
    c_forvar (csmap_mx m = csmap_mx_init(), csmap_mx_del(&m))
    {
        csmap_mx_insert(&m, 5, 'a');
        csmap_mx_insert(&m, 8, 'b');
        csmap_mx_insert(&m, 12, 'c');

        csmap_mx_iter_t it = csmap_mx_find(&m, 10); // none
        char val = csmap_mx_find(&m, 5).ref->second;
        csmap_mx_put(&m, 5, 'd'); // update
        csmap_mx_erase(&m, 8);

        c_foreach (i, csmap_mx, m)
            printf("map %d: %c\n", i.ref->first, i.ref->second);
    }
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define forward_csmap(X, Key, Mapped) _c_aatree_types(csmap_##X, csmap_, Key, Mapped)

#define using_csmap(...) c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
            using_csmap_4(X, Key, Mapped, c_default_compare)
#define using_csmap_4(X, Key, Mapped, keyCompare) \
            using_csmap_6(X, Key, Mapped, keyCompare, c_default_del, c_default_fromraw)
#define using_csmap_5(X, Key, Mapped, keyCompare, mappedDel) \
            using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, c_no_clone)
#define using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, mappedClone) \
            using_csmap_9(X, Key, Mapped, keyCompare, mappedDel, mappedClone, c_default_toraw, Mapped, c_true)
#define using_csmap_9(X, Key, Mapped, keyCompare, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            using_csmap_13(X, Key, Mapped, keyCompare, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                           c_default_del, c_default_fromraw, c_default_toraw, Key)
#define using_csmap_13(X, Key, Mapped, keyCompareRaw, \
                       mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)


#define using_csmap_keydef(...) c_MACRO_OVERLOAD(using_csmap_keydef, __VA_ARGS__)

#define using_csmap_keydef_6(X, Key, Mapped, keyCompare, keyDel, keyClone) \
            using_csmap_keydef_9(X, Key, Mapped, keyCompare, \
                                 keyDel, keyClone, c_default_toraw, Key, c_true)
#define using_csmap_keydef_9(X, Key, Mapped, keyCompareRaw, \
                             keyDel, keyFromRaw, keyToRaw, RawKey, defTypes) \
            _c_using_aatree(csmap_##X, csmap_, Key, Mapped, keyCompareRaw, \
                            c_default_del, c_default_fromraw, c_default_toraw, Mapped, defTypes, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_csmap_str() \
            _c_using_aatree(csmap_str, csmap_, cstr, cstr, c_rawstr_compare, \
                            cstr_del, cstr_from, cstr_str, const char*, c_true, \
                            cstr_del, cstr_from, cstr_str, const char*)


#define using_csmap_strkey(...) c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
            using_csmap_strkey_4(X, Mapped, c_default_del, c_default_fromraw)
#define using_csmap_strkey_3(X, Mapped, mappedDel) \
            using_csmap_strkey_4(X, Mapped, mappedDel, c_no_clone)
#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
            using_csmap_strkey_7(X, Mapped, mappedDel, mappedClone, c_default_toraw, Mapped, c_true)
#define using_csmap_strkey_7(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            _c_using_aatree_strkey(X, csmap_, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes)
#define _c_using_aatree_strkey(X, C, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes) \
            _c_using_aatree(C##X, C, cstr, Mapped, c_rawstr_compare, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                            cstr_del, cstr_from, cstr_str, const char*)


#define using_csmap_strval(...) c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
            using_csmap_strval_3(X, Key, c_default_compare)
#define using_csmap_strval_3(X, Key, keyCompare) \
            using_csmap_strval_5(X, Key, keyCompare, c_default_del, c_default_fromraw)
#define using_csmap_strval_4(X, Key, keyCompare, keyDel) \
            using_csmap_strval_5(X, Key, keyCompare, keyDel, c_no_clone)
#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
            using_csmap_strval_8(X, Key, keyCompare, keyDel, keyClone, c_default_toraw, Key, c_true)
#define using_csmap_strval_8(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey, defTypes) \
            _c_using_aatree(csmap_##X, csmap_, Key, cstr, keyCompareRaw, \
                            cstr_del, cstr_from, cstr_str, const char*, defTypes, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)

#define SET_ONLY_csmap_(...)
#define MAP_ONLY_csmap_(...) __VA_ARGS__
#define KEY_REF_csmap_(vp)   (&(vp)->first)
#ifndef CSMAP_SIZE_T
#define CSMAP_SIZE_T uint32_t
#endif
struct csmap_rep { size_t root, disp, head, size, cap; void* nodes[]; };
#define _csmap_rep(self) c_container_of((self)->nodes, struct csmap_rep, nodes)

#define _c_aatree_types(CX, C, Key, Mapped) \
    typedef Key CX##_key_t; \
    typedef Mapped CX##_mapped_t; \
    typedef CSMAP_SIZE_T CX##_size_t; \
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
    typedef struct CX##_node_t CX##_node_t; \
\
    typedef struct { \
        CX##_value_t *ref; \
        CX##_node_t *_d; \
        int _top; \
        CX##_size_t _tn, _st[36]; \
    } CX##_iter_t; \
\
    typedef struct { \
        CX##_node_t *nodes; \
    } CX

#define _c_using_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                        mappedDel, mappedFromRaw, mappedToRaw, RawMapped, defTypes, \
                        keyDel, keyFromRaw, keyToRaw, RawKey) \
    defTypes( _c_aatree_types(CX, C, Key, Mapped); ) \
\
    MAP_ONLY_##C( struct CX##_value_t { \
        CX##_key_t first; \
        CX##_mapped_t second; \
    }; ) \
\
    struct CX##_node_t { \
        CX##_size_t link[2]; \
        int8_t level; \
        CX##_value_t value; \
    }; \
\
    typedef RawKey CX##_rawkey_t; \
    typedef RawMapped CX##_rawmapped_t; \
    typedef SET_ONLY_##C( RawKey ) \
            MAP_ONLY_##C( struct { RawKey first; \
                                   RawMapped second; } ) \
    CX##_rawvalue_t; \
\
    STC_API CX               CX##_init(void); \
    STC_API CX               CX##_clone(CX tree); \
    STC_API void             CX##_del(CX* self); \
    STC_API void             CX##_reserve(CX* self, size_t cap); \
    STC_API CX##_value_t*    CX##_find_it(const CX* self, RawKey rkey, CX##_iter_t* out); \
    STC_API CX##_iter_t      CX##_lower_bound(const CX* self, RawKey rkey); \
    STC_API CX##_value_t*    CX##_front(const CX* self); \
    STC_API CX##_value_t*    CX##_back(const CX* self); \
    STC_API int              CX##_erase(CX* self, RawKey rkey); \
    STC_API CX##_iter_t      CX##_erase_at(CX* self, CX##_iter_t it); \
    STC_API CX##_iter_t      CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2); \
    STC_API CX##_result_t    CX##_insert_entry_(CX* self, RawKey rkey); \
    STC_API void             CX##_next(CX##_iter_t* it); \
\
    STC_INLINE bool          CX##_empty(CX tree) {return _csmap_rep(&tree)->size == 0;} \
    STC_INLINE size_t        CX##_size(CX tree) {return _csmap_rep(&tree)->size;} \
    STC_INLINE size_t        CX##_capacity(CX tree) {return _csmap_rep(&tree)->cap;} \
    STC_INLINE void          CX##_clear(CX* self) {CX##_del(self); *self = CX##_init();} \
    STC_INLINE void          CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
    STC_INLINE bool          CX##_contains(const CX* self, RawKey rkey) \
                                {CX##_iter_t it; return CX##_find_it(self, rkey, &it) != NULL;} \
    STC_INLINE CX##_value_t* CX##_get(const CX* self, RawKey rkey) \
                                {CX##_iter_t it; return CX##_find_it(self, rkey, &it);} \
\
    STC_INLINE CX \
    CX##_with_capacity(size_t size) { \
        CX tree = CX##_init(); \
        CX##_reserve(&tree, size); \
        return tree; \
    } \
\
    STC_INLINE CX##_rawvalue_t \
    CX##_value_toraw(CX##_value_t* val) { \
        return SET_ONLY_##C( keyToRaw(val) ) \
               MAP_ONLY_##C( c_make(CX##_rawvalue_t){keyToRaw(&val->first), mappedToRaw(&val->second)} ); \
    } \
    STC_INLINE void \
    CX##_value_del(CX##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE void \
    CX##_value_clone(CX##_value_t* dst, CX##_value_t* val) { \
        *KEY_REF_##C(dst) = keyFromRaw(keyToRaw(KEY_REF_##C(val))); \
        MAP_ONLY_##C( dst->second = mappedFromRaw(mappedToRaw(&val->second)); ) \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawKey rkey) { \
        CX##_iter_t it; \
        CX##_find_it(self, rkey, &it); \
        return it; \
    } \
\
    STC_INLINE CX##_result_t \
    CX##_emplace(CX* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped)) { \
        CX##_result_t res = CX##_insert_entry_(self, rkey); \
        if (res.inserted) { \
            *KEY_REF_##C(res.ref) = keyFromRaw(rkey); \
            MAP_ONLY_##C(res.ref->second = mappedFromRaw(rmapped);) \
        } \
        return res; \
    } \
\
    STC_INLINE void \
    CX##_emplace_items(CX* self, const CX##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) SET_ONLY_##C( CX##_emplace(self, arr[i]); ) \
                                   MAP_ONLY_##C( CX##_emplace(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE CX##_result_t \
    CX##_insert(CX* self, Key key MAP_ONLY_##C(, Mapped mapped)) { \
        CX##_result_t res = CX##_insert_entry_(self, keyToRaw(&key)); \
        if (res.inserted) {*KEY_REF_##C(res.ref) = key; MAP_ONLY_##C( res.ref->second = mapped; )} \
        else              {keyDel(&key); MAP_ONLY_##C( mappedDel(&mapped); )} \
        return res; \
    } \
\
    MAP_ONLY_##C( \
        STC_INLINE CX##_result_t \
        CX##_insert_or_assign(CX* self, Key key, Mapped mapped) { \
            CX##_result_t res = CX##_insert_entry_(self, keyToRaw(&key)); \
            if (res.inserted) res.ref->first = key; \
            else {keyDel(&key); mappedDel(&res.ref->second);} \
            res.ref->second = mapped; return res; \
        } \
    \
        STC_INLINE CX##_result_t \
        CX##_put(CX* self, Key key, Mapped mapped) { \
            return CX##_insert_or_assign(self, key, mapped); \
        } \
    \
        STC_INLINE CX##_result_t \
        CX##_emplace_or_assign(CX* self, RawKey rkey, RawMapped rmapped) { \
            CX##_result_t res = CX##_insert_entry_(self, rkey); \
            if (res.inserted) res.ref->first = keyFromRaw(rkey); \
            else mappedDel(&res.ref->second); \
            res.ref->second = mappedFromRaw(rmapped); return res; \
        } \
    \
        STC_INLINE CX##_mapped_t* \
        CX##_at(const CX* self, RawKey rkey) { \
            CX##_iter_t it; \
            return &CX##_find_it(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_iter_t it; it._d = self->nodes, it._top = 0; \
        it._tn = (CX##_size_t) _csmap_rep(self)->root; \
        if (it._tn) CX##_next(&it); \
        return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) {\
        return c_make(CX##_iter_t){.ref = NULL}; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_fwd(CX##_iter_t it, size_t n) { \
        while (n-- && it.ref) CX##_next(&it); \
        return it; \
    } \
\
    _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                        mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                        keyDel, keyFromRaw, keyToRaw, RawKey) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct csmap_rep _csmap_sentinel = {0, 0, 0, 0, 0};

#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey) \
    STC_DEF CX \
    CX##_init(void) { \
        CX tree = {(CX##_node_t *) _csmap_sentinel.nodes}; \
        return tree; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_front(const CX* self) { \
        CX##_node_t *d = self->nodes; \
        CX##_size_t tn = (CX##_size_t) _csmap_rep(self)->root; \
        while (d[tn].link[0]) tn = d[tn].link[0]; \
        return &d[tn].value; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_back(const CX* self) { \
        CX##_node_t *d = self->nodes; \
        CX##_size_t tn = (CX##_size_t) _csmap_rep(self)->root; \
        while (d[tn].link[1]) tn = d[tn].link[1]; \
        return &d[tn].value; \
    } \
\
    STC_DEF void \
    CX##_reserve(CX* self, size_t cap) { \
        struct csmap_rep* rep = _csmap_rep(self); \
        CX##_size_t oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct csmap_rep*) c_realloc(oldcap ? rep : NULL, \
                                                sizeof(struct csmap_rep) + (cap + 1)*sizeof(CX##_node_t)); \
            if (oldcap == 0) \
                memset(rep, 0, sizeof(struct csmap_rep) + sizeof(CX##_node_t)); \
            rep->cap = cap; \
            self->nodes = (CX##_node_t *) rep->nodes; \
        } \
    } \
\
    STC_DEF CX##_size_t \
    CX##_node_new_(CX* self, int level) { \
        size_t tn; struct csmap_rep *rep = _csmap_rep(self); \
        if (rep->disp) { \
            tn = rep->disp; \
            rep->disp = self->nodes[tn].link[1]; \
        } else { \
            if ((tn = rep->head + 1) > rep->cap) CX##_reserve(self, 4 + (tn*13 >> 3)); \
            ++_csmap_rep(self)->head; /* do after reserve */ \
        } \
        CX##_node_t* dn = &self->nodes[tn]; \
        dn->link[0] = dn->link[1] = 0; dn->level = level; \
        return (CX##_size_t) tn; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_find_it(const CX* self, RawKey rkey, CX##_iter_t* out) { \
        CX##_size_t tn = _csmap_rep(self)->root; \
        CX##_node_t *d = out->_d = self->nodes; \
        out->_top = 0; \
        while (tn) { \
            int c; CX##_rawkey_t raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
            if ((c = keyCompareRaw(&raw, &rkey)) < 0) \
                tn = d[tn].link[1]; \
            else if (c > 0) \
                { out->_st[out->_top++] = tn; tn = d[tn].link[0]; } \
            else \
                { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_lower_bound(const CX* self, RawKey rkey) { \
        CX##_iter_t it; \
        CX##_find_it(self, rkey, &it); \
        if (!it.ref && it._top) { \
            CX##_size_t tn = it._st[--it._top]; \
            it._tn = it._d[tn].link[1]; \
            it.ref = &it._d[tn].value; \
        } \
        return it; \
    } \
\
    STC_DEF void \
    CX##_next(CX##_iter_t *it) { \
        CX##_size_t tn = it->_tn; \
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
    static CX##_size_t \
    CX##_skew_(CX##_node_t *d, CX##_size_t tn) { \
        if (tn && d[d[tn].link[0]].level == d[tn].level) { \
            CX##_size_t tmp = d[tn].link[0]; \
            d[tn].link[0] = d[tmp].link[1]; \
            d[tmp].link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
    static CX##_size_t \
    CX##_split_(CX##_node_t *d, CX##_size_t tn) { \
        if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) { \
            CX##_size_t tmp = d[tn].link[1]; \
            d[tn].link[1] = d[tmp].link[0]; \
            d[tmp].link[0] = tn; \
            tn = tmp; \
            ++d[tn].level; \
        } \
        return tn; \
    } \
\
    STC_DEF CX##_size_t \
    CX##_insert_entry_i_(CX* self, CX##_size_t tn, const CX##_rawkey_t* rkey, CX##_result_t* res) { \
        CX##_size_t up[64], tx = tn; \
        CX##_node_t* d = self->nodes; \
        int c, top = 0, dir = 0; \
        while (tx) { \
            up[top++] = tx; \
            RawKey raw = keyToRaw(KEY_REF_##C(&d[tx].value)); \
            if ((c = keyCompareRaw(&raw, rkey)) == 0) {res->ref = &d[tx].value; return tn;} \
            dir = (c < 0); \
            tx = d[tx].link[dir]; \
        } \
        tx = CX##_node_new_(self, 1); d = self->nodes; \
        res->ref = &d[tx].value, res->inserted = true; \
        if (top == 0) return tx; \
        d[up[top - 1]].link[dir] = tx; \
        while (top--) { \
            if (top) dir = (d[up[top - 1]].link[1] == up[top]); \
            up[top] = CX##_skew_(d, up[top]); \
            up[top] = CX##_split_(d, up[top]); \
            if (top) d[up[top - 1]].link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF CX##_result_t \
    CX##_insert_entry_(CX* self, RawKey rkey) { \
        CX##_result_t res = {NULL, false}; \
        CX##_size_t tn = CX##_insert_entry_i_(self, (CX##_size_t) _csmap_rep(self)->root, &rkey, &res); \
        _csmap_rep(self)->root = tn; \
        _csmap_rep(self)->size += res.inserted; \
        return res; \
    } \
\
    STC_DEF CX##_size_t \
    CX##_erase_r_(CX##_node_t *d, CX##_size_t tn, const CX##_rawkey_t* rkey, int *erased) { \
        if (tn == 0) return 0; \
        RawKey raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
        CX##_size_t tx; int c = keyCompareRaw(&raw, rkey); \
        if (c != 0) \
            d[tn].link[c < 0] = CX##_erase_r_(d, d[tn].link[c < 0], rkey, erased); \
        else { \
            if (!(*erased)++) CX##_value_del(&d[tn].value); \
            if (d[tn].link[0] && d[tn].link[1]) { \
                tx = d[tn].link[0]; \
                while (d[tx].link[1]) \
                    tx = d[tx].link[1]; \
                memcpy((void *) &d[tn].value, &d[tx].value, sizeof d[0].value); /* move */ \
                raw = keyToRaw(KEY_REF_##C(&d[tn].value)); \
                d[tn].link[0] = CX##_erase_r_(d, d[tn].link[0], &raw, erased); \
            } else { /* unlink node */ \
                tx = tn; \
                tn = d[tn].link[ d[tn].link[0] == 0 ]; \
                /* move it to disposed nodes list */ \
                struct csmap_rep *rep = c_container_of(d, struct csmap_rep, nodes); \
                d[tx].link[1] = (CX##_size_t) rep->disp; \
                rep->disp = tx; \
            } \
        } \
        tx = d[tn].link[1]; \
        if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) { \
            if (d[tx].level > --d[tn].level) \
                d[tx].level = d[tn].level; \
                            tn = CX##_skew_(d, tn); \
            tx = d[tn].link[1] = CX##_skew_(d, d[tn].link[1]); \
                 d[tx].link[1] = CX##_skew_(d, d[tx].link[1]); \
                            tn = CX##_split_(d, tn); \
                 d[tn].link[1] = CX##_split_(d, d[tn].link[1]); \
        } \
        return tn; \
    } \
\
    STC_DEF int \
    CX##_erase(CX* self, RawKey rkey) { \
        int erased = 0; \
        CX##_size_t root = CX##_erase_r_(self->nodes, (CX##_size_t) _csmap_rep(self)->root, &rkey, &erased); \
        return erased ? (_csmap_rep(self)->root = root, --_csmap_rep(self)->size, 1) : 0; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t it) { \
        CX##_rawkey_t raw = keyToRaw(KEY_REF_##C(it.ref)), nxt; \
        CX##_next(&it); \
        if (it.ref) nxt = keyToRaw(KEY_REF_##C(it.ref)); \
        CX##_erase(self, raw); \
        if (it.ref) CX##_find_it(self, nxt, &it); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2) { \
        if (!it2.ref) { while (it1.ref) it1 = CX##_erase_at(self, it1); \
                        return it1; } \
        CX##_key_t k1 = *KEY_REF_##C(it1.ref), k2 = *KEY_REF_##C(it2.ref); \
        CX##_rawkey_t r1 = keyToRaw(&k1); \
        for (;;) { \
            if (memcmp(&k1, &k2, sizeof k1) == 0) return it1; \
            CX##_next(&it1); k1 = *KEY_REF_##C(it1.ref); \
            CX##_erase(self, r1); \
            CX##_find_it(self, (r1 = keyToRaw(&k1)), &it1); \
        } \
    } \
\
    static CX##_size_t \
    CX##_clone_r_(CX* self, CX##_node_t* src, CX##_size_t sn) { \
        if (sn == 0) return 0; \
        CX##_size_t tx, tn = CX##_node_new_(self, src[sn].level); \
        CX##_value_clone(&self->nodes[tn].value, &src[sn].value); \
        tx = CX##_clone_r_(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx; \
        tx = CX##_clone_r_(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx; \
        return tn; \
    } \
    STC_DEF CX \
    CX##_clone(CX tree) { \
        CX clone = CX##_with_capacity(_csmap_rep(&tree)->size); \
        CX##_size_t root = CX##_clone_r_(&clone, tree.nodes, (CX##_size_t) _csmap_rep(&tree)->root); \
        _csmap_rep(&clone)->root = root; \
        _csmap_rep(&clone)->size = _csmap_rep(&tree)->size; \
        return clone; \
    } \
\
    STC_DEF void \
    CX##_del_r_(CX##_node_t* d, CX##_size_t tn) { \
        if (tn) { \
            CX##_del_r_(d, d[tn].link[0]); \
            CX##_del_r_(d, d[tn].link[1]); \
            CX##_value_del(&d[tn].value); \
        } \
    } \
    STC_DEF void \
    CX##_del(CX* self) { \
        if (_csmap_rep(self)->root) { \
            CX##_del_r_(self->nodes, (CX##_size_t) _csmap_rep(self)->root); \
            c_free(_csmap_rep(self)); \
        } \
    }

#else
#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)
#endif

#endif
