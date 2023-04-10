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
#include <stc/cstr.h>

#define i_tag sx  // Sorted map<cstr, double>
#define i_key_str
#define i_val double
#include <stc/csmap.h>

int main(void) {
    csmap_sx m = {0};
    csmap_sx_emplace(&m, "Testing one", 1.234);
    csmap_sx_emplace(&m, "Testing two", 12.34);
    csmap_sx_emplace(&m, "Testing three", 123.4);

    csmap_sx_value *v = csmap_sx_get(&m, "Testing five"); // NULL
    double num = *csmap_sx_at(&m, "Testing one");
    csmap_sx_emplace_or_assign(&m, "Testing three", 1000.0); // update
    csmap_sx_erase(&m, "Testing two");

    c_foreach (i, csmap_sx, m)
        printf("map %s: %g\n", cstr_str(&i.ref->first), i.ref->second);

    csmap_sx_drop(&m);
}
*/
#include "ccommon.h"

#ifndef CSMAP_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#endif // CSMAP_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix csmap_
#endif
#ifdef _i_isset
  #define _i_MAP_ONLY c_false
  #define _i_SET_ONLY c_true
  #define _i_keyref(vp) (vp)
#else
  #define _i_ismap
  #define _i_MAP_ONLY c_true
  #define _i_SET_ONLY c_false
  #define _i_keyref(vp) (&(vp)->first)
#endif
#ifndef i_ssize
  #define i_ssize int32_t
  #define _i_size intptr_t
#else
  #define _i_size i_ssize
#endif
#include "priv/template.h"
#ifndef i_is_forward
  _cx_deftypes(_c_aatree_types, _cx_self, i_key, i_val, i_ssize, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )
struct _cx_node {
    i_ssize link[2];
    int8_t level;
    _cx_value value;
};

typedef i_keyraw _cx_keyraw;
typedef i_valraw _cx_memb(_rmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first; i_valraw second; } )
        _cx_raw;

#if !defined i_no_emplace
STC_API _cx_result      _cx_memb(_emplace)(_cx_self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped));
#endif // !i_no_emplace
#if !defined i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self tree);
#endif // !i_no_clone
STC_API _cx_result      _cx_memb(_insert)(_cx_self* self, i_key key _i_MAP_ONLY(, i_val mapped));
STC_API _cx_result      _cx_memb(_push)(_cx_self* self, _cx_value _val);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, _i_size cap);
STC_API _cx_value*      _cx_memb(_find_it)(const _cx_self* self, _cx_keyraw rkey, _cx_iter* out);
STC_API _cx_iter        _cx_memb(_lower_bound)(const _cx_self* self, _cx_keyraw rkey);
STC_API _cx_value*      _cx_memb(_front)(const _cx_self* self);
STC_API _cx_value*      _cx_memb(_back)(const _cx_self* self);
STC_API int             _cx_memb(_erase)(_cx_self* self, _cx_keyraw rkey);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API void            _cx_memb(_next)(_cx_iter* it);

STC_INLINE _cx_self     _cx_memb(_init)(void) { _cx_self tree = {0}; return tree; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* cx) { return cx->size == 0; }
STC_INLINE _i_size      _cx_memb(_size)(const _cx_self* cx) { return cx->size; }
STC_INLINE _i_size      _cx_memb(_capacity)(const _cx_self* cx) { return cx->cap; }
STC_INLINE _cx_iter     _cx_memb(_find)(const _cx_self* self, _cx_keyraw rkey)
                            { _cx_iter it; _cx_memb(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _cx_value* _cx_memb(_get)(const _cx_self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }
STC_INLINE _cx_value*   _cx_memb(_get_mut)(_cx_self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }

STC_INLINE _cx_self
_cx_memb(_with_capacity)(const _i_size cap) {
    _cx_self tree = _cx_memb(_init)();
    _cx_memb(_reserve)(&tree, cap);
    return tree;
}

STC_INLINE void
_cx_memb(_clear)(_cx_self* self) 
    { _cx_memb(_drop)(self); *self = _cx_memb(_init)(); }

STC_INLINE _cx_raw
_cx_memb(_value_toraw)(const _cx_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_LITERAL(_cx_raw){i_keyto((&val->first)), 
                                        i_valto((&val->second))} );
}

STC_INLINE void
_cx_memb(_value_drop)(_cx_value* val) {
    i_keydrop(_i_keyref(val));
    _i_MAP_ONLY( i_valdrop((&val->second)); )
}

#if !defined i_no_clone
STC_INLINE _cx_value
_cx_memb(_value_clone)(_cx_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}

STC_INLINE void
_cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
    if (self->nodes == other->nodes)
        return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(*other);
}

STC_INLINE void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    _cx_self tmp = _cx_memb(_clone)(*self);
    _cx_memb(_drop)(self); *self = tmp;
}
#endif // !i_no_clone

#ifndef _i_isset
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped);
    #if !defined i_no_emplace
        STC_API _cx_result  _cx_memb(_emplace_or_assign)(_cx_self* self, _cx_keyraw rkey, i_valraw rmapped);
    #endif

    STC_INLINE const _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, _cx_keyraw rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
    STC_INLINE _cx_mapped*
    _cx_memb(_at_mut)(_cx_self* self, _cx_keyraw rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
#endif // !_i_isset

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it;
    it.ref = NULL;
    it._d = self->nodes, it._top = 0;
    it._tn = self->root;
    if (it._tn)
        _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self) {
    (void)self;
    _cx_iter it; it.ref = NULL, it._top = 0, it._tn = 0;
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref)
        _cx_memb(_next)(&it);
    return it;
}

STC_INLINE bool
_cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    if (_cx_memb(_size)(self) != _cx_memb(_size)(other)) return false;
    _cx_iter i = _cx_memb(_begin)(self), j = _cx_memb(_begin)(other);
    for (; i.ref; _cx_memb(_next)(&i), _cx_memb(_next)(&j)) {
        const _cx_keyraw _rx = i_keyto(_i_keyref(i.ref)), _ry = i_keyto(_i_keyref(j.ref));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}

STC_INLINE void _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, _i_size n) {
    while (n--) 
#if defined _i_isset && defined i_no_emplace
        _cx_memb(_insert)(self, *raw++);
#elif defined _i_isset
        _cx_memb(_emplace)(self, *raw++);
#elif defined i_no_emplace
        _cx_memb(_insert_or_assign)(self, raw->first, raw->second), ++raw;
#else
        _cx_memb(_emplace_or_assign)(self, raw->first, raw->second), ++raw;
#endif
}

STC_INLINE _cx_self _cx_memb(_from_n)(const _cx_raw* raw, _i_size n)
    { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const _i_size cap) {
    if (cap <= self->cap)
        return false;
    _cx_node* nodes = (_cx_node*)i_realloc(self->nodes, (cap + 1)*c_sizeof(_cx_node));
    if (!nodes)
        return false;
    nodes[0] = c_LITERAL(_cx_node){{0, 0}, 0};
    self->nodes = nodes;
    self->cap = (i_ssize)cap;
    return true;
}

STC_DEF _cx_value*
_cx_memb(_front)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    i_ssize tn = self->root;
    while (d[tn].link[0])
        tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _cx_value*
_cx_memb(_back)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    i_ssize tn = self->root;
    while (d[tn].link[1])
        tn = d[tn].link[1];
    return &d[tn].value;
}

static i_ssize
_cx_memb(_new_node_)(_cx_self* self, int level) {
    i_ssize tn;
    if (self->disp) {
        tn = self->disp;
        self->disp = self->nodes[tn].link[1];
    } else {
        if (self->head == self->cap)
            if (!_cx_memb(_reserve)(self, self->head*3/2 + 4))
                return 0;
        tn = ++self->head; /* start with 1, 0 is nullnode. */
    }
    _cx_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = (int8_t)level;
    return tn;
}

static _cx_result _cx_memb(_insert_entry_)(_cx_self* self, _cx_keyraw rkey);

STC_DEF _cx_result
_cx_memb(_insert)(_cx_self* self, i_key _key _i_MAP_ONLY(, i_val _mapped)) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else 
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_DEF _cx_result
_cx_memb(_push)(_cx_self* self, _cx_value _val) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val; 
    else
        _cx_memb(_value_drop)(&_val);
    return _res;
}

#ifndef _i_isset
    STC_DEF _cx_result
    _cx_memb(_insert_or_assign)(_cx_self* self, i_key _key, i_val _mapped) {
        _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto((&_key)));
        _cx_mapped* _mp = _res.ref ? &_res.ref->second : &_mapped;
        if (_res.inserted)
            _res.ref->first = _key;
        else
            { i_keydrop((&_key)); i_valdrop(_mp); }
        *_mp = _mapped;
        return _res;
    }

    #if !defined i_no_emplace
    STC_DEF _cx_result
    _cx_memb(_emplace_or_assign)(_cx_self* self, _cx_keyraw rkey, i_valraw rmapped) {
        _cx_result _res = _cx_memb(_insert_entry_)(self, rkey);
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
#endif // !_i_isset

STC_DEF _cx_value*
_cx_memb(_find_it)(const _cx_self* self, _cx_keyraw rkey, _cx_iter* out) {
    i_ssize tn = self->root;
    _cx_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; const _cx_keyraw _raw = i_keyto(_i_keyref(&d[tn].value));
        if ((c = i_cmp((&_raw), (&rkey))) < 0)
            tn = d[tn].link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = d[tn].link[0]; }
        else
            { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); }
    }
    return (out->ref = NULL);
}

STC_DEF _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, _cx_keyraw rkey) {
    _cx_iter it;
    _cx_memb(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        i_ssize tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF void
_cx_memb(_next)(_cx_iter *it) {
    i_ssize tn = it->_tn;
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

STC_DEF i_ssize
_cx_memb(_skew_)(_cx_node *d, i_ssize tn) {
    if (tn && d[d[tn].link[0]].level == d[tn].level) {
        i_ssize tmp = d[tn].link[0];
        d[tn].link[0] = d[tmp].link[1];
        d[tmp].link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF i_ssize
_cx_memb(_split_)(_cx_node *d, i_ssize tn) {
    if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) {
        i_ssize tmp = d[tn].link[1];
        d[tn].link[1] = d[tmp].link[0];
        d[tmp].link[0] = tn;
        tn = tmp;
        ++d[tn].level;
    }
    return tn;
}

static i_ssize
_cx_memb(_insert_entry_i_)(_cx_self* self, i_ssize tn, const _cx_keyraw* rkey, _cx_result* _res) {
    i_ssize up[64], tx = tn;
    _cx_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        const _cx_keyraw _raw = i_keyto(_i_keyref(&d[tx].value));
        if (!(c = i_cmp((&_raw), rkey)))
            { _res->ref = &d[tx].value; return tn; }
        dir = (c < 0);
        tx = d[tx].link[dir];
    }
    if ((tx = _cx_memb(_new_node_)(self, 1)) == 0)
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
        up[top] = _cx_memb(_skew_)(d, up[top]);
        up[top] = _cx_memb(_split_)(d, up[top]);
        if (top)
            d[up[top - 1]].link[dir] = up[top];
    }
    return up[0];
}

static _cx_result
_cx_memb(_insert_entry_)(_cx_self* self, _cx_keyraw rkey) {
    _cx_result res = {NULL};
    i_ssize tn = _cx_memb(_insert_entry_i_)(self, self->root, &rkey, &res);
    self->root = tn;
    self->size += res.inserted;
    return res;
}

static i_ssize
_cx_memb(_erase_r_)(_cx_self *self, i_ssize tn, const _cx_keyraw* rkey, int *erased) {
    _cx_node *d = self->nodes;
    if (tn == 0)
        return 0;
    _cx_keyraw raw = i_keyto(_i_keyref(&d[tn].value));
    i_ssize tx; int c = i_cmp((&raw), rkey);
    if (c != 0)
        d[tn].link[c < 0] = _cx_memb(_erase_r_)(self, d[tn].link[c < 0], rkey, erased);
    else {
        if (!(*erased)++)
            _cx_memb(_value_drop)(&d[tn].value);
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keyto(_i_keyref(&d[tn].value));
            d[tn].link[0] = _cx_memb(_erase_r_)(self, d[tn].link[0], &raw, erased);
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
                       tn = _cx_memb(_skew_)(d, tn);
       tx = d[tn].link[1] = _cx_memb(_skew_)(d, d[tn].link[1]);
            d[tx].link[1] = _cx_memb(_skew_)(d, d[tx].link[1]);
                       tn = _cx_memb(_split_)(d, tn);
            d[tn].link[1] = _cx_memb(_split_)(d, d[tn].link[1]);
    }
    return tn;
}

STC_DEF int
_cx_memb(_erase)(_cx_self* self, _cx_keyraw rkey) {
    int erased = 0;
    i_ssize root = _cx_memb(_erase_r_)(self, self->root, &rkey, &erased);
    if (!erased)
        return 0;
    self->root = root;
    --self->size;
    return 1;
}

STC_DEF _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_keyraw raw = i_keyto(_i_keyref(it.ref));
    _cx_memb(_next)(&it);
    if (it.ref) {
        _cx_keyraw nxt = i_keyto(_i_keyref(it.ref));
        _cx_memb(_erase)(self, raw);
        _cx_memb(_find_it)(self, nxt, &it);
    } else
        _cx_memb(_erase)(self, raw);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    if (!it2.ref) { 
        while (it1.ref)
            it1 = _cx_memb(_erase_at)(self, it1);
        return it1;
    }
    _cx_key k1 = *_i_keyref(it1.ref), k2 = *_i_keyref(it2.ref);
    _cx_keyraw r1 = i_keyto((&k1));
    for (;;) {
        if (memcmp(&k1, &k2, sizeof k1) == 0)
            return it1;
        _cx_memb(_next)(&it1);
        k1 = *_i_keyref(it1.ref);
        _cx_memb(_erase)(self, r1);
        r1 = i_keyto((&k1));
        _cx_memb(_find_it)(self, r1, &it1);
    }
}

#if !defined i_no_clone
static i_ssize
_cx_memb(_clone_r_)(_cx_self* self, _cx_node* src, i_ssize sn) {
    if (sn == 0)
        return 0;
    i_ssize tx, tn = _cx_memb(_new_node_)(self, src[sn].level);
    self->nodes[tn].value = _cx_memb(_value_clone)(src[sn].value);
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF _cx_self
_cx_memb(_clone)(_cx_self tree) {
    _cx_self clone = _cx_memb(_with_capacity)(tree.size);
    i_ssize root = _cx_memb(_clone_r_)(&clone, tree.nodes, tree.root);
    clone.root = root;
    clone.size = tree.size;
    return clone;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_result
_cx_memb(_emplace)(_cx_self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *_i_keyref(res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}
#endif // i_no_emplace

static void
_cx_memb(_drop_r_)(_cx_node* d, i_ssize tn) {
    if (tn) {
        _cx_memb(_drop_r_)(d, d[tn].link[0]);
        _cx_memb(_drop_r_)(d, d[tn].link[1]);
        _cx_memb(_value_drop)(&d[tn].value);
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    if (self->cap) {
        _cx_memb(_drop_r_)(self->nodes, self->root);
        i_free(self->nodes);
    }
}

#endif // i_implement
#undef _i_size
#undef _i_isset
#undef _i_ismap
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CSMAP_H_INCLUDED
#include "priv/template2.h"
