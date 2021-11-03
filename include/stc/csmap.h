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

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#include <stc/cstr.h>

#define i_tag sx  // Sorted map<cstr, double>
#define i_key_str
#define i_val double
#include <stc/csmap.h>

int main(void) {
    c_autovar (csmap_sx m = csmap_sx_init(), csmap_sx_del(&m))
    {
        csmap_sx_emplace(&m, "Testing one", 1.234);
        csmap_sx_emplace(&m, "Testing two", 12.34);
        csmap_sx_emplace(&m, "Testing three", 123.4);

        csmap_sx_value *v = csmap_sx_get(&m, "Testing five"); // NULL
        double num = *csmap_sx_at(&m, "Testing one");
        csmap_sx_emplace_or_assign(&m, "Testing three", 1000.0); // update
        csmap_sx_erase(&m, "Testing two");

        c_foreach (i, csmap_sx, m)
            printf("map %s: %g\n", i.ref->first.str, i.ref->second);
    }
}
*/

#ifndef CSMAP_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct csmap_rep { size_t root, disp, head, size, cap; void* nodes[]; };
#define _csmap_rep(self) c_container_of((self)->nodes, struct csmap_rep, nodes)
#endif // CSMAP_H_INCLUDED

#ifndef i_prefix
#define i_prefix csmap_
#endif
#ifdef i_isset
  #define cx_MAP_ONLY c_false
  #define cx_SET_ONLY c_true
  #define cx_keyref(vp) (vp)
#else
  #define cx_MAP_ONLY c_true
  #define cx_SET_ONLY c_false
  #define cx_keyref(vp) (&(vp)->first)
#endif
#include "template.h"

#ifndef i_fwd
_cx_deftypes(_c_aatree_types, _cx_self, i_key, i_val, cx_MAP_ONLY, cx_SET_ONLY);
#endif

cx_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )
struct _cx_node {
    _cx_size link[2];
    int8_t level;
    _cx_value value;
};

typedef i_keyraw _cx_rawkey;
typedef i_valraw _cx_memb(_rawmapped);
typedef cx_SET_ONLY( i_keyraw )
        cx_MAP_ONLY( struct { i_keyraw first; i_valraw second; } )
        _cx_rawvalue;

STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_self        _cx_memb(_clone)(_cx_self tree);
STC_API void            _cx_memb(_del)(_cx_self* self);
STC_API void            _cx_memb(_reserve)(_cx_self* self, size_t cap);
STC_API _cx_value*      _cx_memb(_find_it)(const _cx_self* self, i_keyraw rkey, _cx_iter* out);
STC_API _cx_iter        _cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey);
STC_API _cx_value*      _cx_memb(_front)(const _cx_self* self);
STC_API _cx_value*      _cx_memb(_back)(const _cx_self* self);
STC_API int             _cx_memb(_erase)(_cx_self* self, i_keyraw rkey);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API _cx_result      _cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey);
STC_API void            _cx_memb(_next)(_cx_iter* it);

STC_INLINE bool         _cx_memb(_empty)(_cx_self tree) { return _csmap_rep(&tree)->size == 0; }
STC_INLINE size_t       _cx_memb(_size)(_cx_self tree) { return _csmap_rep(&tree)->size; }
STC_INLINE size_t       _cx_memb(_capacity)(_cx_self tree) { return _csmap_rep(&tree)->cap; }
STC_INLINE void         _cx_memb(_clear)(_cx_self* self) { _cx_memb(_del)(self); *self = _cx_memb(_init)(); }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _cx_value* _cx_memb(_get)(const _cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }
STC_INLINE _cx_value*   _cx_memb(_mutget)(_cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }

STC_INLINE _cx_self
_cx_memb(_with_capacity)(size_t size) {
    _cx_self tree = _cx_memb(_init)();
    _cx_memb(_reserve)(&tree, size);
    return tree;
}

STC_INLINE void
_cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->nodes == other.nodes) return;
    _cx_memb(_del)(self); *self = _cx_memb(_clone)(other);
}

STC_INLINE _cx_rawvalue
_cx_memb(_value_toraw)(_cx_value* val) {
    return cx_SET_ONLY( i_keyto(val) )
           cx_MAP_ONLY( c_make(_cx_rawvalue){i_keyto(&val->first), i_valto(&val->second)} );
}

STC_INLINE void
_cx_memb(_value_del)(_cx_value* val) {
    i_keydel(cx_keyref(val));
    cx_MAP_ONLY( i_valdel(&val->second); )
}

STC_INLINE void
_cx_memb(_value_clone)(_cx_value* dst, _cx_value* val) {
    *cx_keyref(dst) = i_keyfrom(i_keyto(cx_keyref(val)));
    cx_MAP_ONLY( dst->second = i_valfrom(i_valto(&val->second)); )
}

cx_MAP_ONLY(
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped);
    STC_API _cx_result _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped);

    STC_INLINE _cx_result
    _cx_memb(_put)(_cx_self* self, i_key key, i_val mapped)
        { return _cx_memb(_insert_or_assign)(self, key, mapped); }

    STC_INLINE const _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, i_keyraw rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
)

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, i_keyraw rkey) {
    _cx_iter it;
    _cx_memb(_find_it)(self, rkey, &it);
    return it;
}

STC_INLINE _cx_result
_cx_memb(_emplace)(_cx_self* self, i_keyraw rkey cx_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *cx_keyref(res.ref) = i_keyfrom(rkey);
        cx_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}

STC_INLINE _cx_result
_cx_memb(_insert)(_cx_self* self, i_key key cx_MAP_ONLY(, i_val mapped)) {
    _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto(&key));
    if (res.inserted) { *cx_keyref(res.ref) = key; cx_MAP_ONLY( res.ref->second = mapped; )}
    else              { i_keydel(&key); cx_MAP_ONLY( i_valdel(&mapped); )}
    return res;
}

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it; it._d = self->nodes, it._top = 0;
    it._tn = (_cx_size) _csmap_rep(self)->root;
    if (it._tn) _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self) {
    (void)self;
    return c_make(_cx_iter){.ref = NULL};
}

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_memb(_next)(&it);
    return it;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

#ifndef CSMAP_H_INCLUDED
static struct csmap_rep _csmap_sentinel = {0, 0, 0, 0, 0};
#endif // CSMAP_H_INCLUDED

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self tree = {(_cx_node *) _csmap_sentinel.nodes};
    return tree;
}

STC_DEF _cx_value*
_cx_memb(_front)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    _cx_size tn = (_cx_size) _csmap_rep(self)->root;
    while (d[tn].link[0]) tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _cx_value*
_cx_memb(_back)(const _cx_self* self) {
    _cx_node *d = self->nodes;
    _cx_size tn = (_cx_size) _csmap_rep(self)->root;
    while (d[tn].link[1]) tn = d[tn].link[1];
    return &d[tn].value;
}

STC_DEF void
_cx_memb(_reserve)(_cx_self* self, size_t cap) {
    struct csmap_rep* rep = _csmap_rep(self);
    _cx_size oldcap = rep->cap;
    if (cap > oldcap) {
        rep = (struct csmap_rep*) c_realloc(oldcap ? rep : NULL,
                                            sizeof(struct csmap_rep) + (cap + 1)*sizeof(_cx_node));
        if (oldcap == 0)
            memset(rep, 0, sizeof(struct csmap_rep) + sizeof(_cx_node));
        rep->cap = cap;
        self->nodes = (_cx_node *) rep->nodes;
    }
}

STC_DEF _cx_size
_cx_memb(_node_new_)(_cx_self* self, int level) {
    size_t tn; struct csmap_rep *rep = _csmap_rep(self);
    if (rep->disp) {
        tn = rep->disp;
        rep->disp = self->nodes[tn].link[1];
    } else {
        if ((tn = rep->head + 1) > rep->cap) _cx_memb(_reserve)(self, 4 + (tn*13 >> 3));
        ++_csmap_rep(self)->head; /* do after reserve */
    }
    _cx_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = level;
    return (_cx_size) tn;
}

cx_MAP_ONLY(
    STC_DEF _cx_result
    _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped) {
        _cx_result res = _cx_memb(_insert_entry_)(self, i_keyto(&key));
        if (res.inserted) res.ref->first = key;
        else { i_keydel(&key); i_valdel(&res.ref->second); }
        res.ref->second = mapped; return res;
    }

    STC_DEF _cx_result
    _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped) {
        _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
        if (res.inserted) res.ref->first = i_keyfrom(rkey);
        else i_valdel(&res.ref->second);
        res.ref->second = i_valfrom(rmapped); return res;
    }
)

STC_DEF _cx_value*
_cx_memb(_find_it)(const _cx_self* self, i_keyraw rkey, _cx_iter* out) {
    _cx_size tn = _csmap_rep(self)->root;
    _cx_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; _cx_rawkey raw = i_keyto(cx_keyref(&d[tn].value));
        if ((c = i_cmp(&raw, &rkey)) < 0)
            tn = d[tn].link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = d[tn].link[0]; }
        else
            { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); }
    }
    return (out->ref = NULL);
}

STC_DEF _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey) {
    _cx_iter it;
    _cx_memb(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        _cx_size tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF void
_cx_memb(_next)(_cx_iter *it) {
    _cx_size tn = it->_tn;
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

STC_DEF _cx_size
_cx_memb(_skew_)(_cx_node *d, _cx_size tn) {
    if (tn && d[d[tn].link[0]].level == d[tn].level) {
        _cx_size tmp = d[tn].link[0];
        d[tn].link[0] = d[tmp].link[1];
        d[tmp].link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF _cx_size
_cx_memb(_split_)(_cx_node *d, _cx_size tn) {
    if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) {
        _cx_size tmp = d[tn].link[1];
        d[tn].link[1] = d[tmp].link[0];
        d[tmp].link[0] = tn;
        tn = tmp;
        ++d[tn].level;
    }
    return tn;
}

STC_DEF _cx_size
_cx_memb(_insert_entry_i_)(_cx_self* self, _cx_size tn, const _cx_rawkey* rkey, _cx_result* res) {
    _cx_size up[64], tx = tn;
    _cx_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        i_keyraw raw = i_keyto(cx_keyref(&d[tx].value));
        if ((c = i_cmp(&raw, rkey)) == 0) {res->ref = &d[tx].value; return tn; }
        dir = (c < 0);
        tx = d[tx].link[dir];
    }
    tx = _cx_memb(_node_new_)(self, 1); d = self->nodes;
    res->ref = &d[tx].value, res->inserted = true;
    if (top == 0) return tx;
    d[up[top - 1]].link[dir] = tx;
    while (top--) {
        if (top) dir = (d[up[top - 1]].link[1] == up[top]);
        up[top] = _cx_memb(_skew_)(d, up[top]);
        up[top] = _cx_memb(_split_)(d, up[top]);
        if (top) d[up[top - 1]].link[dir] = up[top];
    }
    return up[0];
}

STC_DEF _cx_result
_cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey) {
    _cx_result res = {NULL, false};
    _cx_size tn = _cx_memb(_insert_entry_i_)(self, (_cx_size) _csmap_rep(self)->root, &rkey, &res);
    _csmap_rep(self)->root = tn;
    _csmap_rep(self)->size += res.inserted;
    return res;
}

STC_DEF _cx_size
_cx_memb(_erase_r_)(_cx_node *d, _cx_size tn, const _cx_rawkey* rkey, int *erased) {
    if (tn == 0)
        return 0;
    i_keyraw raw = i_keyto(cx_keyref(&d[tn].value));
    _cx_size tx; int c = i_cmp(&raw, rkey);
    if (c != 0)
        d[tn].link[c < 0] = _cx_memb(_erase_r_)(d, d[tn].link[c < 0], rkey, erased);
    else {
        if (!(*erased)++)
            _cx_memb(_value_del)(&d[tn].value);
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keyto(cx_keyref(&d[tn].value));
            d[tn].link[0] = _cx_memb(_erase_r_)(d, d[tn].link[0], &raw, erased);
        } else { /* unlink node */
            tx = tn;
            tn = d[tn].link[ d[tn].link[0] == 0 ];
            /* move it to disposed nodes list */
            struct csmap_rep *rep = c_container_of(d, struct csmap_rep, nodes);
            d[tx].link[1] = (_cx_size) rep->disp;
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
_cx_memb(_erase)(_cx_self* self, i_keyraw rkey) {
    int erased = 0;
    _cx_size root = _cx_memb(_erase_r_)(self->nodes, (_cx_size) _csmap_rep(self)->root, &rkey, &erased);
    return erased ? (_csmap_rep(self)->root = root, --_csmap_rep(self)->size, 1) : 0;
}

STC_DEF _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_rawkey raw = i_keyto(cx_keyref(it.ref)), nxt;
    _cx_memb(_next)(&it);
    if (it.ref) nxt = i_keyto(cx_keyref(it.ref));
    _cx_memb(_erase)(self, raw);
    if (it.ref) _cx_memb(_find_it)(self, nxt, &it);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    if (!it2.ref) { while (it1.ref) it1 = _cx_memb(_erase_at)(self, it1);
                    return it1; }
    _cx_key k1 = *cx_keyref(it1.ref), k2 = *cx_keyref(it2.ref);
    _cx_rawkey r1 = i_keyto(&k1);
    for (;;) {
        if (memcmp(&k1, &k2, sizeof k1) == 0) return it1;
        _cx_memb(_next)(&it1); k1 = *cx_keyref(it1.ref);
        _cx_memb(_erase)(self, r1);
        _cx_memb(_find_it)(self, (r1 = i_keyto(&k1)), &it1);
    }
}

STC_DEF _cx_size
_cx_memb(_clone_r_)(_cx_self* self, _cx_node* src, _cx_size sn) {
    if (sn == 0) return 0;
    _cx_size tx, tn = _cx_memb(_node_new_)(self, src[sn].level);
    _cx_memb(_value_clone)(&self->nodes[tn].value, &src[sn].value);
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _cx_memb(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF _cx_self
_cx_memb(_clone)(_cx_self tree) {
    _cx_self clone = _cx_memb(_with_capacity)(_csmap_rep(&tree)->size);
    _cx_size root = _cx_memb(_clone_r_)(&clone, tree.nodes, (_cx_size) _csmap_rep(&tree)->root);
    _csmap_rep(&clone)->root = root;
    _csmap_rep(&clone)->size = _csmap_rep(&tree)->size;
    return clone;
}

STC_DEF void
_cx_memb(_del_r_)(_cx_node* d, _cx_size tn) {
    if (tn) {
        _cx_memb(_del_r_)(d, d[tn].link[0]);
        _cx_memb(_del_r_)(d, d[tn].link[1]);
        _cx_memb(_value_del)(&d[tn].value);
    }
}

STC_DEF void
_cx_memb(_del)(_cx_self* self) {
    if (_csmap_rep(self)->root) {
        _cx_memb(_del_r_)(self->nodes, (_cx_size) _csmap_rep(self)->root);
        c_free(_csmap_rep(self));
    }
}

#endif // IMPLEMENTATION
#undef i_isset
#undef cx_keyref
#undef cx_MAP_ONLY
#undef cx_SET_ONLY
#include "template.h"
#define CSMAP_H_INCLUDED
