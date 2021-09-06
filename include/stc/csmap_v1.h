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
#include <stc/template.h>
#include <stdlib.h>
#include <string.h>

#define KEY_REF_csmap_(vp)   (&(vp)->first)
#define _c_aatree_complete_types(Self, C) \
    cx_MAP_ONLY( struct cx_value_t { \
        cx_key_t first; \
        cx_mapped_t second; \
    }; ) \
    struct cx_node_t { \
        struct cx_node_t *link[2]; \
        uint8_t level; \
        cx_value_t value; \
    }

#ifndef cx_forwarded
    _c_aatree_types(Self, C, i_KEY, i_VAL);
#endif    

    _c_aatree_complete_types(Self, C); \
\
    typedef i_KEYRAW cx_rawkey_t; \
    typedef i_VALRAW cx_memb(_rawmapped_t); \
    typedef cx_SET_ONLY( cx_rawkey_t ) \
            cx_MAP_ONLY( struct { cx_rawkey_t first; \
                                   cx_memb(_rawmapped_t) second; } ) \
    cx_rawvalue_t; \
\
    STC_API Self               cx_memb(_init)(void); \
    STC_API cx_value_t*    cx_memb(_find_it)(const Self* self, i_KEYRAW rkey, cx_iter_t* out); \
    STC_API cx_iter_t      cx_memb(_lower_bound)(const Self* self, i_KEYRAW rkey); \
    STC_API cx_value_t*    cx_memb(_front)(const Self* self); \
    STC_API cx_value_t*    cx_memb(_back)(const Self* self); \
    STC_API cx_iter_t      cx_memb(_erase_at)(Self* self, cx_iter_t it); \
    STC_API cx_iter_t      cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2); \
    STC_API cx_node_t*     cx_memb(_erase_r_)(cx_node_t *tn, const cx_rawkey_t* rkey, int *erased); \
    STC_API void             cx_memb(_del_r_)(cx_node_t* tn); \
    STC_API cx_node_t*     cx_memb(_clone_r_)(cx_node_t *tn); \
    STC_API cx_result_t    cx_memb(_insert_entry_)(Self* self, i_KEYRAW rkey); \
    STC_API void             cx_memb(_next)(cx_iter_t* it); \
\
    STC_INLINE bool          cx_memb(_empty)(Self cx) { return cx.size == 0; } \
    STC_INLINE size_t        cx_memb(_size)(Self cx) { return cx.size; } \
    STC_INLINE void          cx_memb(_del)(Self* self) { cx_memb(_del_r_)(self->root); } \
    STC_INLINE void          cx_memb(_clear)(Self* self) { cx_memb(_del)(self); *self = cx_memb(_init)(); } \
    STC_INLINE void          cx_memb(_swap)(Self* a, Self* b) {c_swap(Self, *a, *b); } \
    STC_INLINE Self            cx_memb(_clone)(Self cx) { return c_make(Self){ cx_memb(_clone_r_)(cx.root), cx.size}; } \
    STC_INLINE cx_iter_t   cx_memb(_find)(const Self* self, i_KEYRAW rkey) \
                                {cx_iter_t it; cx_memb(_find_it)(self, rkey, &it); return it; } \
    STC_INLINE bool          cx_memb(_contains)(const Self* self, i_KEYRAW rkey) \
                                {cx_iter_t it; return cx_memb(_find_it)(self, rkey, &it) != NULL; } \
    STC_INLINE cx_value_t* cx_memb(_get)(const Self* self, i_KEYRAW rkey) \
                                {cx_iter_t it; return cx_memb(_find_it)(self, rkey, &it); } \
\
    STC_INLINE void \
    cx_memb(_value_del)(cx_value_t* val) { \
        i_KEYDEL(cx_keyref(val)); \
        cx_MAP_ONLY( i_VALDEL(&val->second); ) \
    } \
\
    STC_INLINE void \
    cx_memb(_value_clone)(cx_value_t* dst, cx_value_t* val) { \
        *cx_keyref(dst) = i_KEYFROM(i_KEYTO(cx_keyref(val))); \
        cx_MAP_ONLY( dst->second = i_VALFROM(i_VALTO(&val->second)); ) \
    } \
\
    STC_INLINE cx_result_t \
    cx_memb(_emplace)(Self* self, i_KEYRAW rkey cx_MAP_ONLY(, i_VALRAW rmapped)) { \
        cx_result_t res = cx_memb(_insert_entry_)(self, rkey); \
        if (res.inserted) { \
            *cx_keyref(res.ref) = i_KEYFROM(rkey); \
            cx_MAP_ONLY(res.ref->second = i_VALFROM(rmapped);) \
        } \
        return res; \
    } \
\
    STC_INLINE void \
    cx_memb(_emplace_items)(Self* self, const cx_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) cx_SET_ONLY( cx_memb(_emplace)(self, arr[i]); ) \
                                   cx_MAP_ONLY( cx_memb(_emplace)(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE cx_result_t \
    cx_memb(_insert)(Self* self, i_KEY key cx_MAP_ONLY(, i_VAL mapped)) { \
        cx_result_t res = cx_memb(_insert_entry_)(self, i_KEYTO(&key)); \
        if (res.inserted) {*cx_keyref(res.ref) = key; cx_MAP_ONLY( res.ref->second = mapped; )} \
        else              {i_KEYDEL(&key); cx_MAP_ONLY( i_VALDEL(&mapped); )} \
        return res; \
    } \
\
    cx_MAP_ONLY( \
        STC_INLINE cx_result_t \
        cx_memb(_insert_or_assign)(Self* self, i_KEY key, i_VAL mapped) { \
            cx_result_t res = cx_memb(_insert_entry_)(self, i_KEYTO(&key)); \
            if (res.inserted) res.ref->first = key; \
            else {i_KEYDEL(&key); i_VALDEL(&res.ref->second); } \
            res.ref->second = mapped; return res; \
        } \
    \
        STC_INLINE cx_result_t \
        cx_memb(_put)(Self* self, i_KEY key, i_VAL mapped) { \
            return cx_memb(_insert_or_assign)(self, key, mapped); \
        } \
    \
        STC_INLINE cx_result_t \
        cx_memb(_emplace_or_assign)(Self* self, i_KEYRAW rkey, i_VALRAW rmapped) { \
            cx_result_t res = cx_memb(_insert_entry_)(self, rkey); \
            if (res.inserted) res.ref->first = i_KEYFROM(rkey); \
            else i_VALDEL(&res.ref->second); \
            res.ref->second = i_VALFROM(rmapped); return res; \
        } \
    \
        STC_INLINE cx_mapped_t* \
        cx_memb(_at)(const Self* self, i_KEYRAW rkey) { \
            cx_iter_t it; \
            return &cx_memb(_find_it)(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE cx_iter_t \
    cx_memb(_begin)(const Self* self) { \
        cx_iter_t it = {NULL, 0, self->root}; \
        cx_memb(_next)(&it); return it; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_end)(const Self* self) {\
        cx_iter_t it = {NULL}; return it; \
    } \
\
    STC_INLINE size_t \
    cx_memb(_erase)(Self* self, i_KEYRAW rkey) { \
        int erased = 0; \
        self->root = cx_memb(_erase_r_)(self->root, &rkey, &erased); \
        self->size -= erased; return erased; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_fwd)(cx_iter_t it, size_t n) { \
        while (n-- && it.ref) cx_memb(_next)(&it); \
        return it; \
    } \
\
    _c_implement_aatree(Self, C, i_KEY, i_VAL, i_CMP, \
                        i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW, \
                        i_KEYDEL, i_KEYFROM, i_KEYTO, i_KEYRAW) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

_c_aatree_types(csmap_SENTINEL, csmap_, int, int);
_c_aatree_complete_types(csmap_SENTINEL, csmap_);
static csmap_SENTINEL_node_t _aatree_sentinel = {&_aatree_sentinel, &_aatree_sentinel, 0};

#define _c_implement_aatree(Self, C, i_KEY, i_VAL, i_CMP, \
                            i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW, \
                            i_KEYDEL, i_KEYFROM, i_KEYTO, i_KEYRAW) \
    STC_DEF Self \
    cx_memb(_init)(void) { \
        Self cx = {(cx_node_t *) &_aatree_sentinel, 0}; \
        return cx; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_front)(const Self* self) { \
        cx_node_t *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_back)(const Self* self) { \
        cx_node_t *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_find_it)(const Self* self, cx_rawkey_t rkey, cx_iter_t* out) { \
        cx_node_t *tn = self->root; \
        out->_top = 0; \
        while (tn->level) { \
            int c; cx_rawkey_t rx = i_KEYTO(cx_keyref(&tn->value)); \
            if ((c = i_CMP(&rx, &rkey)) < 0) tn = tn->link[1]; \
            else if (c > 0) {out->_st[out->_top++] = tn; tn = tn->link[0]; } \
            else {out->_tn = tn->link[1]; return (out->ref = &tn->value); } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_lower_bound)(const Self* self, i_KEYRAW rkey) { \
        cx_iter_t it; \
        cx_memb(_find_it)(self, rkey, &it); \
        if (!it.ref && it._top) { \
            cx_node_t *tn = it._st[--it._top]; \
            it._tn = tn->link[1]; \
            it.ref = &tn->value; \
        } \
        return it; \
    } \
\
    STC_DEF void \
    cx_memb(_next)(cx_iter_t *it) { \
        cx_node_t *tn = it->_tn; \
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
    static cx_node_t * \
    cx_memb(_skew_)(cx_node_t *tn) { \
        if (tn && tn->link[0]->level == tn->level && tn->level) { \
            cx_node_t *tmp = tn->link[0]; \
            tn->link[0] = tmp->link[1]; \
            tmp->link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
\
    static cx_node_t * \
    cx_memb(_split_)(cx_node_t *tn) { \
        if (tn->link[1]->link[1]->level == tn->level && tn->level) { \
            cx_node_t *tmp = tn->link[1]; \
            tn->link[1] = tmp->link[0]; \
            tmp->link[0] = tn; \
            tn = tmp; \
            ++tn->level; \
        } \
        return tn; \
    } \
\
    static inline cx_node_t* \
    cx_memb(_insert_entry_i_)(cx_node_t* tn, const cx_rawkey_t* rkey, cx_result_t* res) { \
        cx_node_t *up[64], *tx = tn; \
        int c, top = 0, dir = 0; \
        while (tx->level) { \
            up[top++] = tx; \
            cx_rawkey_t r = i_KEYTO(cx_keyref(&tx->value)); \
            if (!(c = i_CMP(&r, rkey))) {res->ref = &tx->value; return tn; } \
            tx = tx->link[(dir = (c < 0))]; \
        } \
        tn = c_new(cx_node_t); \
        res->ref = &tn->value, res->inserted = true; \
        tn->link[0] = tn->link[1] = (cx_node_t*) &_aatree_sentinel, tn->level = 1; \
        if (top == 0) return tn; \
        up[top - 1]->link[dir] = tn; \
        while (top--) { \
            if (top) dir = (up[top - 1]->link[1] == up[top]); \
            up[top] = cx_memb(_skew_)(up[top]); \
            up[top] = cx_memb(_split_)(up[top]); \
            if (top) up[top - 1]->link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF cx_result_t \
    cx_memb(_insert_entry_)(Self* self, i_KEYRAW rkey) { \
        cx_result_t res = {NULL, false}; \
        self->root = cx_memb(_insert_entry_i_)(self->root, &rkey, &res); \
        self->size += res.inserted; \
        return res; \
    } \
\
    STC_DEF cx_node_t* \
    cx_memb(_erase_r_)(cx_node_t *tn, const cx_rawkey_t* rkey, int *erased) { \
        if (tn->level == 0) \
            return tn; \
        cx_rawkey_t raw = i_KEYTO(cx_keyref(&tn->value)); \
        cx_node_t *tx; int c = i_CMP(&raw, rkey); \
        if (c != 0) \
            tn->link[c < 0] = cx_memb(_erase_r_)(tn->link[c < 0], rkey, erased); \
        else { \
            if (!*erased) { cx_memb(_value_del)(&tn->value); *erased = 1; } \
            if (tn->link[0]->level && tn->link[1]->level) { \
                tx = tn->link[0]; \
                while (tx->link[1]->level) \
                    tx = tx->link[1]; \
                tn->value = tx->value; \
                raw = i_KEYTO(cx_keyref(&tn->value)); \
                tn->link[0] = cx_memb(_erase_r_)(tn->link[0], &raw, erased); \
            } else { \
                tx = tn; \
                tn = tn->link[tn->link[0]->level == 0]; \
                c_free(tx); \
            } \
        } \
        if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) { \
            if (tn->link[1]->level > --tn->level) \
                tn->link[1]->level = tn->level; \
                          tn = cx_memb(_skew_)(tn); \
            tx = tn->link[0] = cx_memb(_skew_)(tn->link[0]); \
                 tx->link[0] = cx_memb(_skew_)(tx->link[0]); \
                          tn = cx_memb(_split_)(tn); \
                 tn->link[0] = cx_memb(_split_)(tn->link[0]); \
        } \
        return tn; \
    } \
    STC_DEF cx_iter_t \
    cx_memb(_erase_at)(Self* self, cx_iter_t it) { \
        cx_rawkey_t raw = i_KEYTO(cx_keyref(it.ref)), nxt; \
        cx_memb(_next)(&it); \
        if (it.ref) nxt = i_KEYTO(cx_keyref(it.ref)); \
        cx_memb(_erase)(self, raw); \
        if (it.ref) cx_memb(_find_it)(self, nxt, &it); \
        return it; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2) { \
        if (!it2.ref) { while (it1.ref) it1 = cx_memb(_erase_at)(self, it1); \
                        return it1; } \
        cx_key_t k1 = *cx_keyref(it1.ref), k2 = *cx_keyref(it2.ref); \
        cx_rawkey_t r1 = i_KEYTO(&k1); \
        for (;;) { \
            if (memcmp(&k1, &k2, sizeof k1) == 0) return it1; \
            cx_memb(_next)(&it1); k1 = *cx_keyref(it1.ref); \
            cx_memb(_erase)(self, r1); \
            cx_memb(_find_it)(self, (r1 = i_KEYTO(&k1)), &it1); \
        } \
    } \
\
    STC_DEF cx_node_t* \
    cx_memb(_clone_r_)(cx_node_t *tn) { \
        if (! tn->level) return tn; \
        cx_node_t *cn = c_new(cx_node_t); \
        cn->link[0] = cx_memb(_clone_r_)(tn->link[0]); \
        cn->link[1] = cx_memb(_clone_r_)(tn->link[1]); \
        cn->level = tn->level; \
        cx_memb(_value_clone)(&cn->value, &tn->value); \
        return cn; \
    } \
\
    STC_DEF void \
    cx_memb(_del_r_)(cx_node_t* tn) { \
        if (tn->level != 0) { \
            cx_memb(_del_r_)(tn->link[0]); \
            cx_memb(_del_r_)(tn->link[1]); \
            cx_memb(_value_del)(&tn->value); \
            c_free(tn); \
        } \
    }

#endif
#endif
