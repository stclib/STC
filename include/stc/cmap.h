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

// Unordered set/map - implemented as closed hashing with linear probing and no tombstones.
/*
#include <stdio.h>

#define i_tag ichar  // Map int => char
#define i_key int
#define i_val char
#include <stc/cmap.h>

int main(void) {
    c_autovar (cmap_ichar m = cmap_ichar_init(), cmap_ichar_del(&m))
    {
        cmap_ichar_emplace(&m, 5, 'a');
        cmap_ichar_emplace(&m, 8, 'b');
        cmap_ichar_emplace(&m, 12, 'c');

        cmap_ichar_value_t* v = cmap_ichar_get(&m, 10); // NULL
        char val = *cmap_ichar_at(&m, 5);               // 'a'
        cmap_ichar_emplace_or_assign(&m, 5, 'd');       // update
        cmap_ichar_erase(&m, 8);

        c_foreach (i, cmap_ichar, m)
            printf("map %d: %c\n", i.ref->first, i.ref->second);
    }
}
*/

#ifndef CMAP_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define _cmap_inits {NULL, NULL, 0, 0, 0.85f}
typedef struct      { MAP_SIZE_T idx; uint_fast8_t hx; } chash_bucket_t;
#endif // CMAP_H_INCLUDED

#ifndef i_prefix
#define i_prefix cmap_
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
_cx_deftypes(_c_chash_types, _cx_self, i_key, i_val, cx_MAP_ONLY, cx_SET_ONLY);
#endif

cx_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )

typedef i_keyraw _cx_rawkey;
typedef i_valraw _cx_memb(_rawmapped_t);
typedef cx_SET_ONLY( i_keyraw )
        cx_MAP_ONLY( struct { i_keyraw first;
                              i_valraw second; } )
_cx_rawvalue;

STC_API _cx_self        _cx_memb(_with_capacity)(size_t cap);
STC_API _cx_self        _cx_memb(_clone)(_cx_self map);
STC_API void            _cx_memb(_del)(_cx_self* self);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API void            _cx_memb(_reserve)(_cx_self* self, size_t capacity);
STC_API chash_bucket_t  _cx_memb(_bucket_)(const _cx_self* self, const _cx_rawkey* rkeyptr);
STC_API _cx_result    _cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey);
STC_API void            _cx_memb(_erase_entry)(_cx_self* self, _cx_value* val);

STC_INLINE _cx_self     _cx_memb(_init)(void) { return c_make(_cx_self)_cmap_inits; }
STC_INLINE void         _cx_memb(_shrink_to_fit)(_cx_self* self) { _cx_memb(_reserve)(self, self->size); }
STC_INLINE void         _cx_memb(_max_load_factor)(_cx_self* self, float ml) {self->max_load_factor = ml; }
STC_INLINE bool         _cx_memb(_empty)(_cx_self m) { return m.size == 0; }
STC_INLINE size_t       _cx_memb(_size)(_cx_self m) { return m.size; }
STC_INLINE size_t       _cx_memb(_bucket_count)(_cx_self map) { return map.bucket_count; }
STC_INLINE size_t       _cx_memb(_capacity)(_cx_self map)
                            { return (size_t) (map.bucket_count * map.max_load_factor); }
STC_INLINE void         _cx_memb(_swap)(_cx_self *map1, _cx_self *map2) {c_swap(_cx_self, *map1, *map2); }
STC_INLINE bool         _cx_memb(_contains)(const _cx_self* self, i_keyraw rkey)
                            { return self->size && self->_hashx[_cx_memb(_bucket_)(self, &rkey).idx]; }

cx_MAP_ONLY(
    STC_API _cx_result _cx_memb(_insert_or_assign)(_cx_self* self, i_key _key, i_val _mapped);
    STC_API _cx_result _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped);

    STC_INLINE _cx_result  /* short-form, like operator[]: */
    _cx_memb(_put)(_cx_self* self, i_key key, i_val mapped) {
        return _cx_memb(_insert_or_assign)(self, key, mapped);
    }

    STC_INLINE _cx_mapped*
    _cx_memb(_at)(const _cx_self* self, i_keyraw rkey) {
        chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
        assert(self->_hashx[b.idx]);
        return &self->table[b.idx].second;
    }
)

STC_INLINE void
_cx_memb(_value_clone)(_cx_value* _dst, _cx_value* _val) {
    *cx_keyref(_dst) = i_keyfrom(i_keyto(cx_keyref(_val)));
    cx_MAP_ONLY( _dst->second = i_valfrom(i_valto(&_val->second)); )
}

STC_INLINE _cx_rawvalue
_cx_memb(_value_toraw)(_cx_value* val) {
    return cx_SET_ONLY( i_keyto(val) )
           cx_MAP_ONLY( c_make(_cx_rawvalue){i_keyto(&val->first), i_valto(&val->second)} );
}

STC_INLINE void
_cx_memb(_value_del)(_cx_value* _val) {
    i_keydel(cx_keyref(_val));
    cx_MAP_ONLY( i_valdel(&_val->second); )
}

STC_INLINE _cx_result
_cx_memb(_emplace)(_cx_self* self, i_keyraw rkey cx_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, rkey);
    if (_res.inserted) {
        *cx_keyref(_res.ref) = i_keyfrom(rkey);
        cx_MAP_ONLY( _res.ref->second = i_valfrom(rmapped); )
    }
    return _res;
}

STC_INLINE _cx_result
_cx_memb(_insert)(_cx_self* self, i_key _key cx_MAP_ONLY(, i_val _mapped)) {
    _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto(&_key));
    if (_res.inserted) { *cx_keyref(_res.ref) = _key; cx_MAP_ONLY( _res.ref->second = _mapped; )}
    else               { i_keydel(&_key); cx_MAP_ONLY( i_valdel(&_mapped); )}
    return _res;
}

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, i_keyraw rkey) {
    _cx_size idx;
    if (!(self->size && self->_hashx[idx = _cx_memb(_bucket_)(self, &rkey).idx]))
        idx = self->bucket_count;
    return c_make(_cx_iter){self->table+idx, self->_hashx+idx};
}

STC_INLINE _cx_value*
_cx_memb(_get)(const _cx_self* self, i_keyraw rkey) {
    _cx_size idx;
    return self->size && self->_hashx[idx = _cx_memb(_bucket_)(self, &rkey).idx] ?
           self->table + idx : NULL;
}

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_iter it = {self->table, self->_hashx};
    if (it._hx) while (*it._hx == 0) ++it.ref, ++it._hx;
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self)
    { return c_make(_cx_iter){self->table + self->bucket_count}; }

STC_INLINE void
_cx_memb(_next)(_cx_iter* it)
    { while ((++it->ref, *++it->_hx == 0)) ; }

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    // UB if n > elements left
    while (n--) _cx_memb(_next)(&it);
    return it;
}

STC_INLINE size_t
_cx_memb(_erase)(_cx_self* self, i_keyraw rkey) {
    if (self->size == 0) return 0;
    chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
    return self->_hashx[b.idx] ? _cx_memb(_erase_entry)(self, self->table + b.idx), 1 : 0;
}

STC_INLINE _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_memb(_erase_entry)(self, it.ref);
    if (*it._hx == 0) _cx_memb(_next)(&it);
    return it;
}

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

#ifndef CMAP_H_INCLUDED
STC_INLINE uint64_t c_default_hash(const void *key, size_t len) {
    const char* str = (const char*)key, *e = str + len;
    uint64_t h = 0xb5ad4eceda1ce2a9;
    while (str != e) h = (_c_rotl(h, 4) ^ (h << 13)) + *str++;
    return h ^ (h >> 15);
}

//STC_INLINE size_t fastrange_uint64_t(uint64_t x, uint64_t n)
//    { uint64_t lo, hi; c_umul128(x, n, &lo, &hi); return hi; }
#define fastrange_uint32_t(x, n) (uint32_t)((uint32_t)(x)*(uint64_t)(n) >> 32)
#define chash_index_(h, entryPtr) ((entryPtr) - (h).table)
#endif // CMAP_H_INCLUDED

STC_DEF _cx_self
_cx_memb(_with_capacity)(size_t cap) {
    _cx_self h = _cmap_inits;
    _cx_memb(_reserve)(&h, cap);
    return h;
}

STC_INLINE void _cx_memb(_wipe_)(_cx_self* self) {
    if (self->size == 0) return;
    _cx_value* e = self->table, *end = e + self->bucket_count;
    uint8_t *hx = self->_hashx;
    for (; e != end; ++e) if (*hx++) _cx_memb(_value_del)(e);
}

STC_DEF void _cx_memb(_del)(_cx_self* self) {
    _cx_memb(_wipe_)(self);
    c_free(self->_hashx);
    c_free((void *) self->table);
}

STC_DEF void _cx_memb(_clear)(_cx_self* self) {
    _cx_memb(_wipe_)(self);
    self->size = 0;
    memset(self->_hashx, 0, self->bucket_count);
}

STC_INLINE void _cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->table == other.table) return;
    _cx_memb(_del)(self); *self = _cx_memb(_clone)(other);
}

cx_MAP_ONLY(
    STC_DEF _cx_result
    _cx_memb(_insert_or_assign)(_cx_self* self, i_key _key, i_val _mapped) {
        _cx_result _res = _cx_memb(_insert_entry_)(self, i_keyto(&_key));
        if (_res.inserted) _res.ref->first = _key;
        else { i_keydel(&_key); i_valdel(&_res.ref->second); }
        _res.ref->second = _mapped; return _res;
    }

    STC_DEF _cx_result
    _cx_memb(_emplace_or_assign)(_cx_self* self, i_keyraw rkey, i_valraw rmapped) {
        _cx_result _res = _cx_memb(_insert_entry_)(self, rkey);
        if (_res.inserted) _res.ref->first = i_keyfrom(rkey);
        else i_valdel(&_res.ref->second);
        _res.ref->second = i_valfrom(rmapped); return _res;
    }
)

STC_DEF chash_bucket_t
_cx_memb(_bucket_)(const _cx_self* self, const _cx_rawkey* rkeyptr) {
    const uint64_t _hash = i_hash(rkeyptr, sizeof *rkeyptr);
    uint_fast8_t _hx; _cx_size _cap = self->bucket_count;
    chash_bucket_t b = {c_PASTE(fastrange_,MAP_SIZE_T)(_hash, _cap), (uint_fast8_t)(_hash | 0x80)};
    const uint8_t* _hashx = self->_hashx;
    while ((_hx = _hashx[b.idx])) {
        if (_hx == b.hx) {
            _cx_rawkey _raw = i_keyto(cx_keyref(self->table + b.idx));
            if (i_equ(&_raw, rkeyptr)) break;
        }
        if (++b.idx == _cap) b.idx = 0;
    }
    return b;
}

STC_DEF _cx_result
_cx_memb(_insert_entry_)(_cx_self* self, i_keyraw rkey) {
    if (self->size + 1 >= (_cx_size) (self->bucket_count * self->max_load_factor))
        _cx_memb(_reserve)(self, 8 + (self->size*13ull >> 3));
    chash_bucket_t b = _cx_memb(_bucket_)(self, &rkey);
    _cx_result res = {&self->table[b.idx], !self->_hashx[b.idx]};
    if (res.inserted) {
        self->_hashx[b.idx] = b.hx;
        ++self->size;
    }
    return res;
}

STC_DEF _cx_self
_cx_memb(_clone)(_cx_self m) {
    _cx_self clone = {
        c_new_n(_cx_value, m.bucket_count),
        (uint8_t *) memcpy(c_malloc(m.bucket_count + 1), m._hashx, m.bucket_count + 1),
        m.size, m.bucket_count,
        m.max_load_factor
    };
    _cx_value *e = m.table, *end = e + m.bucket_count, *dst = clone.table;
    for (uint8_t *hx = m._hashx; e != end; ++hx, ++e, ++dst)
        if (*hx) _cx_memb(_value_clone)(dst, e);
    return clone;
}

STC_DEF void
_cx_memb(_reserve)(_cx_self* self, size_t _newcap) {
    if (_newcap < self->size) return;
    size_t _oldcap = self->bucket_count;
    _newcap = (size_t) (2 + _newcap / self->max_load_factor) | 1;
    _cx_self _tmp = {
        c_new_n(_cx_value, _newcap),
        (uint8_t *) c_calloc(_newcap + 1, sizeof(uint8_t)),
        self->size, (_cx_size) _newcap,
        self->max_load_factor
    };
    /* Rehash: */
    _tmp._hashx[_newcap] = 0xff; c_swap(_cx_self, *self, _tmp);
    _cx_value* e = _tmp.table, *_slot = self->table;
    uint8_t* _hashx = self->_hashx;
    for (size_t i = 0; i < _oldcap; ++i, ++e)
        if (_tmp._hashx[i]) {
            _cx_rawkey _raw = i_keyto(cx_keyref(e));
            chash_bucket_t b = _cx_memb(_bucket_)(self, &_raw);
            _slot[b.idx] = *e;
            _hashx[b.idx] = (uint8_t) b.hx;
        }
    c_free(_tmp._hashx);
    c_free((void *) _tmp.table);
}

STC_DEF void
_cx_memb(_erase_entry)(_cx_self* self, _cx_value* _val) {
    size_t i = chash_index_(*self, _val), j = i, k, _cap = self->bucket_count;
    _cx_value* _slot = self->table;
    uint8_t* _hashx = self->_hashx;
    _cx_memb(_value_del)(&_slot[i]);
    for (;;) { /* delete without leaving tombstone */
        if (++j == _cap) j = 0;
        if (! _hashx[j])
            break;
        _cx_rawkey _raw = i_keyto(cx_keyref(_slot + j));
        k = c_PASTE(fastrange_,MAP_SIZE_T)(i_hash(&_raw, sizeof _raw), _cap);
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */
            _slot[i] = _slot[j], _hashx[i] = _hashx[j], i = j;
    }
    _hashx[i] = 0;
    --self->size;
}

#endif // TEMPLATED IMPLEMENTATION
#undef i_isset
#undef cx_keyref
#undef cx_MAP_ONLY
#undef cx_SET_ONLY
#include "template.h"
#define CMAP_H_INCLUDED
