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
#include <stc/ccommon.h>
#include <stdlib.h>
#include <string.h>

#define using_csmap(...) c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
            using_csmap_4(X, Key, Mapped, c_default_compare)
#define using_csmap_4(X, Key, Mapped, keyCompare) \
            using_csmap_6(X, Key, Mapped, keyCompare, c_default_del, c_default_fromraw)
#define using_csmap_5(X, Key, Mapped, keyCompare, mappedDel) \
            using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, c_no_clone)
#define using_csmap_6(X, Key, Mapped, keyCompare, mappedDel, mappedClone) \
            using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedClone, c_default_toraw, Mapped)
#define using_csmap_8(X, Key, Mapped, keyCompare, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
            using_csmap_12(X, Key, Mapped, keyCompare, \
                           mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                           c_default_del, c_default_fromraw, c_default_toraw, Key)

#define using_csmap_12(X, Key, Mapped, keyCompareRaw, \
                       mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)


#define using_csmap_keydef(...) c_MACRO_OVERLOAD(using_csmap_keydef, __VA_ARGS__)

#define using_csmap_keydef_6(X, Key, Mapped, keyCompare, keyDel, keyClone) \
            using_csmap_keydef_8(X, Key, Mapped, keyCompare, \
                           keyDel, keyClone, c_default_toraw, Key)

#define using_csmap_keydef_8(X, Key, Mapped, keyCompareRaw, \
                       keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, Mapped, keyCompareRaw, \
                            c_default_del, c_default_fromraw, c_default_toraw, Mapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_csmap_str() \
            _c_using_aatree(csmap_str, csmap_, cstr, cstr, c_rawstr_compare, \
                            cstr_del, cstr_from, cstr_str, const char*, \
                            cstr_del, cstr_from, cstr_str, const char*)


#define using_csmap_strkey(...) c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
            using_csmap_strkey_4(X, Mapped, c_default_del, c_default_fromraw)
#define using_csmap_strkey_3(X, Mapped, mappedDel) \
            using_csmap_strkey_4(X, Mapped, mappedDel, c_no_clone)
#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
            _c_using_aatree_strkey(X, csmap_, Mapped, mappedDel, mappedClone, c_default_toraw, Mapped)
#define using_csmap_strkey_6(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
            _c_using_aatree_strkey(X, csmap_, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped)

#define _c_using_aatree_strkey(X, C, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
            _c_using_aatree(C##X, C, cstr, Mapped, c_rawstr_compare, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            cstr_del, cstr_from, cstr_str, const char*)


#define using_csmap_strval(...) c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
            using_csmap_strval_3(X, Key, c_default_compare)
#define using_csmap_strval_3(X, Key, keyCompare) \
            using_csmap_strval_5(X, Key, keyCompare, c_default_del, c_default_fromraw)
#define using_csmap_strval_4(X, Key, keyCompare, keyDel) \
            using_csmap_strval_5(X, Key, keyCompare, keyDel, c_no_clone)
#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
            using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_default_toraw, Key)

#define using_csmap_strval_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
            _c_using_aatree(csmap_##X, csmap_, Key, cstr, keyCompareRaw, \
                            cstr_del, cstr_from, cstr_str, const char*, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)

#define SET_ONLY_csmap_(...)
#define MAP_ONLY_csmap_(...) __VA_ARGS__
#define KEY_REF_csmap_(vp)   (&(vp)->first)

#define _c_aatree_types(CX, C, Key, Mapped) \
    typedef Key CX##_key_t; \
    typedef Mapped CX##_mapped_t; \
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
        int _top; \
        CX##_node_t *_tn, *_st[36]; \
    } CX##_iter_t; \
\
    typedef struct { \
        CX##_node_t* root; \
        size_t size; \
    } CX

#define _c_aatree_complete_types(CX, C) \
    MAP_ONLY_##C( struct CX##_value_t { \
        CX##_key_t first; \
        CX##_mapped_t second; \
    }; ) \
    struct CX##_node_t { \
        struct CX##_node_t *link[2]; \
        uint8_t level; \
        CX##_value_t value; \
    }

#define _c_using_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                        mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                        keyDel, keyFromRaw, keyToRaw, RawKey) \
    _c_aatree_types(CX, C, Key, Mapped); \
    _c_aatree_complete_types(CX, C); \
\
    typedef RawKey CX##_rawkey_t; \
    typedef RawMapped CX##_rawmapped_t; \
    typedef SET_ONLY_##C( CX##_rawkey_t ) \
            MAP_ONLY_##C( struct { CX##_rawkey_t first; \
                                   CX##_rawmapped_t second; } ) \
    CX##_rawvalue_t; \
\
    STC_API CX               CX##_init(void); \
    STC_API CX##_value_t*    CX##_find_it(const CX* self, RawKey rkey, CX##_iter_t* out); \
    STC_API CX##_iter_t      CX##_lower_bound(const CX* self, RawKey rkey); \
    STC_API CX##_value_t*    CX##_front(const CX* self); \
    STC_API CX##_value_t*    CX##_back(const CX* self); \
    STC_API CX##_iter_t      CX##_erase_at(CX* self, CX##_iter_t it); \
    STC_API CX##_iter_t      CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2); \
    STC_API CX##_node_t*     CX##_erase_r_(CX##_node_t *tn, const CX##_rawkey_t* rkey, int *erased); \
    STC_API void             CX##_del_r_(CX##_node_t* tn); \
    STC_API CX##_node_t*     CX##_clone_r_(CX##_node_t *tn); \
    STC_API CX##_result_t    CX##_insert_entry_(CX* self, RawKey rkey); \
    STC_API void             CX##_next(CX##_iter_t* it); \
\
    STC_INLINE bool          CX##_empty(CX cx) {return cx.size == 0;} \
    STC_INLINE size_t        CX##_size(CX cx) {return cx.size;} \
    STC_INLINE void          CX##_del(CX* self) {CX##_del_r_(self->root);} \
    STC_INLINE void          CX##_clear(CX* self) {CX##_del(self); *self = CX##_init();} \
    STC_INLINE void          CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
    STC_INLINE CX            CX##_clone(CX cx) {return c_make(CX){CX##_clone_r_(cx.root), cx.size};} \
    STC_INLINE CX##_iter_t   CX##_find(const CX* self, RawKey rkey) \
                                {CX##_iter_t it; CX##_find_it(self, rkey, &it); return it;} \
    STC_INLINE bool          CX##_contains(const CX* self, RawKey rkey) \
                                {CX##_iter_t it; return CX##_find_it(self, rkey, &it) != NULL;} \
    STC_INLINE CX##_value_t* CX##_get(const CX* self, RawKey rkey) \
                                {CX##_iter_t it; return CX##_find_it(self, rkey, &it);} \
\
    STC_INLINE void \
    CX##_value_del(CX##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
\
    STC_INLINE void \
    CX##_value_clone(CX##_value_t* dst, CX##_value_t* val) { \
        *KEY_REF_##C(dst) = keyFromRaw(keyToRaw(KEY_REF_##C(val))); \
        MAP_ONLY_##C( dst->second = mappedFromRaw(mappedToRaw(&val->second)); ) \
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
        CX##_iter_t it = {NULL, 0, self->root}; \
        CX##_next(&it); return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) {\
        CX##_iter_t it = {NULL}; return it; \
    } \
\
    STC_INLINE size_t \
    CX##_erase(CX* self, RawKey rkey) { \
        int erased = 0; \
        self->root = CX##_erase_r_(self->root, &rkey, &erased); \
        self->size -= erased; return erased; \
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

_c_aatree_types(csmap_SENTINEL, csmap_, int, int);
_c_aatree_complete_types(csmap_SENTINEL, csmap_);
static csmap_SENTINEL_node_t _aatree_sentinel = {&_aatree_sentinel, &_aatree_sentinel, 0};

#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey) \
    STC_DEF CX \
    CX##_init(void) { \
        CX cx = {(CX##_node_t *) &_aatree_sentinel, 0}; \
        return cx; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_front(const CX* self) { \
        CX##_node_t *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_back(const CX* self) { \
        CX##_node_t *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
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
    STC_DEF CX##_iter_t \
    CX##_lower_bound(const CX* self, RawKey rkey) { \
        CX##_iter_t it; \
        CX##_find_it(self, rkey, &it); \
        if (!it.ref && it._top) { \
            CX##_node_t *tn = it._st[--it._top]; \
            it._tn = tn->link[1]; \
            it.ref = &tn->value; \
        } \
        return it; \
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
        CX##_node_t *up[64], *tx = tn; \
        int c, top = 0, dir = 0; \
        while (tx->level) { \
            up[top++] = tx; \
            CX##_rawkey_t r = keyToRaw(KEY_REF_##C(&tx->value)); \
            if (!(c = keyCompareRaw(&r, rkey))) {res->ref = &tx->value; return tn;} \
            tx = tx->link[(dir = (c < 0))]; \
        } \
        tn = c_new(CX##_node_t); \
        res->ref = &tn->value, res->inserted = true; \
        tn->link[0] = tn->link[1] = (CX##_node_t*) &_aatree_sentinel, tn->level = 1; \
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
    STC_DEF CX##_node_t* \
    CX##_clone_r_(CX##_node_t *tn) { \
        if (! tn->level) return tn; \
        CX##_node_t *cn = c_new(CX##_node_t); \
        cn->link[0] = CX##_clone_r_(tn->link[0]); \
        cn->link[1] = CX##_clone_r_(tn->link[1]); \
        cn->level = tn->level; \
        CX##_value_clone(&cn->value, &tn->value); \
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

#else
#define _c_implement_aatree(CX, C, Key, Mapped, keyCompareRaw, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            keyDel, keyFromRaw, keyToRaw, RawKey)
#endif
#endif
