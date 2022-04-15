/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
    c_autovar (csmap_mx m = csmap_mx_init(), csmap_mx_drop(&m))
    {
        csmap_mx_insert(&m, 5, 'a');
        csmap_mx_insert(&m, 8, 'b');
        csmap_mx_insert(&m, 12, 'c');

        csmap_mx_iter it = csmap_mx_find(&m, 10); // none
        char val = csmap_mx_find(&m, 5).ref->second;
        csmap_mx_push(&m, 5, 'd'); // update
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
#define _c_aatree_complete_types(_cx_self, C) \
    _i_MAP_ONLY( struct _cx_value { \
        _cx_key first; \
        _cx_mapped second; \
    }; ) \
    struct _cx_node { \
        struct _cx_node *link[2]; \
        uint8_t level; \
        _cx_value value; \
    }

#ifndef cx_forwarded
    _c_aatree_types(_cx_self, C, i_key, i_val, i_size);
#endif    

    _c_aatree_complete_types(_cx_self, C); \
\
    typedef i_keyraw _cx_rawkey; \
    typedef i_valraw _cx_memb(_rawmapped); \
    typedef _i_SET_ONLY( _cx_rawkey ) \
            _i_MAP_ONLY( struct { _cx_rawkey first; \
                                   _cx_memb(_rawmapped) second; } ) \
    _cx_raw; \
\
    STC_API _cx_self               _cx_memb(_init)(void); \
    STC_API _cx_value*    _cx_memb(_find_it)(const _cx_self* self, i_keyraw rkey, _cx_iter* out); \
    STC_API _cx_iter      _cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey); \
    STC_API _cx_value*    _cx_memb(_front)(const _cx_self* self); \
    STC_API _cx_value*    _cx_memb(_back)(const _cx_self* self); \
    STC_API _cx_iter      _cx_memb(_erase_at)(_cx_self* self, _cx_iter it); \
    STC_API _cx_iter      _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2); \
    STC_API _cx_node*     _cx_memb(_erase_r_)(_cx_node *tn, const _cx_rawkey* rkey, int *erased); \
    STC_API void             _cx_memb(_del_r_)(_cx_node* tn); \
    STC_API _cx_node*     _cx_memb(_clone_r_)(_cx_node *tn); \
    STC_API _cx_result    _cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey); \
    STC_API void             _cx_memb(_next)(_cx_iter* it); \
\
    STC_INLINE bool          _cx_memb(_empty)(_cx_self cx) { return cx.size == 0; } \
    STC_INLINE size_t        _cx_memb(_size)(_cx_self cx) { return cx.size; } \
    STC_INLINE void          _cx_memb(_drop)(_cx_self* self) { _cx_memb(_del_r_)(self->root); } \
    STC_INLINE void          _cx_memb(_clear)(_cx_self* self) { _cx_memb(_drop)(self); *self = _cx_memb(_init)(); } \
    STC_INLINE void          _cx_memb(_swap)(_cx_self* a, _cx_self* b) {c_swap(_cx_self, *a, *b); } \
    STC_INLINE _cx_self            _cx_memb(_clone)(_cx_self cx) { return c_make(_cx_self){ _cx_memb(_clone_r_)(cx.root), cx.size}; } \
    STC_INLINE _cx_iter   _cx_memb(_find)(const _cx_self* self, i_keyraw rkey) \
                                {_cx_iter it; _cx_memb(_find_it)(self, rkey, &it); return it; } \
    STC_INLINE bool          _cx_memb(_contains)(const _cx_self* self, i_keyraw rkey) \
                                {_cx_iter it; return _cx_memb(_find_it)(self, rkey, &it) != NULL; } \
    STC_INLINE _cx_value* _cx_memb(_get)(const _cx_self* self, i_keyraw rkey) \
                                {_cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); } \
\
    STC_INLINE void \
    _cx_memb(_value_drop)(_cx_value* val) { \
        i_keydrop(_i_keyref(val)); \
        _i_MAP_ONLY( i_valdrop((&val->second)); ) \
    } \
\
    STC_INLINE void \
    _cx_memb(_value_clone)(_cx_value* dst, _cx_value* val) { \
        *_i_keyref(dst) = i_keyfrom(i_keyto(_i_keyref(val))); \
        _i_MAP_ONLY( i_valraw r = i_valto((&val->second)); dst->second = i_valfrom(r); ) \
    } \
\
    STC_INLINE _cx_result \
    _cx_memb(_emplace)(_cx_self* self, i_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) { \
        _cx_result res = _cx_memb(_insert_entry_)(self, rkey); \
        if (res.inserted) { \
            *_i_keyref(res.ref) = i_keyfrom(rkey); \
            _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);) \
        } \
        return res; \
    } \
\
    STC_INLINE void \
    _cx_memb(_emplace_items)(_cx_self* self, const _cx_raw arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) _i_SET_ONLY( _cx_memb(_emplace)(self, arr[i]); ) \
                                   _i_MAP_ONLY( _cx_memb(_emplace)(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE _cx_result \
    _cx_memb(_insert)(_cx_self* self, i_key key _i_MAP_ONLY(, i_val mapped)) { \
        _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto((&key))); \
        if (res.inserted) {*_i_keyref(res.ref) = key; _i_MAP_ONLY( res.ref->second = mapped; )} \
        else              {i_keydrop((&key)); _i_MAP_ONLY( i_valdrop((&mapped)); )} \
        return res; \
    } \
\
    _i_MAP_ONLY( \
        STC_INLINE _cx_result \
        _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped) { \
            _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto((&key))); \
            if (res.inserted) res.ref->first = key; \
            else {i_keydrop((&key)); i_valdrop((&res.ref->second)); } \
            res.ref->second = mapped; return res; \
        } \
    \
        STC_INLINE _cx_result \
        _cx_memb(_put)(_cx_self* self, i_key key, i_val mapped) { \
            return _cx_memb(_insert_or_assign)(self, key, mapped); \
        } \
    \
        STC_INLINE _cx_result \
        _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped) { \
            _cx_result res = _cx_memb(_insert_entry_)(self, rkey); \
            if (res.inserted) res.ref->first = i_keyfrom(rkey); \
            else i_valdrop((&res.ref->second)); \
            res.ref->second = i_valfrom(rmapped); return res; \
        } \
    \
        STC_INLINE const _cx_mapped* \
        _cx_memb(_at)(const _cx_self* self, i_keyraw rkey) { \
            _cx_iter it; \
            return &_cx_memb(_find_it)(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE _cx_iter \
    _cx_memb(_begin)(const _cx_self* self) { \
        _cx_iter it = {NULL, 0, self->root}; \
        _cx_memb(_next)(&it); return it; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_end)(const _cx_self* self) {\
        _cx_iter it = {NULL}; return it; \
    } \
\
    STC_INLINE size_t \
    _cx_memb(_erase)(_cx_self* self, i_keyraw rkey) { \
        int erased = 0; \
        self->root = _cx_memb(_erase_r_)(self->root, &rkey, &erased); \
        self->size -= erased; return erased; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_advance)(_cx_iter it, size_t n) { \
        while (n-- && it.ref) _cx_memb(_next)(&it); \
        return it; \
    } \
\
    _c_implement_aatree(_cx_self, C, i_key, i_val, i_cmp, \
                        i_valdrop, i_valfrom, i_valto, i_valraw, \
                        i_keydrop, i_keyfrom, i_keyto, i_keyraw) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if defined(_i_implement)

_c_aatree_types(csmap_SENTINEL, csmap_, int, int, i_size);
_c_aatree_complete_types(csmap_SENTINEL, csmap_);
static csmap_SENTINEL_node _aatree_sentinel = {&_aatree_sentinel, &_aatree_sentinel, 0};

#define _c_implement_aatree(_cx_self, C, i_key, i_val, i_cmp, \
                            i_valdrop, i_valfrom, i_valto, i_valraw, \
                            i_keydrop, i_keyfrom, i_keyto, i_keyraw) \
    STC_DEF _cx_self \
    _cx_memb(_init)(void) { \
        _cx_self cx = {(_cx_node *) &_aatree_sentinel, 0}; \
        return cx; \
    } \
\
    STC_DEF _cx_value* \
    _cx_memb(_front)(const _cx_self* self) { \
        _cx_node *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
\
    STC_DEF _cx_value* \
    _cx_memb(_back)(const _cx_self* self) { \
        _cx_node *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
    } \
\
    STC_DEF _cx_value* \
    _cx_memb(_find_it)(const _cx_self* self, _cx_rawkey rkey, _cx_iter* out) { \
        _cx_node *tn = self->root; \
        out->_top = 0; \
        while (tn->level) { \
            int c; _cx_rawkey rx = i_keyto(_i_keyref(&tn->value)); \
            if ((c = (i_cmp((&rx), (&rkey)))) < 0) tn = tn->link[1]; \
            else if (c > 0) {out->_st[out->_top++] = tn; tn = tn->link[0]; } \
            else {out->_tn = tn->link[1]; return (out->ref = &tn->value); } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey) { \
        _cx_iter it; \
        _cx_memb(_find_it)(self, rkey, &it); \
        if (!it.ref && it._top) { \
            _cx_node *tn = it._st[--it._top]; \
            it._tn = tn->link[1]; \
            it.ref = &tn->value; \
        } \
        return it; \
    } \
\
    STC_DEF void \
    _cx_memb(_next)(_cx_iter *it) { \
        _cx_node *tn = it->_tn; \
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
    static _cx_node * \
    _cx_memb(_skew_)(_cx_node *tn) { \
        if (tn && tn->link[0]->level == tn->level && tn->level) { \
            _cx_node *tmp = tn->link[0]; \
            tn->link[0] = tmp->link[1]; \
            tmp->link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
\
    static _cx_node * \
    _cx_memb(_split_)(_cx_node *tn) { \
        if (tn->link[1]->link[1]->level == tn->level && tn->level) { \
            _cx_node *tmp = tn->link[1]; \
            tn->link[1] = tmp->link[0]; \
            tmp->link[0] = tn; \
            tn = tmp; \
            ++tn->level; \
        } \
        return tn; \
    } \
\
    static inline _cx_node* \
    _cx_memb(_insert_entry_i_)(_cx_node* tn, const _cx_rawkey* rkey, _cx_result* res) { \
        _cx_node *up[64], *tx = tn; \
        int c, top = 0, dir = 0; \
        while (tx->level) { \
            up[top++] = tx; \
            _cx_rawkey r = i_keyto(_i_keyref(&tx->value)); \
            if (!(c = (i_cmp((&r), rkey)))) {res->ref = &tx->value; return tn; } \
            tx = tx->link[(dir = (c < 0))]; \
        } \
        tn = c_alloc(_cx_node); \
        res->ref = &tn->value, res->inserted = true; \
        tn->link[0] = tn->link[1] = (_cx_node*) &_aatree_sentinel, tn->level = 1; \
        if (top == 0) return tn; \
        up[top - 1]->link[dir] = tn; \
        while (top--) { \
            if (top) dir = (up[top - 1]->link[1] == up[top]); \
            up[top] = _cx_memb(_skew_)(up[top]); \
            up[top] = _cx_memb(_split_)(up[top]); \
            if (top) up[top - 1]->link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF _cx_result \
    _cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey) { \
        _cx_result res = {NULL, false}; \
        self->root = _cx_memb(_insert_entry_i_)(self->root, &rkey, &res); \
        self->size += res.inserted; \
        return res; \
    } \
\
    STC_DEF _cx_node* \
    _cx_memb(_erase_r_)(_cx_node *tn, const _cx_rawkey* rkey, int *erased) { \
        if (tn->level == 0) \
            return tn; \
        _cx_rawkey raw = i_keyto(_i_keyref(&tn->value)); \
        _cx_node *tx; int c = (i_cmp((&raw), rkey)); \
        if (c != 0) \
            tn->link[c < 0] = _cx_memb(_erase_r_)(tn->link[c < 0], rkey, erased); \
        else { \
            if (!*erased) { _cx_memb(_value_drop)(&tn->value); *erased = 1; } \
            if (tn->link[0]->level && tn->link[1]->level) { \
                tx = tn->link[0]; \
                while (tx->link[1]->level) \
                    tx = tx->link[1]; \
                tn->value = tx->value; \
                raw = i_keyto(_i_keyref(&tn->value)); \
                tn->link[0] = _cx_memb(_erase_r_)(tn->link[0], &raw, erased); \
            } else { \
                tx = tn; \
                tn = tn->link[tn->link[0]->level == 0]; \
                c_free(tx); \
            } \
        } \
        if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) { \
            if (tn->link[1]->level > --tn->level) \
                tn->link[1]->level = tn->level; \
                          tn = _cx_memb(_skew_)(tn); \
            tx = tn->link[0] = _cx_memb(_skew_)(tn->link[0]); \
                 tx->link[0] = _cx_memb(_skew_)(tx->link[0]); \
                          tn = _cx_memb(_split_)(tn); \
                 tn->link[0] = _cx_memb(_split_)(tn->link[0]); \
        } \
        return tn; \
    } \
    STC_DEF _cx_iter \
    _cx_memb(_erase_at)(_cx_self* self, _cx_iter it) { \
        _cx_rawkey raw = i_keyto(_i_keyref(it.ref)), nxt; \
        _cx_memb(_next)(&it); \
        if (it.ref) nxt = i_keyto(_i_keyref(it.ref)); \
        _cx_memb(_erase)(self, raw); \
        if (it.ref) _cx_memb(_find_it)(self, nxt, &it); \
        return it; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) { \
        if (!it2.ref) { while (it1.ref) it1 = _cx_memb(_erase_at)(self, it1); \
                        return it1; } \
        _cx_key k1 = *_i_keyref(it1.ref), k2 = *_i_keyref(it2.ref); \
        _cx_rawkey r1 = i_keyto((&k1)); \
        for (;;) { \
            if (memcmp(&k1, &k2, sizeof k1) == 0) return it1; \
            _cx_memb(_next)(&it1); k1 = *_i_keyref(it1.ref); \
            _cx_memb(_erase)(self, r1); \
            _cx_memb(_find_it)(self, (r1 = i_keyto((&k1))), &it1); \
        } \
    } \
\
    STC_DEF _cx_node* \
    _cx_memb(_clone_r_)(_cx_node *tn) { \
        if (! tn->level) return tn; \
        _cx_node *cn = c_alloc(_cx_node); \
        cn->link[0] = _cx_memb(_clone_r_)(tn->link[0]); \
        cn->link[1] = _cx_memb(_clone_r_)(tn->link[1]); \
        cn->level = tn->level; \
        _cx_memb(_value_clone)(&cn->value, &tn->value); \
        return cn; \
    } \
\
    STC_DEF void \
    _cx_memb(_del_r_)(_cx_node* tn) { \
        if (tn->level != 0) { \
            _cx_memb(_del_r_)(tn->link[0]); \
            _cx_memb(_del_r_)(tn->link[1]); \
            _cx_memb(_value_drop)(&tn->value); \
            c_free(tn); \
        } \
    }

#endif
#endif
