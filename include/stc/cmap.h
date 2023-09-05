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
#include "priv/linkage.h"

#ifndef CMAP_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>
struct chash_slot { uint8_t hashx; };
#endif // CMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix cmap_
  #define _i_ismap
  #define _i_MAP_ONLY c_true
  #define _i_SET_ONLY c_false
  #define _i_keyref(vp) (&(vp)->first)
#else
  #define _i_isset
  #define _i_MAP_ONLY c_false
  #define _i_SET_ONLY c_true
  #define _i_keyref(vp) (vp)
#endif
#define _i_ishash
#include "priv/template.h"
#ifndef i_is_forward
  _cx_DEFTYPES(_c_chash_types, _cx_Self, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )

typedef i_keyraw _cx_keyraw;
typedef i_valraw _cx_MEMB(_rmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first;
                              i_valraw second; } )
_cx_raw;

STC_API _cx_Self        _cx_MEMB(_with_capacity)(intptr_t cap);
#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self map);
#endif
STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API void            _cx_MEMB(_clear)(_cx_Self* self);
STC_API bool            _cx_MEMB(_reserve)(_cx_Self* self, intptr_t capacity);
STC_API _cx_result      _cx_MEMB(_bucket_)(const _cx_Self* self, const _cx_keyraw* rkeyptr);
STC_API _cx_result      _cx_MEMB(_insert_entry_)(_cx_Self* self, _cx_keyraw rkey);
STC_API void            _cx_MEMB(_erase_entry)(_cx_Self* self, _cx_value* val);
STC_API float           _cx_MEMB(_max_load_factor)(const _cx_Self* self);
STC_API intptr_t        _cx_MEMB(_capacity)(const _cx_Self* map);

STC_INLINE _cx_Self     _cx_MEMB(_init)(void) { _cx_Self map = {0}; return map; }
STC_INLINE void         _cx_MEMB(_shrink_to_fit)(_cx_Self* self) { _cx_MEMB(_reserve)(self, (intptr_t)self->size); }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* map) { return !map->size; }
STC_INLINE intptr_t     _cx_MEMB(_size)(const _cx_Self* map) { return (intptr_t)map->size; }
STC_INLINE intptr_t     _cx_MEMB(_bucket_count)(_cx_Self* map) { return map->bucket_count; }
STC_INLINE bool         _cx_MEMB(_contains)(const _cx_Self* self, _cx_keyraw rkey)
                            { return self->size && !_cx_MEMB(_bucket_)(self, &rkey).inserted; }

#ifdef _i_ismap
    STC_API _cx_result _cx_MEMB(_insert_or_assign)(_cx_Self* self, i_key key, i_val mapped);
    #if !defined i_no_emplace
        STC_API _cx_result  _cx_MEMB(_emplace_or_assign)(_cx_Self* self, _cx_keyraw rkey, i_valraw rmapped);
    #endif

    STC_INLINE const _cx_mapped*
    _cx_MEMB(_at)(const _cx_Self* self, _cx_keyraw rkey) {
        _cx_result b = _cx_MEMB(_bucket_)(self, &rkey);
        c_assert(!b.inserted);
        return &b.ref->second;
    }

    STC_INLINE _cx_mapped*
    _cx_MEMB(_at_mut)(_cx_Self* self, _cx_keyraw rkey)
        { return (_cx_mapped*)_cx_MEMB(_at)(self, rkey); }
#endif // _i_ismap

#if !defined i_no_clone
STC_INLINE void _cx_MEMB(_copy)(_cx_Self *self, const _cx_Self* other) {
    if (self->data == other->data)
        return;
    _cx_MEMB(_drop)(self);
    *self = _cx_MEMB(_clone)(*other);
}

STC_INLINE _cx_value
_cx_MEMB(_value_clone)(_cx_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _cx_result
_cx_MEMB(_emplace)(_cx_Self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result _res = _cx_MEMB(_insert_entry_)(self, rkey);
    if (_res.inserted) {
        *_i_keyref(_res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY( _res.ref->second = i_valfrom(rmapped); )
    }
    return _res;
}

#ifdef _i_ismap
    STC_INLINE _cx_result
    _cx_MEMB(_emplace_key)(_cx_Self* self, _cx_keyraw rkey) {
        _cx_result _res = _cx_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted)
            _res.ref->first = i_keyfrom(rkey);
        return _res;
    }
#endif // _i_ismap
#endif // !i_no_emplace

STC_INLINE _cx_raw _cx_MEMB(_value_toraw)(const _cx_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_LITERAL(_cx_raw){i_keyto((&val->first)), i_valto((&val->second))} );
}

STC_INLINE void _cx_MEMB(_value_drop)(_cx_value* _val) {
    i_keydrop(_i_keyref(_val));
    _i_MAP_ONLY( i_valdrop((&_val->second)); )
}

STC_INLINE _cx_result
_cx_MEMB(_insert)(_cx_Self* self, i_key _key _i_MAP_ONLY(, i_val _mapped)) {
    _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _cx_value* _cx_MEMB(_push)(_cx_Self* self, _cx_value _val) {
    _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _cx_MEMB(_value_drop)(&_val);
    return _res.ref;
}

STC_INLINE void _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n) {
    while (n--) 
#if defined _i_isset && defined i_no_emplace
        _cx_MEMB(_insert)(self, *raw++);
#elif defined _i_isset
        _cx_MEMB(_emplace)(self, *raw++);
#elif defined i_no_emplace
        _cx_MEMB(_insert_or_assign)(self, raw->first, raw->second), ++raw;
#else
        _cx_MEMB(_emplace_or_assign)(self, raw->first, raw->second), ++raw;
#endif
}

STC_INLINE _cx_Self _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
    { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_API _cx_iter _cx_MEMB(_begin)(const _cx_Self* self);

STC_INLINE _cx_iter _cx_MEMB(_end)(const _cx_Self* self)
    { return c_LITERAL(_cx_iter){NULL}; }

STC_INLINE void _cx_MEMB(_next)(_cx_iter* it) { 
    while ((++it->ref, (++it->sref)->hashx == 0)) ;
    if (it->ref == it->_end) it->ref = NULL;
}

STC_INLINE _cx_iter _cx_MEMB(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_MEMB(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_MEMB(_find)(const _cx_Self* self, _cx_keyraw rkey) {
    _cx_result b;
    if (self->size && !(b = _cx_MEMB(_bucket_)(self, &rkey)).inserted)
        return c_LITERAL(_cx_iter){b.ref, 
                                   self->data + self->bucket_count,
                                   self->slot + (b.ref - self->data)};
    return _cx_MEMB(_end)(self);
}

STC_INLINE const _cx_value*
_cx_MEMB(_get)(const _cx_Self* self, _cx_keyraw rkey) {
    _cx_result b;
    if (self->size && !(b = _cx_MEMB(_bucket_)(self, &rkey)).inserted)
        return b.ref;
    return NULL;
}

STC_INLINE _cx_value*
_cx_MEMB(_get_mut)(_cx_Self* self, _cx_keyraw rkey)
    { return (_cx_value*)_cx_MEMB(_get)(self, rkey); }

STC_INLINE int
_cx_MEMB(_erase)(_cx_Self* self, _cx_keyraw rkey) {
    _cx_result b;
    if (self->size && !(b = _cx_MEMB(_bucket_)(self, &rkey)).inserted)
        { _cx_MEMB(_erase_entry)(self, b.ref); return 1; }
    return 0;
}

STC_INLINE _cx_iter
_cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it) {
    _cx_MEMB(_erase_entry)(self, it.ref);
    if (it.sref->hashx == 0)
        _cx_MEMB(_next)(&it);
    return it;
}

STC_INLINE bool
_cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    if (_cx_MEMB(_size)(self) != _cx_MEMB(_size)(other)) return false;
    for (_cx_iter i = _cx_MEMB(_begin)(self); i.ref; _cx_MEMB(_next)(&i)) {
        const _cx_keyraw _raw = i_keyto(_i_keyref(i.ref));
        if (!_cx_MEMB(_contains)(other, _raw)) return false;
    }
    return true;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)
#ifndef i_max_load_factor
  #define i_max_load_factor 0.80f
#endif
#define fastrange_2(x, n) (intptr_t)((x) & (size_t)((n) - 1)) // n power of 2.

STC_DEF _cx_iter _cx_MEMB(_begin)(const _cx_Self* self) {
    _cx_iter it = {self->data, self->data+self->bucket_count, self->slot};
    if (it.sref)
        while (it.sref->hashx == 0)
            ++it.ref, ++it.sref;
    if (it.ref == it._end) it.ref = NULL;
    return it;
}

STC_DEF float _cx_MEMB(_max_load_factor)(const _cx_Self* self) {
    return (float)(i_max_load_factor);
}

STC_DEF intptr_t _cx_MEMB(_capacity)(const _cx_Self* map) {
    return (intptr_t)((float)map->bucket_count * (i_max_load_factor));
}

STC_DEF _cx_Self _cx_MEMB(_with_capacity)(const intptr_t cap) {
    _cx_Self map = {0};
    _cx_MEMB(_reserve)(&map, cap);
    return map;
}

STC_INLINE void _cx_MEMB(_wipe_)(_cx_Self* self) {
    if (self->size == 0)
        return;
    _cx_value* d = self->data, *_end = d + self->bucket_count;
    struct chash_slot* s = self->slot;
    for (; d != _end; ++d)
        if ((s++)->hashx)
            _cx_MEMB(_value_drop)(d);
}

STC_DEF void _cx_MEMB(_drop)(_cx_Self* self) {
    _cx_MEMB(_wipe_)(self);
    i_free(self->slot);
    i_free(self->data);
}

STC_DEF void _cx_MEMB(_clear)(_cx_Self* self) {
    _cx_MEMB(_wipe_)(self);
    self->size = 0;
    c_memset(self->slot, 0, c_sizeof(struct chash_slot)*self->bucket_count);
}

#ifdef _i_ismap
    STC_DEF _cx_result
    _cx_MEMB(_insert_or_assign)(_cx_Self* self, i_key _key, i_val _mapped) {
        _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto((&_key)));
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
    _cx_MEMB(_emplace_or_assign)(_cx_Self* self, _cx_keyraw rkey, i_valraw rmapped) {
        _cx_result _res = _cx_MEMB(_insert_entry_)(self, rkey);
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
#endif // _i_ismap

STC_DEF _cx_result
_cx_MEMB(_bucket_)(const _cx_Self* self, const _cx_keyraw* rkeyptr) {
    const uint64_t _hash = i_hash(rkeyptr);
    intptr_t _cap = self->bucket_count;
    intptr_t _idx = fastrange_2(_hash, _cap);
    _cx_result b = {NULL, true, (uint8_t)(_hash | 0x80)};
    const struct chash_slot* s = self->slot;
    while (s[_idx].hashx) {
        if (s[_idx].hashx == b.hashx) {
            const _cx_keyraw _raw = i_keyto(_i_keyref(self->data + _idx));
            if (i_eq((&_raw), rkeyptr)) {
                b.inserted = false; 
                break;
            }
        }
        if (++_idx == _cap) _idx = 0;
    }
    b.ref = self->data + _idx;
    return b;
}

STC_DEF _cx_result
_cx_MEMB(_insert_entry_)(_cx_Self* self, _cx_keyraw rkey) {
    if (self->size >= (intptr_t)((float)self->bucket_count * (i_max_load_factor)))
        if (!_cx_MEMB(_reserve)(self, (intptr_t)(self->size*3/2 + 2)))
            return c_LITERAL(_cx_result){NULL};

    _cx_result b = _cx_MEMB(_bucket_)(self, &rkey);
    if (b.inserted) {
        self->slot[b.ref - self->data].hashx = b.hashx;
        ++self->size;
    }
    return b;
}

#if !defined i_no_clone
STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self m) {
    if (m.data) {
        _cx_value *d = (_cx_value *)i_malloc(c_sizeof(_cx_value)*m.bucket_count),
                  *_dst = d, *_end = m.data + m.bucket_count;
        const intptr_t _mem = c_sizeof(struct chash_slot)*(m.bucket_count + 1);
        struct chash_slot *s = (struct chash_slot *)c_memcpy(i_malloc(_mem), m.slot, _mem);
        if (!(d && s)) 
            { i_free(d), i_free(s), d = 0, s = 0, m.bucket_count = 0; }
        else
            for (; m.data != _end; ++m.data, ++m.slot, ++_dst)
                if (m.slot->hashx)
                    *_dst = _cx_MEMB(_value_clone)(*m.data);
        m.data = d, m.slot = s;
    }
    return m;
}
#endif

STC_DEF bool
_cx_MEMB(_reserve)(_cx_Self* self, const intptr_t _newcap) {
    const intptr_t _oldbucks = self->bucket_count;
    if (_newcap != self->size && _newcap <= _oldbucks)
        return true;
    intptr_t _newbucks = (intptr_t)((float)_newcap / (i_max_load_factor)) + 4;
    _newbucks = stc_nextpow2(_newbucks);
    _cx_Self m = {
        (_cx_value *)i_malloc(_newbucks*c_sizeof(_cx_value)),
        (struct chash_slot *)i_calloc(_newbucks + 1, c_sizeof(struct chash_slot)),
        self->size, _newbucks
    };
    bool ok = m.data && m.slot;
    if (ok) {  // Rehash:
        m.slot[_newbucks].hashx = 0xff;
        const _cx_value* d = self->data;
        const struct chash_slot* s = self->slot;
        for (intptr_t i = 0; i < _oldbucks; ++i, ++d) if ((s++)->hashx) {
            _cx_keyraw r = i_keyto(_i_keyref(d));
            _cx_result b = _cx_MEMB(_bucket_)(&m, &r);
            m.slot[b.ref - m.data].hashx = b.hashx;
            *b.ref = *d; // move
        }
        c_swap(_cx_Self, self, &m);
    }
    i_free(m.slot);
    i_free(m.data);
    return ok;
}

STC_DEF void
_cx_MEMB(_erase_entry)(_cx_Self* self, _cx_value* _val) {
    _cx_value* d = self->data;
    struct chash_slot* s = self->slot;
    intptr_t i = _val - d, j = i, k;
    const intptr_t _cap = self->bucket_count;
    _cx_MEMB(_value_drop)(_val);
    for (;;) { // delete without leaving tombstone
        if (++j == _cap) j = 0;
        if (! s[j].hashx)
            break;
        const _cx_keyraw _raw = i_keyto(_i_keyref(d + j));
        k = fastrange_2(i_hash((&_raw)), _cap);
        if ((j < i) ^ (k <= i) ^ (k > j)) { // is k outside (i, j]?
            d[i] = d[j];
            s[i] = s[j];
            i = j;
        }
    }
    s[i].hashx = 0;
    --self->size;
}
#endif // i_implement
#undef i_max_load_factor
#undef _i_isset
#undef _i_ismap
#undef _i_ishash
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CMAP_H_INCLUDED
#include "priv/template2.h"
