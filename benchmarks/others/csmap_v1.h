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
        printf("set %d\n", i.ref->second);
    csset_sx_del(&s);
}
*/
#include <stc/ccommon.h>
#include <stdlib.h>
#include <string.h>

#define using_csmap(...) c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
            using_csmap_4(X, Key, Mapped, c_default_compare)

#define using_csmap_4(X, Key, Mapped, keyCompare) \
            using_csmap_6(X, Key, Mapped, keyCompare, c_trivial_del, c_trivial_fromraw)

#define using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, mappedClone) \
            using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, c_trivial_del, c_trivial_fromraw)

#define using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, keyDel, keyClone) \
            using_csmap_10(X, Key, Mapped, keyCompare, mappedDel, mappedClone, \
                           keyDel, keyClone, c_trivial_toraw, Key)

#define using_csmap_10(X, Key, Mapped, keyCompareRaw, mappedDel, mappedClone, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, mappedClone, c_trivial_toraw, Mapped)

/* csset: */
#define using_csset(...) c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
            using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
            using_csset_5(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw)

#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
            using_csset_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csset_##X, csset_, Key, Key, keyCompareRaw, @@, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, @@, @@, void)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
            _c_using_aatree_strkey(str, csset_, cstr_t, @@, @@)
#define using_csmap_str() \
            _c_using_aatree(csmap_str, csmap_, cstr_t, cstr_t, cstr_compare_raw, cstr_del, cstr_del, \
                            cstr_from, cstr_c_str, const char*, cstr_from, cstr_c_str, const char*)


#define using_csmap_strkey(...) c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
            _c_using_aatree_strkey(X, csmap_, Mapped, c_trivial_del, c_trivial_fromraw)

#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
            _c_using_aatree_strkey(X, csmap_, Mapped, mappedDel, mappedClone)

#define _c_using_aatree_strkey(X, C, Mapped, mappedDel, mappedClone) \
            _c_using_aatree(C##X, C, cstr_t, Mapped, cstr_compare_raw, mappedDel, cstr_del, \
                            cstr_from, cstr_c_str, const char*, mappedClone, c_trivial_toraw, Mapped)


#define using_csmap_strval(...) c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
            using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
            using_csmap_strval_5(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw)

#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
            using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csmap_strval_7(X, Key, keyCompare, keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, cstr_t, keyCompare, cstr_del, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, cstr_from, cstr_c_str, const char*)

#define SET_ONLY_csset_(...) __VA_ARGS__
#define SET_ONLY_csmap_(...)
#define MAP_ONLY_csset_(...)
#define MAP_ONLY_csmap_(...) __VA_ARGS__
#define KEY_REF_csset_(vp)   (vp)
#define KEY_REF_csmap_(vp)   (&(vp)->first)

#define _c_using_aatree_types(CX, C, Key, Mapped) \
    typedef Key CX##_key_t; \
    typedef Mapped CX##_mapped_t; \
\
    typedef SET_ONLY_##C( CX##_key_t ) \
            MAP_ONLY_##C( struct {CX##_key_t first; \
                                  CX##_mapped_t second;} ) \
    CX##_value_t; \
\
    typedef struct CX##_node { \
        struct CX##_node *link[2]; \
        uint8_t level; \
        CX##_value_t value; \
    } CX##_node_t; \
\
    typedef struct { \
        CX##_value_t *ref; \
        int _top; \
        CX##_node_t *_tn, *_st[48]; \
    } CX##_iter_t


#define _c_using_aatree(CX, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    _c_using_aatree_types(CX, C, Key, Mapped); \
\
    typedef struct { \
        CX##_node_t* root; \
        size_t size; \
    } CX; \
\
    typedef RawKey CX##_rawkey_t; \
    typedef RawMapped CX##_rawmapped_t; \
    typedef SET_ONLY_##C( CX##_rawkey_t ) \
            MAP_ONLY_##C( struct {CX##_rawkey_t first; \
                                  CX##_rawmapped_t second;} ) \
    CX##_rawvalue_t; \
\
    typedef struct { \
        CX##_value_t *ref; \
        bool inserted; \
    } CX##_result_t; \
\
    STC_API CX \
    CX##_init(void); \
    STC_INLINE bool \
    CX##_empty(CX m) {return m.size == 0;} \
    STC_INLINE size_t \
    CX##_size(CX m) {return m.size;} \
\
    STC_API void \
    CX##_del_r_(CX##_node_t* tn); \
\
    STC_INLINE void \
    CX##_del(CX* self) {CX##_del_r_(self->root);} \
    STC_INLINE void \
    CX##_clear(CX* self) {CX##_del(self); *self = CX##_init();} \
    STC_INLINE void \
    CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
\
    STC_INLINE void \
    CX##_value_del(CX##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE CX##_value_t \
    CX##_value_clone(CX##_value_t val) { \
        *KEY_REF_##C(&val) = keyFromRaw(keyToRaw(KEY_REF_##C(&val))); \
        MAP_ONLY_##C( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
\
    STC_API CX##_node_t* CX##_clone_r_(CX##_node_t *tn); \
    STC_INLINE CX \
    CX##_clone(CX bst) { \
        CX clone = {CX##_clone_r_(bst.root), bst.size}; \
        return clone; \
    } \
\
    STC_API CX##_value_t* \
    CX##_find_it(const CX* self, RawKey rkey, CX##_iter_t* out); \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawKey rkey) { \
        CX##_iter_t it; \
        CX##_find_it(self, rkey, &it); \
        return it; \
    } \
    STC_INLINE bool \
    CX##_contains(const CX* self, RawKey rkey) { \
        CX##_iter_t it; \
        return CX##_find_it(self, rkey, &it) != NULL; \
    } \
\
    STC_API CX##_result_t \
    CX##_insert_entry_(CX* self, RawKey rkey); \
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
    STC_INLINE void \
    CX##_emplace_n(CX* self, const CX##_rawvalue_t arr[], size_t n) { \
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
        STC_INLINE CX##_result_t \
        CX##_put(CX* self, Key k, Mapped m) { \
            return CX##_insert_or_assign(self, k, m); \
        } \
        STC_INLINE CX##_result_t \
        CX##_emplace_or_assign(CX* self, RawKey rkey, RawMapped rmapped) { \
            CX##_result_t res = CX##_insert_entry_(self, rkey); \
            if (res.inserted) res.ref->first = keyFromRaw(rkey); \
            else mappedDel(&res.ref->second); \
            res.ref->second = mappedFromRaw(rmapped); return res; \
        } \
        STC_INLINE CX##_mapped_t* \
        CX##_at(const CX* self, RawKey rkey) { \
            CX##_iter_t it; \
            return &CX##_find_it(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE CX##_value_t* \
    CX##_front(const CX* self) { \
        CX##_node_t *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
    STC_INLINE CX##_value_t* \
    CX##_back(const CX* self) { \
        CX##_node_t *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
    } \
\
    STC_API void \
    CX##_next(CX##_iter_t* it); \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_iter_t it = {NULL, 0, self->root}; \
        CX##_next(&it); return it; \
    } \
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) {\
        CX##_iter_t it = {NULL}; return it; \
    } \
    STC_INLINE CX##_mapped_t* \
    CX##_itval(CX##_iter_t it) {return SET_ONLY_##C( it.ref ) \
                                           MAP_ONLY_##C( &it.ref->second );} \
\
    STC_API CX##_node_t* \
    CX##_erase_r_(CX##_node_t *tn, const CX##_rawkey_t* rkey, int *erased); \
\
    STC_INLINE size_t \
    CX##_erase(CX* self, RawKey rkey) { \
        int erased = 0; \
        self->root = CX##_erase_r_(self->root, &rkey, &erased); \
        self->size -= erased; return erased; \
    } \
    STC_INLINE size_t \
    CX##_erase_at(CX* self, CX##_iter_t pos) { \
        return CX##_erase(self, keyToRaw(KEY_REF_##C(pos.ref))); \
    } \
\
    _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef CX CX##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    STC_DEF CX \
    CX##_init(void) { \
        CX m = {(CX##_node_t *) &aatree_nil, 0}; \
        return m; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_find_it(const CX* self, CX##_rawkey_t rkey, CX##_iter_t* out) { \
        CX##_node_t *tn = self->root; \
        out->_top = 0; \
        while (tn->level) { \
            int c; CX##_rawkey_t rx = keyToRaw(KEY_REF_##C(&tn->value)); \
            if ((c = keyCompareRaw(&rx, &rkey)) < 0) tn = tn->link[1]; \
            else if (c > 0) {out->_st[out->_top++] = tn; tn = tn->link[0];} \
            else {out->_tn = tn->link[1]; return (out->ref = &tn->value);} \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF void \
    CX##_next(CX##_iter_t *it) { \
        CX##_node_t *tn = it->_tn; \
        if (it->_top || tn->level) { \
            while (tn->level) { \
                it->_st[it->_top++] = tn; \
                tn = tn->link[0]; \
            } \
            tn = it->_st[--it->_top]; \
            it->_tn = tn->link[1]; \
            it->ref = &tn->value; \
        } else \
            it->ref = NULL; \
    } \
\
    static CX##_node_t * \
    CX##_skew_(CX##_node_t *tn) { \
        if (tn && tn->link[0]->level == tn->level && tn->level) { \
            CX##_node_t *tmp = tn->link[0]; \
            tn->link[0] = tmp->link[1]; \
            tmp->link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
\
    static CX##_node_t * \
    CX##_split_(CX##_node_t *tn) { \
        if (tn->link[1]->link[1]->level == tn->level && tn->level) { \
            CX##_node_t *tmp = tn->link[1]; \
            tn->link[1] = tmp->link[0]; \
            tmp->link[0] = tn; \
            tn = tmp; \
            ++tn->level; \
        } \
        return tn; \
    } \
\
    static inline CX##_node_t* \
    CX##_insert_entry_i_(CX##_node_t* tn, const CX##_rawkey_t* rkey, CX##_result_t* res) { \
        CX##_node_t *up[64], *it = tn; \
        int c, top = 0, dir = 0; \
        while (it->level) { \
            up[top++] = it; \
            CX##_rawkey_t r = keyToRaw(KEY_REF_##C(&it->value)); \
            if ((c = keyCompareRaw(&r, rkey)) == 0) {res->ref = &it->value; return tn;} \
            it = it->link[(dir = (c < 0))]; \
        } \
        tn = c_new(CX##_node_t); \
        res->ref = &tn->value, res->inserted = true; \
        tn->link[0] = tn->link[1] = (CX##_node_t*) &aatree_nil, tn->level = 1; \
        if (top == 0) return tn; \
        up[top - 1]->link[dir] = tn; \
        while (top--) { \
            if (top) dir = (up[top - 1]->link[1] == up[top]); \
            up[top] = CX##_skew_(up[top]); \
            up[top] = CX##_split_(up[top]); \
            if (top) up[top - 1]->link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF CX##_result_t \
    CX##_insert_entry_(CX* self, RawKey rkey) { \
        CX##_result_t res = {NULL, false}; \
        self->root = CX##_insert_entry_i_(self->root, &rkey, &res); \
        self->size += res.inserted; \
        return res; \
    } \
\
    STC_DEF CX##_node_t* \
    CX##_erase_r_(CX##_node_t *tn, const CX##_rawkey_t* rkey, int *erased) { \
        if (tn->level == 0) \
            return tn; \
        CX##_rawkey_t raw = keyToRaw(KEY_REF_##C(&tn->value)); \
        CX##_node_t *tx; int c = keyCompareRaw(&raw, rkey); \
        if (c != 0) \
            tn->link[c < 0] = CX##_erase_r_(tn->link[c < 0], rkey, erased); \
        else { \
            if (!*erased) {CX##_value_del(&tn->value); *erased = 1;} \
            if (tn->link[0]->level && tn->link[1]->level) { \
                tx = tn->link[0]; \
                while (tx->link[1]->level) \
                    tx = tx->link[1]; \
                tn->value = tx->value; \
                raw = keyToRaw(KEY_REF_##C(&tn->value)); \
                tn->link[0] = CX##_erase_r_(tn->link[0], &raw, erased); \
            } else { \
                tx = tn; \
                tn = tn->link[tn->link[0]->level == 0]; \
                c_free(tx); \
            } \
        } \
        if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) { \
            if (tn->link[1]->level > --tn->level) \
                tn->link[1]->level = tn->level; \
                          tn = CX##_skew_(tn); \
            tx = tn->link[0] = CX##_skew_(tn->link[0]); \
                 tx->link[0] = CX##_skew_(tx->link[0]); \
                          tn = CX##_split_(tn); \
                 tn->link[0] = CX##_split_(tn->link[0]); \
        } \
        return tn; \
    } \
\
    STC_DEF CX##_node_t* \
    CX##_clone_r_(CX##_node_t *tn) { \
        if (! tn->level) return tn; \
        CX##_node_t *cn = c_new(CX##_node_t); \
        cn->link[0] = CX##_clone_r_(tn->link[0]); \
        cn->link[1] = CX##_clone_r_(tn->link[1]); \
        cn->level = tn->level; \
        cn->value = CX##_value_clone(tn->value); \
        return cn; \
    } \
\
    STC_DEF void \
    CX##_del_r_(CX##_node_t* tn) { \
        if (tn->level != 0) { \
            CX##_del_r_(tn->link[0]); \
            CX##_del_r_(tn->link[1]); \
            CX##_value_del(&tn->value); \
            c_free(tn); \
        } \
    }


_c_using_aatree_types(csmap_VOID, csmap_, int, int);
static csmap_VOID_node_t aatree_nil = {&aatree_nil, &aatree_nil, 0};

#else
#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                            keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

#endif
