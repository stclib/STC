/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
#include <stc/cstr.h>

#define T SMap, cstr, double, (c_keypro)  // Sorted map<cstr, double>
#include <stc/sortedmap.h>

int main(void) {
    SMap m = {0};
    SMap_emplace(&m, "Testing one", 1.234);
    SMap_emplace(&m, "Testing two", 12.34);
    SMap_emplace(&m, "Testing three", 123.4);

    SMap_value *v = SMap_get(&m, "Testing five"); // NULL
    double num = *SMap_at(&m, "Testing one");
    SMap_emplace_or_assign(&m, "Testing three", 1000.0); // update
    SMap_erase(&m, "Testing two");

    for (c_each(i, SMap, m))
        printf("map %s: %g\n", cstr_str(&i.ref->first), i.ref->second);

    SMap_drop(&m);
}
*/
#include "priv/linkage.h"
#include "types.h"

#ifndef STC_SMAP_H_INCLUDED
#define STC_SMAP_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#endif // STC_SMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix smap_
#endif
#ifndef _i_is_set
  #define _i_is_map
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
#ifndef i_declared
  _c_DEFTYPES(_declare_aatree, Self, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY, _i_aux_def);
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

#ifndef i_no_emplace
STC_API _m_result       _c_MEMB(_emplace)(Self* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped));
#endif // !i_no_emplace

#ifndef i_no_clone
STC_API Self            _c_MEMB(_clone)(Self tree);
#endif // !i_no_clone

STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API bool            _c_MEMB(_reserve)(Self* self, isize cap);
STC_API _m_value*       _c_MEMB(_find_it)(const Self* self, _m_keyraw rkey, _m_iter* out);
STC_API _m_iter         _c_MEMB(_lower_bound)(const Self* self, _m_keyraw rkey);
STC_API _m_value*       _c_MEMB(_front)(const Self* self);
STC_API _m_value*       _c_MEMB(_back)(const Self* self);
STC_API int             _c_MEMB(_erase)(Self* self, _m_keyraw rkey);
STC_API _m_iter         _c_MEMB(_erase_at)(Self* self, _m_iter it);
STC_API _m_iter         _c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2);
STC_API _m_iter         _c_MEMB(_begin)(const Self* self);
STC_API void            _c_MEMB(_next)(_m_iter* it);

STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self) { return self->size == 0; }
STC_INLINE isize        _c_MEMB(_size)(const Self* self) { return self->size; }
STC_INLINE isize        _c_MEMB(_capacity)(const Self* self) { return self->capacity; }
STC_INLINE _m_iter      _c_MEMB(_find)(const Self* self, _m_keyraw rkey)
                            { _m_iter it; _c_MEMB(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _c_MEMB(_contains)(const Self* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _m_value* _c_MEMB(_get)(const Self* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it); }
STC_INLINE _m_value*    _c_MEMB(_get_mut)(Self* self, _m_keyraw rkey)
                            { _m_iter it; return _c_MEMB(_find_it)(self, rkey, &it); }

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* val) {
    return _i_SET_ONLY( i_keytoraw(val) )
           _i_MAP_ONLY( c_literal(_m_raw){i_keytoraw((&val->first)),
                                          i_valtoraw((&val->second))} );
}

STC_INLINE void _c_MEMB(_value_drop)(const Self* self, _m_value* val) {
    (void)self;
    i_keydrop(_i_keyref(val));
    _i_MAP_ONLY( i_valdrop((&val->second)); )
}

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->capacity = self->size = self->root = self->disp = self->head = 0;
    self->nodes = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_clear)(Self* self) {
    _c_MEMB(_drop)(self);
    (void)_c_MEMB(_move)(self);
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

#ifndef i_no_clone
STC_INLINE _m_value _c_MEMB(_value_clone)(const Self* self, _m_value _val) {
    (void)self;
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}

STC_INLINE void _c_MEMB(_copy)(Self *self, const Self* other) {
    if (self == other)
        return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(Self *self) {
    Self tmp = _c_MEMB(_clone)(*self);
    _c_MEMB(_drop)(self); *self = tmp;
}
#endif // !i_no_clone

STC_API _m_result _c_MEMB(_insert_entry_)(Self* self, _m_keyraw rkey);

#ifdef _i_is_map
    STC_API _m_result _c_MEMB(_insert_or_assign)(Self* self, _m_key key, _m_mapped mapped);
    #ifndef i_no_emplace
    STC_API _m_result _c_MEMB(_emplace_or_assign)(Self* self, _m_keyraw rkey, _m_rmapped rmapped);
    #endif

    STC_INLINE const _m_mapped* _c_MEMB(_at)(const Self* self, _m_keyraw rkey)
        { _m_iter it; return &_c_MEMB(_find_it)(self, rkey, &it)->second; }

    STC_INLINE _m_mapped* _c_MEMB(_at_mut)(Self* self, _m_keyraw rkey)
        { _m_iter it; return &_c_MEMB(_find_it)(self, rkey, &it)->second; }
#endif // _i_is_map

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self) {
    _m_iter it; (void)self;
    it.ref = NULL, it._top = 0, it._tn = 0;
    return it;
}

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref)
        _c_MEMB(_next)(&it);
    return it;
}

#if defined _i_has_eq
STC_INLINE bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    _m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
    for (; i.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j)) {
        const _m_keyraw _rx = i_keytoraw(_i_keyref(i.ref)), _ry = i_keytoraw(_i_keyref(j.ref));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif

STC_INLINE _m_result
_c_MEMB(_insert)(Self* self, _m_key _key _i_MAP_ONLY(, _m_mapped _mapped)) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _m_value* _c_MEMB(_push)(Self* self, _m_value _val) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _c_MEMB(_value_drop)(self, &_val);
    return _res.ref;
}

#if defined _i_is_map && !defined _i_no_put
STC_INLINE _m_result _c_MEMB(_put)(Self* self, _m_keyraw rkey, _m_rmapped rmapped) {
    #ifdef i_no_emplace
        return _c_MEMB(_insert_or_assign)(self, rkey, rmapped);
    #else
        return _c_MEMB(_emplace_or_assign)(self, rkey, rmapped);
    #endif
}
#endif

#ifndef _i_no_put
STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n) {
    while (n--)
        #if defined _i_is_set && defined i_no_emplace
            _c_MEMB(_insert)(self, *raw++);
        #elif defined _i_is_set
            _c_MEMB(_emplace)(self, *raw++);
        #else
            _c_MEMB(_put)(self, raw->first, raw->second), ++raw;
        #endif
}
#endif

#ifndef _i_aux_alloc
STC_INLINE Self _c_MEMB(_init)(void)
    { Self cx = {0}; return cx; }

#ifndef _i_no_put
STC_INLINE Self _c_MEMB(_from_n)(const _m_raw* raw, isize n)
    { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
#endif

STC_INLINE Self _c_MEMB(_with_capacity)(const isize cap)
    { Self cx = {0}; _c_MEMB(_reserve)(&cx, cap); return cx; }
#endif

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

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
_c_MEMB(_begin)(const Self* self) {
    _m_iter it;
    it.ref = NULL;
    it._d = self->nodes, it._top = 0;
    it._tn = self->root;
    if (it._tn)
        _c_MEMB(_next)(&it);
    return it;
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize cap) {
    if (cap <= self->capacity)
        return false;
    _m_node* nodes = (_m_node*)_i_realloc_n(self->nodes, self->capacity + 1, cap + 1);
    if (nodes == NULL)
        return false;
    nodes[0] = c_literal(_m_node){0};
    self->nodes = nodes;
    self->capacity = (int32_t)cap;
    return true;
}

STC_DEF _m_value*
_c_MEMB(_front)(const Self* self) {
    _m_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[0])
        tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _m_value*
_c_MEMB(_back)(const Self* self) {
    _m_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[1])
        tn = d[tn].link[1];
    return &d[tn].value;
}

static int32_t
_c_MEMB(_new_node_)(Self* self, int level) {
    int32_t tn;
    if (self->disp != 0) {
        tn = self->disp;
        self->disp = self->nodes[tn].link[1];
    } else {
        if (self->head == self->capacity)
            if (!_c_MEMB(_reserve)(self, self->head*3/2 + 4))
                return 0;
        tn = ++self->head; /* start with 1, 0 is nullnode. */
    }
    _m_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = (int8_t)level;
    return tn;
}

#ifdef _i_is_map
    STC_DEF _m_result
    _c_MEMB(_insert_or_assign)(Self* self, _m_key _key, _m_mapped _mapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw((&_key)));
        _m_mapped* _mp = _res.ref ? &_res.ref->second : &_mapped;
        if (_res.inserted)
            _res.ref->first = _key;
        else
            { i_keydrop((&_key)); i_valdrop(_mp); }
        *_mp = _mapped;
        return _res;
    }

    #ifndef i_no_emplace
    STC_DEF _m_result
    _c_MEMB(_emplace_or_assign)(Self* self, _m_keyraw rkey, _m_rmapped rmapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted)
            _res.ref->first = i_keyfrom(rkey);
        else {
            if (_res.ref == NULL) return _res;
            i_valdrop((&_res.ref->second));
        }
        _res.ref->second = i_valfrom(rmapped);
        return _res;
    }
    #endif // !i_no_emplace
#endif // !_i_is_map

STC_DEF _m_value*
_c_MEMB(_find_it)(const Self* self, _m_keyraw rkey, _m_iter* out) {
    int32_t tn = self->root;
    _m_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; const _m_keyraw _raw = i_keytoraw(_i_keyref(&d[tn].value));
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
_c_MEMB(_lower_bound)(const Self* self, _m_keyraw rkey) {
    _m_iter it;
    _c_MEMB(_find_it)(self, rkey, &it);
    if (it.ref == NULL && it._top != 0) {
        int32_t tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF int32_t
_c_MEMB(_skew_)(_m_node *d, int32_t tn) {
    if (tn != 0 && d[d[tn].link[0]].level == d[tn].level) {
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
_c_MEMB(_insert_entry_i_)(Self* self, int32_t tn, const _m_keyraw* rkey, _m_result* _res) {
    int32_t up[64], tx = tn;
    _m_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        const _m_keyraw _raw = i_keytoraw(_i_keyref(&d[tx].value));
        if ((c = i_cmp((&_raw), rkey)) == 0)
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
        if (top != 0)
            dir = (d[up[top - 1]].link[1] == up[top]);
        up[top] = _c_MEMB(_skew_)(d, up[top]);
        up[top] = _c_MEMB(_split_)(d, up[top]);
        if (top)
            d[up[top - 1]].link[dir] = up[top];
    }
    return up[0];
}

STC_DEF _m_result
_c_MEMB(_insert_entry_)(Self* self, _m_keyraw rkey) {
    _m_result res = {0};
    int32_t tn = _c_MEMB(_insert_entry_i_)(self, self->root, &rkey, &res);
    self->root = tn;
    self->size += res.inserted;
    return res;
}

STC_DEF int32_t
_c_MEMB(_erase_r_)(Self *self, int32_t tn, const _m_keyraw* rkey, int *erased) {
    _m_node *d = self->nodes;
    if (tn == 0)
        return 0;
    _m_keyraw raw = i_keytoraw(_i_keyref(&d[tn].value));
    int32_t tx; int c = i_cmp((&raw), rkey);
    if (c != 0)
        d[tn].link[c < 0] = _c_MEMB(_erase_r_)(self, d[tn].link[c < 0], rkey, erased);
    else {
        if ((*erased)++ == 0)
            _c_MEMB(_value_drop)(self, &d[tn].value); // drop first time, not second.
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keytoraw(_i_keyref(&d[tn].value));
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
_c_MEMB(_erase)(Self* self, _m_keyraw rkey) {
    int erased = 0;
    int32_t root = _c_MEMB(_erase_r_)(self, self->root, &rkey, &erased);
    if (erased == 0)
        return 0;
    self->root = root;
    --self->size;
    return 1;
}

STC_DEF _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _m_keyraw raw = i_keytoraw(_i_keyref(it.ref));
    _c_MEMB(_next)(&it);
    if (it.ref != NULL) {
        _m_keyraw nxt = i_keytoraw(_i_keyref(it.ref));
        _c_MEMB(_erase)(self, raw);
        _c_MEMB(_find_it)(self, nxt, &it);
    } else
        _c_MEMB(_erase)(self, raw);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2) {
    if (it2.ref == NULL) {
        while (it1.ref != NULL)
            it1 = _c_MEMB(_erase_at)(self, it1);
        return it1;
    }
    _m_key k1 = *_i_keyref(it1.ref), k2 = *_i_keyref(it2.ref);
    _m_keyraw r1 = i_keytoraw((&k1));
    for (;;) {
        if (memcmp(&k1, &k2, sizeof k1) == 0)
            return it1;
        _c_MEMB(_next)(&it1);
        k1 = *_i_keyref(it1.ref);
        _c_MEMB(_erase)(self, r1);
        r1 = i_keytoraw((&k1));
        _c_MEMB(_find_it)(self, r1, &it1);
    }
}

#ifndef i_no_clone
STC_DEF int32_t
_c_MEMB(_clone_r_)(Self* self, _m_node* src, int32_t sn) {
    if (sn == 0)
        return 0;
    int32_t tx, tn = _c_MEMB(_new_node_)(self, src[sn].level);
    self->nodes[tn].value = _c_MEMB(_value_clone)(self, src[sn].value);
    tx = _c_MEMB(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _c_MEMB(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF Self
_c_MEMB(_clone)(Self tree) {
    Self out = tree;
    out.root = out.disp = out.head = out.size = out.capacity = 0;
    out.nodes = NULL; _c_MEMB(_reserve)(&out, tree.size);
    out.root = _c_MEMB(_clone_r_)(&out, tree.nodes, tree.root);
    return out;
}
#endif // !i_no_clone

#ifndef i_no_emplace
STC_DEF _m_result
_c_MEMB(_emplace)(Self* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped)) {
    _m_result res = _c_MEMB(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *_i_keyref(res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}
#endif // i_no_emplace

static void
_c_MEMB(_drop_r_)(Self* s, int32_t tn) {
    if (tn != 0) {
        _c_MEMB(_drop_r_)(s, s->nodes[tn].link[0]);
        _c_MEMB(_drop_r_)(s, s->nodes[tn].link[1]);
        _c_MEMB(_value_drop)(s, &s->nodes[tn].value);
    }
}

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    if (self->capacity != 0) {
        _c_MEMB(_drop_r_)(self, self->root);
        _i_free_n(self->nodes, self->capacity + 1);
    }
}

#endif // i_implement
#undef _i_is_set
#undef _i_is_map
#undef _i_sorted
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#include "sys/finalize.h"
