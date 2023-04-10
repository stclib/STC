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

// Unordered set/map - implemented as closed hashing with linear probing and no tombstones.
/*
#include <stdio.h>

#define i_tag ichar  // Map int => char
#define i_key int
#define i_val char
#include <stc/cmap.h>

int main(void) {
    cmap_ichar m = {0};
    cmap_ichar_emplace(&m, 5, 'a');
    cmap_ichar_emplace(&m, 8, 'b');
    cmap_ichar_emplace(&m, 12, 'c');

    cmap_ichar_value* v = cmap_ichar_get(&m, 10); // NULL
    char val = *cmap_ichar_at(&m, 5);               // 'a'
    cmap_ichar_emplace_or_assign(&m, 5, 'd');       // update
    cmap_ichar_erase(&m, 8);

    c_foreach (i, cmap_ichar, m)
        printf("map %d: %c\n", i.ref->first, i.ref->second);

    cmap_ichar_drop(&m);
}
*/
#include "ccommon.h"

#ifndef CMAP_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>
typedef struct { int64_t idx; uint8_t hx; } chash_bucket_t;
#endif // CMAP_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cmap_
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
#define _i_ishash
#ifndef i_max_load_factor
  #define i_max_load_factor 0.85f
#endif
#ifndef i_ssize
  #define i_ssize int32_t
  #define _i_size intptr_t
  #define _i_expandby 1
#else
  #define _i_expandby 2
  #define _i_size i_ssize
#endif
#include "priv/template.h"
#ifndef i_is_forward
  _cx_deftypes(_c_chash_types, _cx_self, i_key, i_val, i_ssize, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )

typedef i_keyraw _cx_keyraw;
typedef i_valraw _cx_memb(_rmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first;
                              i_valraw second; } )
_cx_raw;

STC_API _cx_self        _cx_memb(_with_capacity)(_i_size cap);
#if !defined i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self map);
#endif
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, _i_size capacity);
STC_API chash_bucket_t  _cx_memb(_bucket_)(const _cx_self* self, const _cx_keyraw* rkeyptr);
STC_API _cx_result      _cx_memb(_insert_entry_)(_cx_self* self, _cx_keyraw rkey);
STC_API void            _cx_memb(_erase_entry)(_cx_self* self, _cx_value* val);

STC_INLINE _cx_self     _cx_memb(_init)(void) { _cx_self map = {0}; return map; }
STC_INLINE void         _cx_memb(_shrink_to_fit)(_cx_self* self) { _cx_memb(_reserve)(self, self->size); }
STC_INLINE float        _cx_memb(_max_load_factor)(const _cx_self* self) { return (float)(i_max_load_factor); }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* map) { return !map->size; }
STC_INLINE _i_size      _cx_memb(_size)(const _cx_self* map) { return map->size; }
STC_INLINE _i_size      _cx_memb(_bucket_count)(_cx_self* map) { return map->bucket_count; }
STC_INLINE _i_size      _cx_memb(_capacity)(const _cx_self* map)
                            { return (_i_size)((float)map->bucket_count * (i_max_load_factor)); }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, _cx_keyraw rkey)
                            { return self->size && self->_hashx[_cx_memb(_bucket_)(self, &rkey).idx]; }

#ifndef _i_isset
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key key, i_val mapped);
    #if !defined i_no_emplace
        STC_API _cx_result  _cx_memb(_emplace_or_assign)(_cx_self* self, _cx_keyraw rkey, i_valraw rmapped);
    #endif

    STC_INLINE const _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, _cx_keyraw rkey) {
        chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
        assert(self->_hashx[b.idx]);
        return &self->table[b.idx].second;
    }
    STC_INLINE _cx_mapped*
    _cx_memb(_at_mut)(_cx_self* self, _cx_keyraw rkey)
        { return (_cx_mapped*)_cx_memb(_at)(self, rkey); }
#endif // !_i_isset

#if !defined i_no_clone
STC_INLINE void _cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
    if (self->table == other->table)
        return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(*other);
}

STC_INLINE _cx_value
_cx_memb(_value_clone)(_cx_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _cx_result
_cx_memb(_emplace)(_cx_self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, rkey);
    if (_res.inserted) {
        *_i_keyref(_res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY( _res.ref->second = i_valfrom(rmapped); )
    }
    return _res;
}
#endif // !i_no_emplace

STC_INLINE _cx_raw
_cx_memb(_value_toraw)(const _cx_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_LITERAL(_cx_raw){i_keyto((&val->first)), i_valto((&val->second))} );
}

STC_INLINE void
_cx_memb(_value_drop)(_cx_value* _val) {
    i_keydrop(_i_keyref(_val));
    _i_MAP_ONLY( i_valdrop((&_val->second)); )
}

STC_INLINE _cx_result
_cx_memb(_insert)(_cx_self* self, i_key _key _i_MAP_ONLY(, i_val _mapped)) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _cx_result
_cx_memb(_push)(_cx_self* self, _cx_value _val) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _cx_memb(_value_drop)(&_val);
    return _res;
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

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it = {self->table, self->table+self->bucket_count, self->_hashx};
    if (it._hx)
        while (*it._hx == 0)
            ++it.ref, ++it._hx;
    if (it.ref == it._end) it.ref = NULL;
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL}; }

STC_INLINE void
_cx_memb(_next)(_cx_iter* it) { 
    while ((++it->ref, *++it->_hx == 0)) ;
    if (it->ref == it->_end) it->ref = NULL;
}

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_keyraw rkey) {
    int64_t idx;
    if (self->size && self->_hashx[idx = _cx_memb(_bucket_)(self, &rkey).idx])
        return c_LITERAL(_cx_iter){self->table + idx, 
                                self->table + self->bucket_count,
                                self->_hashx + idx};
    return _cx_memb(_end)(self);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_keyraw rkey) {
    int64_t idx;
    if (self->size && self->_hashx[idx = _cx_memb(_bucket_)(self, &rkey).idx])
        return self->table + idx;
    return NULL;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_keyraw rkey)
    { return (_cx_value*)_cx_memb(_get)(self, rkey); }

STC_INLINE int
_cx_memb(_erase)(_cx_self* self, _cx_keyraw rkey) {
    if (self->size == 0)
        return 0;
    chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
    return self->_hashx[b.idx] ? _cx_memb(_erase_entry)(self, self->table + b.idx), 1 : 0;
}

STC_INLINE _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_memb(_erase_entry)(self, it.ref);
    if (*it._hx == 0)
        _cx_memb(_next)(&it);
    return it;
}

STC_INLINE bool
_cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    if (_cx_memb(_size)(self) != _cx_memb(_size)(other)) return false;
    for (_cx_iter i = _cx_memb(_begin)(self); i.ref; _cx_memb(_next)(&i)) {
        const _cx_keyraw _raw = i_keyto(_i_keyref(i.ref));
        if (!_cx_memb(_contains)(other, _raw)) return false;
    }
    return true;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

#ifndef CMAP_H_INCLUDED
STC_INLINE int64_t fastrange_1(uint64_t x, uint64_t n)
    { return (int64_t)((uint32_t)x*n >> 32); } // n < 2^32

STC_INLINE int64_t fastrange_2(uint64_t x, uint64_t n) 
    { return (int64_t)(x & (n - 1)); } // n power of 2.

STC_INLINE uint64_t next_power_of_2(uint64_t n) {
    n--;
    n |= n >>  1, n |= n >>  2;
    n |= n >>  4, n |= n >>  8;
    n |= n >> 16, n |= n >> 32;
    return n + 1;
}
#endif // CMAP_H_INCLUDED

STC_DEF _cx_self
_cx_memb(_with_capacity)(const _i_size cap) {
    _cx_self h = {0};
    _cx_memb(_reserve)(&h, cap);
    return h;
}

STC_INLINE void _cx_memb(_wipe_)(_cx_self* self) {
    if (self->size == 0)
        return;
    _cx_value* e = self->table, *end = e + self->bucket_count;
    uint8_t *hx = self->_hashx;
    for (; e != end; ++e)
        if (*hx++)
            _cx_memb(_value_drop)(e);
}

STC_DEF void _cx_memb(_drop)(_cx_self* self) {
    _cx_memb(_wipe_)(self);
    i_free(self->_hashx);
    i_free((void *) self->table);
}

STC_DEF void _cx_memb(_clear)(_cx_self* self) {
    _cx_memb(_wipe_)(self);
    self->size = 0;
    c_memset(self->_hashx, 0, self->bucket_count);
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

STC_DEF chash_bucket_t
_cx_memb(_bucket_)(const _cx_self* self, const _cx_keyraw* rkeyptr) {
    const uint64_t _hash = i_hash(rkeyptr);
    int64_t _cap = self->bucket_count;
    chash_bucket_t b = {c_PASTE(fastrange_,_i_expandby)(_hash, (uint64_t)_cap), (uint8_t)(_hash | 0x80)};
    const uint8_t* _hx = self->_hashx;
    while (_hx[b.idx]) {
        if (_hx[b.idx] == b.hx) {
            const _cx_keyraw _raw = i_keyto(_i_keyref(self->table + b.idx));
            if (i_eq((&_raw), rkeyptr))
                break;
        }
        if (++b.idx == _cap)
            b.idx = 0;
    }
    return b;
}

STC_DEF _cx_result
_cx_memb(_insert_entry_)(_cx_self* self, _cx_keyraw rkey) {
    _cx_result res = {NULL};
    if (self->size + 2 > (i_ssize)((float)self->bucket_count * (i_max_load_factor)))
        if (!_cx_memb(_reserve)(self, self->size*3/2))
            return res;

    chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
    res.ref = &self->table[b.idx];
    if ((res.inserted = !self->_hashx[b.idx])) {
        self->_hashx[b.idx] = b.hx;
        ++self->size;
    }
    return res;
}

#if !defined i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self m) {
    if (m.table) {
        _cx_value *t = (_cx_value *)i_malloc(c_sizeof(_cx_value)*m.bucket_count),
                  *dst = t, *m_end = m.table + m.bucket_count;
        uint8_t *h = (uint8_t *)c_memcpy(i_malloc(m.bucket_count + 1), m._hashx, m.bucket_count + 1);
        if (!(t && h)) 
            { i_free(t), i_free(h), t = 0, h = 0, m.bucket_count = 0; }
        else
            for (; m.table != m_end; ++m.table, ++m._hashx, ++dst)
                if (*m._hashx)
                    *dst = _cx_memb(_value_clone)(*m.table);
        m.table = t, m._hashx = h;
    }
    return m;
}
#endif

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const _i_size newcap) {
    const i_ssize _oldbuckets = self->bucket_count, _newcap = (i_ssize)newcap;
    if (_newcap != self->size && _newcap <= _oldbuckets)
        return true;
    i_ssize _nbuckets = (i_ssize)((float)_newcap / (i_max_load_factor)) + 4;
    #if _i_expandby == 2
    _nbuckets = (i_ssize)next_power_of_2(_nbuckets);
    #else
    _nbuckets |= 1;
    #endif
    _cx_self m = {
        (_cx_value *)i_malloc(c_sizeof(_cx_value)*_nbuckets),
        (uint8_t *)i_calloc(_nbuckets + 1, 1),
        self->size, _nbuckets,
    };
    bool ok = m.table && m._hashx;
    if (ok) {  /* Rehash: */
        m._hashx[_nbuckets] = 0xff;
        const _cx_value* e = self->table;
        const uint8_t* h = self->_hashx;
        for (i_ssize i = 0; i < _oldbuckets; ++i, ++e) if (*h++) {
            _cx_keyraw r = i_keyto(_i_keyref(e));
            chash_bucket_t b = _cx_memb(_bucket_)(&m, &r);
            m.table[b.idx] = *e;
            m._hashx[b.idx] = b.hx;
        }
        c_swap(_cx_self, self, &m);
    }
    i_free(m._hashx);
    i_free(m.table);
    return ok;
}

STC_DEF void
_cx_memb(_erase_entry)(_cx_self* self, _cx_value* _val) {
    i_ssize i = (i_ssize)(_val - self->table), j = i, k;
    const i_ssize _cap = self->bucket_count;
    _cx_value* _slot = self->table;
    uint8_t* _hashx = self->_hashx;
    _cx_memb(_value_drop)(_val);
    for (;;) { /* delete without leaving tombstone */
        if (++j == _cap)
            j = 0;
        if (! _hashx[j])
            break;
        const _cx_keyraw _raw = i_keyto(_i_keyref(_slot + j));
        k = (i_ssize)c_PASTE(fastrange_,_i_expandby)(i_hash((&_raw)), (uint64_t)_cap);
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */
            _slot[i] = _slot[j], _hashx[i] = _hashx[j], i = j;
    }
    _hashx[i] = 0;
    --self->size;
}

#endif // i_implement
#undef i_max_load_factor
#undef _i_size
#undef _i_isset
#undef _i_ismap
#undef _i_ishash
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CMAP_H_INCLUDED
#include "priv/template2.h"
