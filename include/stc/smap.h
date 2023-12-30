/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#define i_implement
#include "stc/cstr.h"

#define i_type smap_sd  // Sorted map<cstr, double>
#define i_key_str
#define i_val double
#include "stc/smap.h"

int main(void) {
    smap_sd m = {0};
    smap_sd_emplace(&m, "Testing one", 1.234);
    smap_sd_emplace(&m, "Testing two", 12.34);
    smap_sd_emplace(&m, "Testing three", 123.4);

    smap_sd_value *v = smap_sd_get(&m, "Testing five"); // NULL
    double num = *smap_sd_at(&m, "Testing one");
    smap_sd_emplace_or_assign(&m, "Testing three", 1000.0); // update
    smap_sd_erase(&m, "Testing two");

    c_foreach (i, smap_sd, m)
        printf("map %s: %g\n", cstr_str(&i.ref->first), i.ref->second);

    smap_sd_drop(&m);
}
*/
#include "priv/linkage.h"

#ifndef STC_SMAP_H_INCLUDED
#define STC_SMAP_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>
#endif // STC_SMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix smap_
#endif
#ifndef _i_isset
  #define _i_ismap
  #define _i_MAP_ONLY c_true
  #define _i_SET_ONLY c_false
  #define _i_keyref(vp) (&(vp)->first)
#else
  #define _i_MAP_ONLY c_false
  #define _i_SET_ONLY c_true
  #define _i_keyref(vp) (vp)
#endif
#define _i_sorted
#include "priv/template.h"
#ifndef i_is_forward
  _c_DEFTYPES(_c_aatree_types, i_type, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _m_value {
    _m_key first;
    _m_mapped second;
}; )
struct _m_node {
    int32_t link[2];
    int8_t level;
    _m_value value;
};

typedef i_keyraw _m_keyraw;
typedef i_valraw _m_rmapped;
typedef _i_SET_ONLY( _m_keyraw )
        _i_MAP_ONLY( struct { _m_keyraw first; _m_rmapped second; } )
        _m_raw;

#if !defined i_no_emplace
STC_API _m_result       _c_MEMB(_emplace)(i_type* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped));
#endif // !i_no_emplace
#if !defined i_no_clone
STC_API i_type          _c_MEMB(_clone)(i_type tree);
#endif // !i_no_clone
STC_API void            _c_MEMB(_drop)(const i_type* cself);
STC_API bool            _c_MEMB(_reserve)(i_type* self, intptr_t cap);
STC_API _m_value*       _c_MEMB(_find_it)(const i_type* self, _m_keyraw rkey, _m_iter* out);
STC_API _m_iter         _c_MEMB(_lower_bound)(const i_type* self, _m_keyraw rkey);
STC_API _m_value*       _c_MEMB(_front)(const i_type* self);
STC_API _m_value*       _c_MEMB(_back)(const i_type* self);
STC_API int             _c_MEMB(_erase)(i_type* self, _m_keyraw rkey);
STC_API _m_iter         _c_MEMB(_erase_at)(i_type* self, _m_iter it);
STC_API _m_iter         _c_MEMB(_erase_range)(i_type* self, _m_iter it1, _m_iter it2);
STC_API _m_iter         _c_MEMB(_begin)(const i_type* self);
STC_API void            _c_MEMB(_next)(_m_iter* it);

STC_INLINE i_type       _c_MEMB(_init)(void) { i_type tree = {0}; return tree; }
STC_INLINE bool         _c_MEMB(_empty)(const i_type* cx) { return cx->size == 0; }
STC_INLINE intptr_t     _c_MEMB(_size)(const i_type* cx) { return cx->size; }
STC_INLINE intptr_t     _c_MEMB(_capacity)(const i_type* cx) { return cx->cap; }
STC_INLINE _m_iter      _c_MEMB(_find)(const i_type* self, _m_keyraw rkey)
                            { _m_iter it; _c_MEMB(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _c_MEMB(_contains)(const i_type* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _m_value* _c_MEMB(_get)(const i_type* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it); }
STC_INLINE _m_value*    _c_MEMB(_get_mut)(i_type* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it); }

STC_INLINE i_type
_c_MEMB(_with_capacity)(const intptr_t cap) {
    i_type tree = _c_MEMB(_init)();
    _c_MEMB(_reserve)(&tree, cap);
    return tree;
}

STC_INLINE void
_c_MEMB(_clear)(i_type* self)
    { _c_MEMB(_drop)(self); *self = _c_MEMB(_init)(); }

STC_INLINE _m_raw
_c_MEMB(_value_toraw)(const _m_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_LITERAL(_m_raw){i_keyto((&val->first)),
                                        i_valto((&val->second))} );
}

STC_INLINE void
_c_MEMB(_value_drop)(_m_value* val) {
    i_keydrop(_i_keyref(val));
    _i_MAP_ONLY( i_valdrop((&val->second)); )
}

#if !defined i_no_clone
STC_INLINE _m_value
_c_MEMB(_value_clone)(_m_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}

STC_INLINE void
_c_MEMB(_copy)(i_type *self, const i_type* other) {
    if (self->nodes == other->nodes)
        return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}

STC_INLINE void
_c_MEMB(_shrink_to_fit)(i_type *self) {
    i_type tmp = _c_MEMB(_clone)(*self);
    _c_MEMB(_drop)(self); *self = tmp;
}
#endif // !i_no_clone

STC_API _m_result _c_MEMB(_insert_entry_)(i_type* self, _m_keyraw rkey);

#ifdef _i_ismap
    STC_API _m_result _c_MEMB(_insert_or_assign)(i_type* self, _m_key key, _m_mapped mapped);
    #if !defined i_no_emplace
        STC_API _m_result  _c_MEMB(_emplace_or_assign)(i_type* self, _m_keyraw rkey, _m_rmapped rmapped);

        STC_INLINE _m_result
        _c_MEMB(_emplace_key)(i_type* self, _m_keyraw rkey) {
            _m_result res = _c_MEMB(_insert_entry_)(self, rkey);
            if (res.inserted)
                res.ref->first = i_keyfrom(rkey);
            return res;
        }
    #endif
    STC_INLINE const _m_mapped*
    _c_MEMB(_at)(const i_type* self, _m_keyraw rkey)
        { _m_iter it; return &_c_MEMB(_find_it)(self, rkey, &it)->second; }

    STC_INLINE _m_mapped*
    _c_MEMB(_at_mut)(i_type* self, _m_keyraw rkey)
        { _m_iter it; return &_c_MEMB(_find_it)(self, rkey, &it)->second; }
#endif // _i_ismap

STC_INLINE _m_iter
_c_MEMB(_end)(const i_type* self) {
    _m_iter it; (void)self;
    it.ref = NULL, it._top = 0, it._tn = 0;
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref)
        _c_MEMB(_next)(&it);
    return it;
}

#if defined _i_has_eq
STC_INLINE bool
_c_MEMB(_eq)(const i_type* self, const i_type* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    _m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
    for (; i.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j)) {
        const _m_keyraw _rx = i_keyto(_i_keyref(i.ref)), _ry = i_keyto(_i_keyref(j.ref));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif

STC_INLINE _m_result
_c_MEMB(_insert)(i_type* self, _m_key _key _i_MAP_ONLY(, _m_mapped _mapped)) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keyto((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _m_value*
_c_MEMB(_push)(i_type* self, _m_value _val) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keyto(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _c_MEMB(_value_drop)(&_val);
    return _res.ref;
}

STC_INLINE void
_c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n) {
    while (n--)
#if defined _i_isset && defined i_no_emplace
        _c_MEMB(_insert)(self, *raw++);
#elif defined _i_isset
        _c_MEMB(_emplace)(self, *raw++);
#elif defined i_no_emplace
        _c_MEMB(_insert_or_assign)(self, raw->first, raw->second), ++raw;
#else
        _c_MEMB(_emplace_or_assign)(self, raw->first, raw->second), ++raw;
#endif
}

STC_INLINE i_type
_c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
    { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void
_c_MEMB(_next)(_m_iter *it) {
    int32_t tn = it->_tn;
    if (it->_top || tn) {
        while (tn) {
            it->_st[it->_top++] = tn;
            tn = it->_d[tn].link[0];
        }
        tn = it->_st[--it->_top];
        it->_tn = it->_d[tn].link[1];
        it->ref = &it->_d[tn].value;
    } else
        it->ref = NULL;
}

STC_DEF _m_iter
_c_MEMB(_begin)(const i_type* self) {
    _m_iter it;
    it.ref = NULL;
    it._d = self->nodes, it._top = 0;
    it._tn = self->root;
    if (it._tn)
        _c_MEMB(_next)(&it);
    return it;
}

STC_DEF bool
_c_MEMB(_reserve)(i_type* self, const intptr_t cap) {
    if (cap <= self->cap)
        return false;
    _m_node* nodes = (_m_node*)i_realloc(self->nodes, (self->cap + 1)*c_sizeof(_m_node),
                                                      (cap + 1)*c_sizeof(_m_node));
    if (!nodes)
        return false;
    nodes[0] = c_LITERAL(_m_node){0};
    self->nodes = nodes;
    self->cap = (int32_t)cap;
    return true;
}

STC_DEF _m_value*
_c_MEMB(_front)(const i_type* self) {
    _m_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[0])
        tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _m_value*
_c_MEMB(_back)(const i_type* self) {
    _m_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[1])
        tn = d[tn].link[1];
    return &d[tn].value;
}

static int32_t
_c_MEMB(_new_node_)(i_type* self, int level) {
    int32_t tn;
    if (self->disp) {
        tn = self->disp;
        self->disp = self->nodes[tn].link[1];
    } else {
        if (self->head == self->cap)
            if (!_c_MEMB(_reserve)(self, self->head*3/2 + 4))
                return 0;
        tn = ++self->head; /* start with 1, 0 is nullnode. */
    }
    _m_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = (int8_t)level;
    return tn;
}

#ifdef _i_ismap
    STC_DEF _m_result
    _c_MEMB(_insert_or_assign)(i_type* self, _m_key _key, _m_mapped _mapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, i_keyto((&_key)));
        _m_mapped* _mp = _res.ref ? &_res.ref->second : &_mapped;
        if (_res.inserted)
            _res.ref->first = _key;
        else
            { i_keydrop((&_key)); i_valdrop(_mp); }
        *_mp = _mapped;
        return _res;
    }

    #if !defined i_no_emplace
    STC_DEF _m_result
    _c_MEMB(_emplace_or_assign)(i_type* self, _m_keyraw rkey, _m_rmapped rmapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted)
            _res.ref->first = i_keyfrom(rkey);
        else {
            if (!_res.ref) return _res;
            i_valdrop((&_res.ref->second));
        }
        _res.ref->second = i_valfrom(rmapped);
        return _res;
    }
    #endif // !i_no_emplace
#endif // !_i_ismap

STC_DEF _m_value*
_c_MEMB(_find_it)(const i_type* self, _m_keyraw rkey, _m_iter* out) {
    int32_t tn = self->root;
    _m_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; const _m_keyraw _raw = i_keyto(_i_keyref(&d[tn].value));
        if ((c = i_cmp((&_raw), (&rkey))) < 0)
            tn = d[tn].link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = d[tn].link[0]; }
        else
            { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); }
    }
    return (out->ref = NULL);
}

STC_DEF _m_iter
_c_MEMB(_lower_bound)(const i_type* self, _m_keyraw rkey) {
    _m_iter it;
    _c_MEMB(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        int32_t tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF int32_t
_c_MEMB(_skew_)(_m_node *d, int32_t tn) {
    if (tn && d[d[tn].link[0]].level == d[tn].level) {
        int32_t tmp = d[tn].link[0];
        d[tn].link[0] = d[tmp].link[1];
        d[tmp].link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF int32_t
_c_MEMB(_split_)(_m_node *d, int32_t tn) {
    if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) {
        int32_t tmp = d[tn].link[1];
        d[tn].link[1] = d[tmp].link[0];
        d[tmp].link[0] = tn;
        tn = tmp;
        ++d[tn].level;
    }
    return tn;
}

STC_DEF int32_t
_c_MEMB(_insert_entry_i_)(i_type* self, int32_t tn, const _m_keyraw* rkey, _m_result* _res) {
    int32_t up[64], tx = tn;
    _m_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        const _m_keyraw _raw = i_keyto(_i_keyref(&d[tx].value));
        if (!(c = i_cmp((&_raw), rkey)))
            { _res->ref = &d[tx].value; return tn; }
        dir = (c < 0);
        tx = d[tx].link[dir];
    }
    if ((tx = _c_MEMB(_new_node_)(self, 1)) == 0)
        return 0;
    d = self->nodes;
    _res->ref = &d[tx].value;
    _res->inserted = true;
    if (top == 0)
        return tx;
    d[up[top - 1]].link[dir] = tx;
    while (top--) {
        if (top)
            dir = (d[up[top - 1]].link[1] == up[top]);
        up[top] = _c_MEMB(_skew_)(d, up[top]);
        up[top] = _c_MEMB(_split_)(d, up[top]);
        if (top)
            d[up[top - 1]].link[dir] = up[top];
    }
    return up[0];
}

STC_DEF _m_result
_c_MEMB(_insert_entry_)(i_type* self, _m_keyraw rkey) {
    _m_result res = {NULL};
    int32_t tn = _c_MEMB(_insert_entry_i_)(self, self->root, &rkey, &res);
    self->root = tn;
    self->size += res.inserted;
    return res;
}

STC_DEF int32_t
_c_MEMB(_erase_r_)(i_type *self, int32_t tn, const _m_keyraw* rkey, int *erased) {
    _m_node *d = self->nodes;
    if (tn == 0)
        return 0;
    _m_keyraw raw = i_keyto(_i_keyref(&d[tn].value));
    int32_t tx; int c = i_cmp((&raw), rkey);
    if (c != 0)
        d[tn].link[c < 0] = _c_MEMB(_erase_r_)(self, d[tn].link[c < 0], rkey, erased);
    else {
        if (!(*erased)++)
            _c_MEMB(_value_drop)(&d[tn].value);
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keyto(_i_keyref(&d[tn].value));
            d[tn].link[0] = _c_MEMB(_erase_r_)(self, d[tn].link[0], &raw, erased);
        } else { /* unlink node */
            tx = tn;
            tn = d[tn].link[ d[tn].link[0] == 0 ];
            /* move it to disposed nodes list */
            d[tx].link[1] = self->disp;
            self->disp = tx;
        }
    }
    tx = d[tn].link[1];
    if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) {
        if (d[tx].level > --d[tn].level)
            d[tx].level = d[tn].level;
                       tn = _c_MEMB(_skew_)(d, tn);
       tx = d[tn].link[1] = _c_MEMB(_skew_)(d, d[tn].link[1]);
            d[tx].link[1] = _c_MEMB(_skew_)(d, d[tx].link[1]);
                       tn = _c_MEMB(_split_)(d, tn);
            d[tn].link[1] = _c_MEMB(_split_)(d, d[tn].link[1]);
    }
    return tn;
}

STC_DEF int
_c_MEMB(_erase)(i_type* self, _m_keyraw rkey) {
    int erased = 0;
    int32_t root = _c_MEMB(_erase_r_)(self, self->root, &rkey, &erased);
    if (!erased)
        return 0;
    self->root = root;
    --self->size;
    return 1;
}

STC_DEF _m_iter
_c_MEMB(_erase_at)(i_type* self, _m_iter it) {
    _m_keyraw raw = i_keyto(_i_keyref(it.ref));
    _c_MEMB(_next)(&it);
    if (it.ref) {
        _m_keyraw nxt = i_keyto(_i_keyref(it.ref));
        _c_MEMB(_erase)(self, raw);
        _c_MEMB(_find_it)(self, nxt, &it);
    } else
        _c_MEMB(_erase)(self, raw);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_range)(i_type* self, _m_iter it1, _m_iter it2) {
    if (!it2.ref) {
        while (it1.ref)
            it1 = _c_MEMB(_erase_at)(self, it1);
        return it1;
    }
    _m_key k1 = *_i_keyref(it1.ref), k2 = *_i_keyref(it2.ref);
    _m_keyraw r1 = i_keyto((&k1));
    for (;;) {
        if (memcmp(&k1, &k2, sizeof k1) == 0)
            return it1;
        _c_MEMB(_next)(&it1);
        k1 = *_i_keyref(it1.ref);
        _c_MEMB(_erase)(self, r1);
        r1 = i_keyto((&k1));
        _c_MEMB(_find_it)(self, r1, &it1);
    }
}

#if !defined i_no_clone
STC_DEF int32_t
_c_MEMB(_clone_r_)(i_type* self, _m_node* src, int32_t sn) {
    if (sn == 0)
        return 0;
    int32_t tx, tn = _c_MEMB(_new_node_)(self, src[sn].level);
    self->nodes[tn].value = _c_MEMB(_value_clone)(src[sn].value);
    tx = _c_MEMB(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _c_MEMB(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF i_type
_c_MEMB(_clone)(i_type tree) {
    i_type clone = _c_MEMB(_with_capacity)(tree.size);
    int32_t root = _c_MEMB(_clone_r_)(&clone, tree.nodes, tree.root);
    clone.root = root;
    clone.size = tree.size;
    return clone;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _m_result
_c_MEMB(_emplace)(i_type* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped)) {
    _m_result res = _c_MEMB(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *_i_keyref(res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}
#endif // i_no_emplace

static void
_c_MEMB(_drop_r_)(_m_node* d, int32_t tn) {
    if (tn) {
        _c_MEMB(_drop_r_)(d, d[tn].link[0]);
        _c_MEMB(_drop_r_)(d, d[tn].link[1]);
        _c_MEMB(_value_drop)(&d[tn].value);
    }
}

STC_DEF void
_c_MEMB(_drop)(const i_type* cself) {
    i_type* self = (i_type*)cself;
    if (self->cap) {
        _c_MEMB(_drop_r_)(self->nodes, self->root);
        i_free(self->nodes, (self->cap + 1)*c_sizeof(_m_node));
    }
}

#endif // i_implement
#undef _i_isset
#undef _i_ismap
#undef _i_sorted
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#include "priv/template2.h"
#include "priv/linkage2.h"
