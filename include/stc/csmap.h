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

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#include <stc/cstr.h>

#define i_tag sx  // Sorted map<cstr, double>
#define i_key_str
#define i_val double
#include <stc/csmap.h>

int main(void) {
    c_autovar (csmap_sx m = csmap_sx_init(), csmap_sx_drop(&m))
    {
        csmap_sx_emplace(&m, "Testing one", 1.234);
        csmap_sx_emplace(&m, "Testing two", 12.34);
        csmap_sx_emplace(&m, "Testing three", 123.4);

        csmap_sx_value *v = csmap_sx_get(&m, "Testing five"); // NULL
        double num = *csmap_sx_at(&m, "Testing one");
        csmap_sx_emplace_or_assign(&m, "Testing three", 1000.0); // update
        csmap_sx_erase(&m, "Testing two");

        c_foreach (i, csmap_sx, m)
            printf("map %s: %g\n", cstr_str(&i.ref->first), i.ref->second);
    }
}
*/
#ifdef STC_CSMAP_V1
#include "alt/csmap.h"
#else
#include "ccommon.h"

#ifndef CSMAP_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct csmap_rep { size_t root, disp, head, size, cap; unsigned nodes[1]; };
#define _csmap_rep(self) c_unchecked_container_of((self)->nodes, struct csmap_rep, nodes)
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
#include "template.h"

#if !c_option(c_is_fwd)
_cx_deftypes(_c_aatree_types, _cx_self, i_key, i_val, i_size, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )
struct _cx_node {
    i_size link[2];
    int8_t level;
    _cx_value value;
};

typedef i_keyraw _cx_rawkey;
typedef i_valraw _cx_memb(_rawmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first; i_valraw second; } )
        _cx_raw;

#if !defined _i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self tree);
#if !defined _i_no_emplace
STC_API _cx_result      _cx_memb(_emplace)(_cx_self* self, _cx_rawkey rkey _i_MAP_ONLY(, i_valraw rmapped));
#endif // !_i_no_emplace
#endif // !_i_no_clone
STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_result      _cx_memb(_insert)(_cx_self* self, i_key key _i_MAP_ONLY(, i_val mapped));
STC_API _cx_result      _cx_memb(_push)(_cx_self* self, _cx_value _val);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, size_t cap);
STC_API _cx_value*      _cx_memb(_find_it)(const _cx_self* self, _cx_rawkey rkey, _cx_iter* out);
STC_API _cx_iter        _cx_memb(_lower_bound)(const _cx_self* self, _cx_rawkey rkey);
STC_API _cx_value*      _cx_memb(_front)(const _cx_self* self);
STC_API _cx_value*      _cx_memb(_back)(const _cx_self* self);
STC_API int             _cx_memb(_erase)(_cx_self* self, _cx_rawkey rkey);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API void            _cx_memb(_next)(_cx_iter* it);

STC_INLINE bool         _cx_memb(_empty)(_cx_self cx) { return _csmap_rep(&cx)->size == 0; }
STC_INLINE size_t       _cx_memb(_size)(_cx_self cx) { return _csmap_rep(&cx)->size; }
STC_INLINE size_t       _cx_memb(_capacity)(_cx_self cx) { return _csmap_rep(&cx)->cap; }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE _cx_iter     _cx_memb(_find)(const _cx_self* self, _cx_rawkey rkey)
                            { _cx_iter it; _cx_memb(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, _cx_rawkey rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _cx_value* _cx_memb(_get)(const _cx_self* self, _cx_rawkey rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }
STC_INLINE _cx_value*   _cx_memb(_get_mut)(_cx_self* self, _cx_rawkey rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }

STC_INLINE _cx_self
_cx_memb(_with_capacity)(const size_t cap) {
    _cx_self tree = _cx_memb(_init)();
    _cx_memb(_reserve)(&tree, cap);
    return tree;
}

STC_INLINE void
_cx_memb(_clear)(_cx_self* self) 
    { _cx_memb(_drop)(self); *self = _cx_memb(_init)(); }

STC_INLINE _cx_raw
_cx_memb(_value_toraw)(_cx_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_make(_cx_raw){i_keyto((&val->first)), 
                                        i_valto((&val->second))} );
}

STC_INLINE int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_rawkey rx = i_keyto(_i_keyref(x)), ry = i_keyto(_i_keyref(y));
    return i_cmp((&rx), (&ry));
}

STC_INLINE void
_cx_memb(_value_drop)(_cx_value* val) {
    i_keydrop(_i_keyref(val));
    _i_MAP_ONLY( i_valdrop((&val->second)); )
}

#if !defined _i_no_clone
STC_INLINE _cx_value
_cx_memb(_value_clone)(_cx_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}

STC_INLINE void
_cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->nodes == other.nodes)
        return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(other);
}

STC_INLINE void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    _cx_self tmp = _cx_memb(_clone)(*self);
    _cx_memb(_drop)(self); *self = tmp;
}
#endif // !_i_no_clone

#ifndef _i_isset
    #if !defined _i_no_clone && !defined _i_no_emplace
    STC_API _cx_result _cx_memb(_emplace_or_assign)(_cx_self* self, _cx_rawkey rkey, i_valraw rmapped);
    #endif
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped);

    STC_INLINE const _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, _cx_rawkey rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
    STC_INLINE _cx_mapped*
    _cx_memb(_at_mut)(_cx_self* self, _cx_rawkey rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
#endif // !_i_isset

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it;
    it._d = self->nodes, it._top = 0;
    it._tn = (i_size) _csmap_rep(self)->root;
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

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

#ifndef CSMAP_H_INCLUDED
static struct csmap_rep _csmap_sentinel = {0, 0, 0, 0, 0};
#endif

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self tree = {(_cx_node *)_csmap_sentinel.nodes};
    return tree;
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const size_t cap) {
    struct csmap_rep* rep = _csmap_rep(self), *oldrep;
    if (cap >= rep->size) {
        // second test is bogus, but supresses gcc warning:
        oldrep = rep->cap && rep != &_csmap_sentinel ? rep : NULL;
        rep = (struct csmap_rep*) c_realloc(oldrep, offsetof(struct csmap_rep, nodes) + 
                                                    (cap + 1)*sizeof(_cx_node));
        if (!rep)
            return false;
        if (oldrep == NULL)
            memset(rep, 0, offsetof(struct csmap_rep, nodes) + sizeof(_cx_node));
        rep->cap = cap;
        self->nodes = (_cx_node *) rep->nodes;
    }
    return true;
}

STC_DEF _cx_value*
_cx_memb(_front)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    i_size tn = (i_size) _csmap_rep(self)->root;
    while (d[tn].link[0])
        tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _cx_value*
_cx_memb(_back)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    i_size tn = (i_size) _csmap_rep(self)->root;
    while (d[tn].link[1])
        tn = d[tn].link[1];
    return &d[tn].value;
}

static i_size
_cx_memb(_new_node_)(_cx_self* self, int level) {
    i_size tn; struct csmap_rep *rep = _csmap_rep(self);
    if (rep->disp) {
        tn = rep->disp;
        rep->disp = self->nodes[tn].link[1];
    } else {
        if (rep->head == rep->cap)
            if (!_cx_memb(_reserve)(self, rep->head*3/2 + 4))
                return 0;
        tn = ++_csmap_rep(self)->head; /* start with 1, 0 is nullnode. */
    }
    _cx_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = level;
    return tn;
}

static _cx_result _cx_memb(_insert_entry_)(_cx_self* self, _cx_rawkey rkey);

STC_DEF _cx_result
_cx_memb(_insert)(_cx_self* self, i_key key _i_MAP_ONLY(, i_val mapped)) {
    _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto((&key)));
    if (res.inserted)
        { *_i_keyref(res.ref) = key; _i_MAP_ONLY( res.ref->second = mapped; )}
    else 
        { i_keydrop((&key)); _i_MAP_ONLY( i_valdrop((&mapped)); )}
    return res;
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
    _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped) {
        _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto((&key)));
        if (!res.nomem_error) {
            if (res.inserted)
                res.ref->first = key;
            else
                { i_keydrop((&key)); i_valdrop((&res.ref->second)); }
            res.ref->second = mapped;
        }
        return res;
    }

    #if !defined _i_no_clone && !defined _i_no_emplace
    STC_DEF _cx_result
    _cx_memb(_emplace_or_assign)(_cx_self* self, _cx_rawkey rkey, i_valraw rmapped) {
        _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
        if (!res.nomem_error) {
            if (res.inserted)
                res.ref->first = i_keyfrom(rkey);
            else
                { i_valdrop((&res.ref->second)); }
            res.ref->second = i_valfrom(rmapped);
        }
        return res;
    }
    #endif // !_i_no_clone && !_i_no_emplace
#endif // !_i_isset

STC_DEF _cx_value*
_cx_memb(_find_it)(const _cx_self* self, _cx_rawkey rkey, _cx_iter* out) {
    i_size tn = _csmap_rep(self)->root;
    _cx_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; const _cx_rawkey raw = i_keyto(_i_keyref(&d[tn].value));
        if ((c = i_cmp((&raw), (&rkey))) < 0)
            tn = d[tn].link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = d[tn].link[0]; }
        else
            { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); }
    }
    return (out->ref = NULL);
}

STC_DEF _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, _cx_rawkey rkey) {
    _cx_iter it;
    _cx_memb(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        i_size tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF void
_cx_memb(_next)(_cx_iter *it) {
    i_size tn = it->_tn;
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

STC_DEF i_size
_cx_memb(_skew_)(_cx_node *d, i_size tn) {
    if (tn && d[d[tn].link[0]].level == d[tn].level) {
        i_size tmp = d[tn].link[0];
        d[tn].link[0] = d[tmp].link[1];
        d[tmp].link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF i_size
_cx_memb(_split_)(_cx_node *d, i_size tn) {
    if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) {
        i_size tmp = d[tn].link[1];
        d[tn].link[1] = d[tmp].link[0];
        d[tmp].link[0] = tn;
        tn = tmp;
        ++d[tn].level;
    }
    return tn;
}

static i_size
_cx_memb(_insert_entry_i_)(_cx_self* self, i_size tn, const _cx_rawkey* rkey, _cx_result* res) {
    i_size up[64], tx = tn;
    _cx_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        const _cx_rawkey raw = i_keyto(_i_keyref(&d[tx].value));
        if (!(c = i_cmp((&raw), rkey)))
            { res->ref = &d[tx].value; return tn; }
        dir = (c < 0);
        tx = d[tx].link[dir];
    }
    if ((tx = _cx_memb(_new_node_)(self, 1)) == 0)
        { res->nomem_error = true; return 0; }
    d = self->nodes;
    res->ref = &d[tx].value, res->inserted = true;
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
_cx_memb(_insert_entry_)(_cx_self* self, _cx_rawkey rkey) {
    _cx_result res = {NULL};
    i_size tn = _cx_memb(_insert_entry_i_)(self, (i_size) _csmap_rep(self)->root, &rkey, &res);
    _csmap_rep(self)->root = tn;
    _csmap_rep(self)->size += res.inserted;
    return res;
}

static i_size
_cx_memb(_erase_r_)(_cx_node *d, i_size tn, const _cx_rawkey* rkey, int *erased) {
    if (tn == 0)
        return 0;
    _cx_rawkey raw = i_keyto(_i_keyref(&d[tn].value));
    i_size tx; int c = i_cmp((&raw), rkey);
    if (c != 0)
        d[tn].link[c < 0] = _cx_memb(_erase_r_)(d, d[tn].link[c < 0], rkey, erased);
    else {
        if (!(*erased)++)
            _cx_memb(_value_drop)(&d[tn].value);
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keyto(_i_keyref(&d[tn].value));
            d[tn].link[0] = _cx_memb(_erase_r_)(d, d[tn].link[0], &raw, erased);
        } else { /* unlink node */
            tx = tn;
            tn = d[tn].link[ d[tn].link[0] == 0 ];
            /* move it to disposed nodes list */
            struct csmap_rep *rep = c_unchecked_container_of(d, struct csmap_rep, nodes);
            d[tx].link[1] = (i_size) rep->disp;
            rep->disp = tx;
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
_cx_memb(_erase)(_cx_self* self, _cx_rawkey rkey) {
    int erased = 0;
    i_size root = _cx_memb(_erase_r_)(self->nodes, (i_size) _csmap_rep(self)->root, &rkey, &erased);
    if (erased) {
        _csmap_rep(self)->root = root;
        --_csmap_rep(self)->size;
        return 1;
    }
    return 0;
}

STC_DEF _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_rawkey raw = i_keyto(_i_keyref(it.ref)), nxt;
    _cx_memb(_next)(&it);
    if (it.ref)
        nxt = i_keyto(_i_keyref(it.ref));
    _cx_memb(_erase)(self, raw);
    if (it.ref)
        _cx_memb(_find_it)(self, nxt, &it);
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
    _cx_rawkey r1 = i_keyto((&k1));
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

#if !defined _i_no_clone
static i_size
_cx_memb(_clone_r_)(_cx_self* self, _cx_node* src, i_size sn) {
    if (sn == 0)
        return 0;
    i_size tx, tn = _cx_memb(_new_node_)(self, src[sn].level);
    self->nodes[tn].value = _cx_memb(_value_clone)(src[sn].value);
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF _cx_self
_cx_memb(_clone)(_cx_self tree) {
    _cx_self clone = _cx_memb(_with_capacity)(_csmap_rep(&tree)->size);
    i_size root = _cx_memb(_clone_r_)(&clone, tree.nodes, (i_size) _csmap_rep(&tree)->root);
    _csmap_rep(&clone)->root = root;
    _csmap_rep(&clone)->size = _csmap_rep(&tree)->size;
    return clone;
}

#if !defined _i_no_emplace
STC_DEF _cx_result
_cx_memb(_emplace)(_cx_self* self, _cx_rawkey rkey _i_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *_i_keyref(res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}
#endif // _i_no_emplace
#endif // !_i_no_clone

static void
_cx_memb(_drop_r_)(_cx_node* d, i_size tn) {
    if (tn) {
        _cx_memb(_drop_r_)(d, d[tn].link[0]);
        _cx_memb(_drop_r_)(d, d[tn].link[1]);
        _cx_memb(_value_drop)(&d[tn].value);
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    struct csmap_rep* rep = _csmap_rep(self);
    // second test is bogus, but supresses gcc warning:
    if (rep->cap && rep != &_csmap_sentinel) {
        _cx_memb(_drop_r_)(self->nodes, (i_size) rep->root);
        c_free(rep);  // correct, but may give warning
    }
}

#endif // i_implement
#undef _i_isset
#undef _i_ismap
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CSMAP_H_INCLUDED
#include "template.h"
#endif // !STC_CSMAP_V1
