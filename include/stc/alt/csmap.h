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
#include <stc/ccommon.h>

#ifndef CSMAP_H_INCLUDED
#define STC_CSMAP_V1 1
#include <stc/forward.h>
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
#include <stc/template.h>

#if !c_option(c_is_fwd)
_cx_deftypes(_c_aatree_types, _cx_self, i_key, i_val, i_size, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )
struct _cx_node {
    struct _cx_node *link[2];
    uint8_t level;
    _cx_value value;
};

typedef i_keyraw _cx_rawkey;
typedef i_valraw _cx_memb(_rawmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first; i_valraw second; } )
        _cx_raw;

#if !defined _i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
#if !defined _i_no_emplace
STC_API _cx_result      _cx_memb(_emplace)(_cx_self* self, i_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped));
#endif // !_i_no_emplace
#endif // !_i_no_clone
STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_result      _cx_memb(_insert)(_cx_self* self, i_key key _i_MAP_ONLY(, i_val mapped));
STC_API _cx_result      _cx_memb(_push)(_cx_self* self, _cx_value _val);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_find_it)(const _cx_self* self, i_keyraw rkey, _cx_iter* out);
STC_API _cx_iter        _cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey);
STC_API _cx_value*      _cx_memb(_front)(const _cx_self* self);
STC_API _cx_value*      _cx_memb(_back)(const _cx_self* self);
STC_API int             _cx_memb(_erase)(_cx_self* self, i_keyraw rkey);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API void            _cx_memb(_next)(_cx_iter* it);

STC_INLINE bool         _cx_memb(_empty)(_cx_self cx) { return cx.size == 0; }
STC_INLINE size_t       _cx_memb(_size)(_cx_self cx) { return cx.size; }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE _cx_iter     _cx_memb(_find)(const _cx_self* self, i_keyraw rkey)
                            { _cx_iter it; _cx_memb(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _cx_value* _cx_memb(_get)(const _cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }
STC_INLINE _cx_value*   _cx_memb(_get_mut)(_cx_self* self, i_keyraw rkey)
                            { _cx_iter it; return _cx_memb(_find_it)(self, rkey, &it); }

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
    _cx_rawkey rx = i_keyto(_i_keyref(x)), ry = i_keyto(_i_keyref(y));
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
    if (self->root == other.root)
        return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(other);
}
#endif // !_i_no_clone

#ifndef _i_isset
    #if !defined _i_no_clone && !defined _i_no_emplace
    STC_API _cx_result _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped);
    #endif
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped);

    STC_INLINE const _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, i_keyraw rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
    STC_INLINE _cx_mapped*
    _cx_memb(_at_mut)(_cx_self* self, i_keyraw rkey)
        { _cx_iter it; return &_cx_memb(_find_it)(self, rkey, &it)->second; }
#endif // !_i_isset

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it; 
    it.ref = NULL, it._top = 0, it._tn = self->root;
    _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self) {
    (void)self;
    _cx_iter it; it.ref = NULL, it._top = 0, it._tn = NULL;
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
static struct { void *link[2]; uint8_t level; }
_csmap_sentinel = {{&_csmap_sentinel, &_csmap_sentinel}, 0};
#endif

static _cx_result _cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey);

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self cx = {(_cx_node *)&_csmap_sentinel, 0};
    return cx;
}

STC_DEF _cx_value*
_cx_memb(_front)(const _cx_self* self) {
    _cx_node *tn = self->root;
    while (tn->link[0]->level)
        tn = tn->link[0];
    return &tn->value;
}

STC_DEF _cx_value*
_cx_memb(_back)(const _cx_self* self) {
    _cx_node *tn = self->root;
    while (tn->link[1]->level)
        tn = tn->link[1];
    return &tn->value;
}

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
        if (res.inserted)
            res.ref->first = key;
        else
            { i_keydrop((&key)); i_valdrop((&res.ref->second)); }
        res.ref->second = mapped;
        return res;
    }
    #if !defined _i_no_clone && !defined _i_no_emplace
    STC_DEF _cx_result
    _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped) {
        _cx_result res = _cx_memb(_insert_entry_)(self, rkey);
        if (res.inserted)
            res.ref->first = i_keyfrom(rkey);
        else
            { i_valdrop((&res.ref->second)); }
        res.ref->second = i_valfrom(rmapped);
        return res;
    }
   #endif // !_i_no_clone && !_i_no_emplace
#endif // !_i_isset

STC_DEF _cx_value*
_cx_memb(_find_it)(const _cx_self* self, _cx_rawkey rkey, _cx_iter* out) {
    _cx_node *tn = self->root;
    out->_top = 0;
    while (tn->level) {
        int c; _cx_rawkey raw = i_keyto(_i_keyref(&tn->value));
        if ((c = i_cmp((&raw), (&rkey))) < 0)
            tn = tn->link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = tn->link[0]; }
        else
            { out->_tn = tn->link[1]; return (out->ref = &tn->value); }
    }
    return (out->ref = NULL);
}

STC_DEF _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, i_keyraw rkey) {
    _cx_iter it;
    _cx_memb(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        _cx_node *tn = it._st[--it._top];
        it._tn = tn->link[1];
        it.ref = &tn->value;
    }
    return it;
}

STC_DEF void
_cx_memb(_next)(_cx_iter *it) {
    _cx_node *tn = it->_tn;
    if (it->_top || tn->level) {
        while (tn->level) {
            it->_st[it->_top++] = tn;
            tn = tn->link[0];
        }
        tn = it->_st[--it->_top];
        it->_tn = tn->link[1];
        it->ref = &tn->value;
    } else
        it->ref = NULL;
}

static _cx_node *
_cx_memb(_skew_)(_cx_node *tn) {
    if (tn && tn->link[0]->level == tn->level && tn->level) {
        _cx_node *tmp = tn->link[0];
        tn->link[0] = tmp->link[1];
        tmp->link[1] = tn;
        tn = tmp;
    }
    return tn;
}

static _cx_node *
_cx_memb(_split_)(_cx_node *tn) {
    if (tn->link[1]->link[1]->level == tn->level && tn->level) {
        _cx_node *tmp = tn->link[1];
        tn->link[1] = tmp->link[0];
        tmp->link[0] = tn;
        tn = tmp;
        ++tn->level;
    }
    return tn;
}

static _cx_node*
_cx_memb(_insert_entry_i_)(_cx_node* tn, const _cx_rawkey* rkey, _cx_result* res) {
    _cx_node *up[64], *tx = tn;
    int c, top = 0, dir = 0;
    while (tx->level) {
        up[top++] = tx;
        _cx_rawkey r = i_keyto(_i_keyref(&tx->value));
        if (!(c = (i_cmp((&r), rkey))))
            { res->ref = &tx->value; return tn; }
        dir = (c < 0);
        tx = tx->link[dir];
    }
    tn = c_alloc(_cx_node);
    tn->link[0] = tn->link[1] = (_cx_node*)&_csmap_sentinel;
    tn->level = 1;
    res->ref = &tn->value, res->inserted = true;
    if (top == 0) 
        return tn;
    up[top - 1]->link[dir] = tn;
    while (top--) {
        if (top)
            dir = (up[top - 1]->link[1] == up[top]);
        up[top] = _cx_memb(_skew_)(up[top]);
        up[top] = _cx_memb(_split_)(up[top]);
        if (top)
            up[top - 1]->link[dir] = up[top];
    }
    return up[0];
}

STC_DEF _cx_result
_cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey) {
    _cx_result res = {NULL};
    self->root = _cx_memb(_insert_entry_i_)(self->root, &rkey, &res);
    self->size += res.inserted;
    return res;
}

static _cx_node*
_cx_memb(_erase_r_)(_cx_node *tn, const _cx_rawkey* rkey, int *erased) {
    if (tn->level == 0)
        return tn;
    _cx_rawkey raw = i_keyto(_i_keyref(&tn->value));
    _cx_node *tx; int c = (i_cmp((&raw), rkey));
    if (c != 0)
        tn->link[c < 0] = _cx_memb(_erase_r_)(tn->link[c < 0], rkey, erased);
    else {
        if (!*erased)
            { _cx_memb(_value_drop)(&tn->value); *erased = 1; }
        if (tn->link[0]->level && tn->link[1]->level) {
            tx = tn->link[0];
            while (tx->link[1]->level)
                tx = tx->link[1];
            tn->value = tx->value;
            raw = i_keyto(_i_keyref(&tn->value));
            tn->link[0] = _cx_memb(_erase_r_)(tn->link[0], &raw, erased);
        } else { /* unlink node */
            tx = tn;
            tn = tn->link[tn->link[0]->level == 0];
            c_free(tx);
        }
    }
    if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) {
        if (tn->link[1]->level > --tn->level)
            tn->link[1]->level = tn->level;
                     tn = _cx_memb(_skew_)(tn);
       tx = tn->link[0] = _cx_memb(_skew_)(tn->link[0]);
            tx->link[0] = _cx_memb(_skew_)(tx->link[0]);
                     tn = _cx_memb(_split_)(tn);
            tn->link[0] = _cx_memb(_split_)(tn->link[0]);
    }
    return tn;
}

STC_DEF int
_cx_memb(_erase)(_cx_self* self, i_keyraw rkey) {
    int erased = 0;
    self->root = _cx_memb(_erase_r_)(self->root, &rkey, &erased);
    self->size -= erased;
    return erased;
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
static _cx_node*
_cx_memb(_clone_r_)(_cx_node *tn) {
    if (! tn->level)
        return tn;
    _cx_node *cn = c_alloc(_cx_node);
    cn->level = tn->level;
    cn->value = _cx_memb(_value_clone)(tn->value);
    cn->link[0] = _cx_memb(_clone_r_)(tn->link[0]);
    cn->link[1] = _cx_memb(_clone_r_)(tn->link[1]);
    return cn;
}

STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) { 
    return c_make(_cx_self){_cx_memb(_clone_r_)(cx.root), cx.size};
}

#if !defined _i_no_emplace
STC_DEF _cx_result
_cx_memb(_emplace)(_cx_self* self, i_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) {
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
_cx_memb(_drop_r_)(_cx_node* tn) {
    if (tn->level != 0) {
        _cx_memb(_drop_r_)(tn->link[0]);
        _cx_memb(_drop_r_)(tn->link[1]);
        _cx_memb(_value_drop)(&tn->value);
        c_free(tn);
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) { 
    _cx_memb(_drop_r_)(self->root);
}

#endif // i_implement
#undef _i_isset
#undef _i_ismap
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CSMAP_H_INCLUDED
#include <stc/template.h>
