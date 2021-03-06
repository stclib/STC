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

#define using_csmap(...) \
    c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

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
    _using_CBST(X, csmap, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, mappedClone, c_trivial_toraw, Mapped)

/* csset: */
#define using_csset(...) \
    c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
    using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
    using_csset_5(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw)

#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csset_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csset, Key, Key, keyCompareRaw, @@, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, @@, @@, void)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
    _using_CBST_strkey(str, csset, cstr_t, @@, @@)
#define using_csmap_str() \
    _using_CBST(str, csmap, cstr_t, cstr_t, cstr_compare_raw, cstr_del, cstr_del, \
                     cstr_from, cstr_c_str, const char*, cstr_from, cstr_c_str, const char*)

#define using_csmap_strkey(...) \
    c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
    _using_CBST_strkey(X, csmap, Mapped, c_trivial_del, c_trivial_fromraw)

#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
    _using_CBST_strkey(X, csmap, Mapped, mappedDel, mappedClone)

#define _using_CBST_strkey(X, C, Mapped, mappedDel, mappedClone) \
    _using_CBST(X, C, cstr_t, Mapped, cstr_compare_raw, mappedDel, cstr_del, \
                   cstr_from, cstr_c_str, const char*, mappedClone, c_trivial_toraw, Mapped)

#define using_csmap_strval(...) \
    c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
    using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
    using_csmap_strval_5(X, Key, keyCompare, c_trivial_del, c_trivial_fromraw)

#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_trivial_toraw, Key)

#define using_csmap_strval_7(X, Key, keyCompare, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csmap, Key, cstr_t, keyCompare, cstr_del, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, cstr_from, cstr_c_str, const char*)

#define SET_ONLY_csset(...) __VA_ARGS__
#define SET_ONLY_csmap(...)
#define MAP_ONLY_csset(...)
#define MAP_ONLY_csmap(...) __VA_ARGS__
#define KEY_REF_csset(vp)   (vp)
#define KEY_REF_csmap(vp)   (&(vp)->first)

#define _using_CBST_types(X, C, Key, Mapped) \
    typedef Key C##_##X##_key_t; \
    typedef Mapped C##_##X##_mapped_t; \
\
    typedef SET_ONLY_##C( C##_##X##_key_t ) \
            MAP_ONLY_##C( struct {C##_##X##_key_t first; \
                                  C##_##X##_mapped_t second;} ) \
    C##_##X##_value_t; \
\
    typedef struct C##_##X##_node { \
        struct C##_##X##_node *link[2]; \
        uint8_t level; \
        C##_##X##_value_t value; \
    } C##_##X##_node_t; \
\
    typedef struct { \
        C##_##X##_value_t *ref; \
        int _top; \
        C##_##X##_node_t *_tn, *_st[48]; \
    } C##_##X##_iter_t


#define _using_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_CBST_types(X, C, Key, Mapped); \
\
    typedef struct { \
        C##_##X##_node_t* root; \
        size_t size; \
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
    STC_API C##_##X \
    C##_##X##_init(void); \
    STC_INLINE bool \
    C##_##X##_empty(C##_##X m) {return m.size == 0;} \
    STC_INLINE size_t \
    C##_##X##_size(C##_##X m) {return m.size;} \
\
    STC_API void \
    C##_##X##_del_r_(C##_##X##_node_t* tn); \
\
    STC_INLINE void \
    C##_##X##_del(C##_##X* self) {C##_##X##_del_r_(self->root);} \
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
    STC_API C##_##X##_node_t* C##_##X##_clone_r_(C##_##X##_node_t *tn); \
    STC_INLINE C##_##X \
    C##_##X##_clone(C##_##X bst) { \
        C##_##X clone = {C##_##X##_clone_r_(bst.root), bst.size}; \
        return clone; \
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
            if (res.second) res.first->first = key; \
            else {keyDel(&key); mappedDel(&res.first->second);} \
            res.first->second = mapped; return res; \
        } \
        STC_INLINE C##_##X##_result_t \
        C##_##X##_put(C##_##X* self, Key k, Mapped m) { \
            return C##_##X##_insert_or_assign(self, k, m); \
        } \
        STC_INLINE C##_##X##_result_t \
        C##_##X##_emplace_or_assign(C##_##X* self, RawKey rkey, RawMapped rmapped) { \
            C##_##X##_result_t res = C##_##X##_insert_entry_(self, rkey); \
            if (res.second) res.first->first = keyFromRaw(rkey); \
            else mappedDel(&res.first->second); \
            res.first->second = mappedFromRaw(rmapped); return res; \
        } \
        STC_INLINE C##_##X##_mapped_t* \
        C##_##X##_at(const C##_##X* self, RawKey rkey) { \
            C##_##X##_iter_t it; \
            return &C##_##X##_find_it(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE C##_##X##_value_t* \
    C##_##X##_front(C##_##X* self) { \
        C##_##X##_node_t *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
    STC_INLINE C##_##X##_value_t* \
    C##_##X##_back(C##_##X* self) { \
        C##_##X##_node_t *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
    } \
\
    STC_API void \
    C##_##X##_next(C##_##X##_iter_t* it); \
\
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_begin(C##_##X* self) { \
        C##_##X##_iter_t it = {NULL, 0, self->root}; \
        C##_##X##_next(&it); return it; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_end(C##_##X* self) {\
        C##_##X##_iter_t it = {NULL}; return it; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_itval(C##_##X##_iter_t it) {return SET_ONLY_##C( it.ref ) \
                                                 MAP_ONLY_##C( &it.ref->second );} \
\
    STC_API C##_##X##_node_t* \
    C##_##X##_erase_r_(C##_##X##_node_t *tn, const C##_##X##_rawkey_t* rkey, int *erased); \
\
    STC_INLINE size_t \
    C##_##X##_erase(C##_##X* self, RawKey rkey) { \
        int erased = 0; \
        self->root = C##_##X##_erase_r_(self->root, &rkey, &erased); \
        self->size -= erased; return erased; \
    } \
    STC_INLINE size_t \
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
        C##_##X m = {(C##_##X##_node_t *) &cbst_nil, 0}; \
        return m; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_find_it(const C##_##X* self, C##_##X##_rawkey_t rkey, C##_##X##_iter_t* out) { \
        C##_##X##_node_t *tn = self->root; \
        out->_top = 0; \
        while (tn->level) { \
            int c; C##_##X##_rawkey_t rx = keyToRaw(KEY_REF_##C(&tn->value)); \
            if ((c = keyCompareRaw(&rx, &rkey)) < 0) tn = tn->link[1]; \
            else if (c > 0) {out->_st[out->_top++] = tn; tn = tn->link[0];} \
            else {out->_tn = tn->link[1]; return (out->ref = &tn->value);} \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF void \
    C##_##X##_next(C##_##X##_iter_t *it) { \
        C##_##X##_node_t *tn = it->_tn; \
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
    static C##_##X##_node_t * \
    C##_##X##_skew_(C##_##X##_node_t *tn) { \
        if (tn && tn->link[0]->level == tn->level && tn->level) { \
            C##_##X##_node_t *tmp = tn->link[0]; \
            tn->link[0] = tmp->link[1]; \
            tmp->link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
\
    static C##_##X##_node_t * \
    C##_##X##_split_(C##_##X##_node_t *tn) { \
        if (tn->link[1]->link[1]->level == tn->level && tn->level) { \
            C##_##X##_node_t *tmp = tn->link[1]; \
            tn->link[1] = tmp->link[0]; \
            tmp->link[0] = tn; \
            tn = tmp; \
            ++tn->level; \
        } \
        return tn; \
    } \
\
    static inline C##_##X##_node_t* \
    C##_##X##_insert_entry_i_(C##_##X##_node_t* tn, const C##_##X##_rawkey_t* rkey, C##_##X##_result_t* res) { \
        C##_##X##_node_t *up[64], *it = tn; \
        int c, top = 0, dir = 0; \
        while (it->level) { \
            up[top++] = it; \
            C##_##X##_rawkey_t r = keyToRaw(KEY_REF_##C(&it->value)); \
            if ((c = keyCompareRaw(&r, rkey)) == 0) {res->first = &it->value; return tn;} \
            it = it->link[(dir = (c == -1))]; \
        } \
        tn = c_new_1(C##_##X##_node_t); \
        res->first = &tn->value, res->second = true; \
        tn->link[0] = tn->link[1] = (C##_##X##_node_t*) &cbst_nil, tn->level = 1; \
        if (top == 0) return tn; \
        up[top - 1]->link[dir] = tn; \
        while (top--) { \
            if (top) dir = (up[top - 1]->link[1] == up[top]); \
            up[top] = C##_##X##_skew_(up[top]); \
            up[top] = C##_##X##_split_(up[top]); \
            if (top) up[top - 1]->link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF C##_##X##_result_t \
    C##_##X##_insert_entry_(C##_##X* self, RawKey rkey) { \
        C##_##X##_result_t res = {NULL, false}; \
        self->root = C##_##X##_insert_entry_i_(self->root, &rkey, &res); \
        self->size += res.second; \
        return res; \
    } \
\
    STC_DEF C##_##X##_node_t* \
    C##_##X##_erase_r_(C##_##X##_node_t *tn, const C##_##X##_rawkey_t* rkey, int *erased) { \
        if (tn->level == 0) \
            return tn; \
        C##_##X##_rawkey_t raw = keyToRaw(KEY_REF_##C(&tn->value)); \
        C##_##X##_node_t *tx; int c = keyCompareRaw(&raw, rkey); \
        if (c != 0) \
            tn->link[c == -1] = C##_##X##_erase_r_(tn->link[c == -1], rkey, erased); \
        else { \
            if (!*erased) {C##_##X##_value_del(&tn->value); *erased = 1;} \
            if (tn->link[0]->level && tn->link[1]->level) { \
                tx = tn->link[0]; \
                while (tx->link[1]->level) \
                    tx = tx->link[1]; \
                tn->value = tx->value; \
                raw = keyToRaw(KEY_REF_##C(&tn->value)); \
                tn->link[0] = C##_##X##_erase_r_(tn->link[0], &raw, erased); \
            } else { \
                tx = tn; \
                tn = tn->link[tn->link[0]->level == 0]; \
                c_free(tx); \
            } \
        } \
        if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) { \
            if (tn->link[1]->level > --tn->level) \
                tn->link[1]->level = tn->level; \
                          tn = C##_##X##_skew_(tn); \
            tx = tn->link[0] = C##_##X##_skew_(tn->link[0]); \
                 tx->link[0] = C##_##X##_skew_(tx->link[0]); \
                          tn = C##_##X##_split_(tn); \
                 tn->link[0] = C##_##X##_split_(tn->link[0]); \
        } \
        return tn; \
    } \
\
    STC_DEF C##_##X##_node_t* \
    C##_##X##_clone_r_(C##_##X##_node_t *tn) { \
        if (! tn->level) return tn; \
        C##_##X##_node_t *cn = c_new_1(C##_##X##_node_t); \
        cn->link[0] = C##_##X##_clone_r_(tn->link[0]); \
        cn->link[1] = C##_##X##_clone_r_(tn->link[1]); \
        cn->level = tn->level; \
        cn->value = C##_##X##_value_clone(tn->value); \
        return cn; \
    } \
\
    STC_DEF void \
    C##_##X##_del_r_(C##_##X##_node_t* tn) { \
        if (tn->level != 0) { \
            C##_##X##_del_r_(tn->link[0]); \
            C##_##X##_del_r_(tn->link[1]); \
            C##_##X##_value_del(&tn->value); \
            c_free(tn); \
        } \
    }


_using_CBST_types(_, csmap, int, int);
static csmap___node_t cbst_nil = {&cbst_nil, &cbst_nil, 0};

#else
#define _implement_CBST(X, C, Key, Mapped, keyCompareRaw, mappedDel, keyDel, \
                           keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

#endif
