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

// Unordered set/map - implemented with the robin-hood hashing scheme.
/*
#include <stdio.h>

#define T icmap, int, char
#include <stc/hashmap.h>

int main(void) {
    icmap m = {0};
    icmap_emplace(&m, 5, 'a');
    icmap_emplace(&m, 8, 'b');
    icmap_emplace(&m, 12, 'c');

    icmap_value* v = icmap_get(&m, 10);   // NULL
    char val = *icmap_at(&m, 5);          // 'a'
    icmap_emplace_or_assign(&m, 5, 'd');  // update
    icmap_erase(&m, 8);

    for (c_each(i, icmap, m))
        printf("map %d: %c\n", i.ref->first, i.ref->second);

    icmap_drop(&m);
}
*/
#include "priv/linkage.h"
#include "types.h"

#ifndef STC_HMAP_H_INCLUDED
#define STC_HMAP_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#define _hashmask 0x3fU
#define _distmask 0x3ffU
struct hmap_meta { uint16_t hashx:6, dist:10; }; // dist: 0=empty, 1=PSL 0, 2=PSL 1, ...
#endif // STC_HMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix hmap_
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
#define _i_is_hash
#include "priv/template.h"
#ifndef i_declared
  _c_DEFTYPES(_declare_htable, Self, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY, _i_aux_def);
#endif

_i_MAP_ONLY( struct _m_value {
    _m_key first;
    _m_mapped second;
}; )

typedef i_keyraw _m_keyraw;
typedef i_valraw _m_rmapped;
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { _m_keyraw first;
                              _m_rmapped second; } )
_m_raw;

#ifndef i_no_clone
STC_API Self            _c_MEMB(_clone)(Self map);
#endif
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API bool            _c_MEMB(_reserve)(Self* self, isize capacity);
STC_API void            _c_MEMB(_erase_entry)(Self* self, _m_value* val);
STC_API float           _c_MEMB(_max_load_factor)(const Self* self);
STC_API isize           _c_MEMB(_capacity)(const Self* map);
STC_API _m_result       _c_MEMB(_bucket_lookup_)(const Self* self, const _m_keyraw* rkeyptr);
STC_API _m_result       _c_MEMB(_bucket_insert_)(const Self* self, const _m_keyraw* rkeyptr);

STC_INLINE bool         _c_MEMB(_is_empty)(const Self* map) { return !map->size; }
STC_INLINE isize        _c_MEMB(_size)(const Self* map) { return (isize)map->size; }
STC_INLINE isize        _c_MEMB(_bucket_count)(Self* map) { return map->bucket_count; }
STC_INLINE bool         _c_MEMB(_contains)(const Self* self, _m_keyraw rkey)
                            { return self->size && _c_MEMB(_bucket_lookup_)(self, &rkey).ref; }
STC_INLINE void         _c_MEMB(_shrink_to_fit)(Self* self)
                            { _c_MEMB(_reserve)(self, (isize)self->size); }

#ifndef i_max_load_factor
  #define i_max_load_factor 0.80f
#endif

STC_INLINE _m_result
_c_MEMB(_insert_entry_)(Self* self, _m_keyraw rkey) {
    if (self->size >= (isize)((float)self->bucket_count * (i_max_load_factor)))
        if (!_c_MEMB(_reserve)(self, (isize)(self->size*3/2 + 2)))
            return c_literal(_m_result){0};

    _m_result res = _c_MEMB(_bucket_insert_)(self, &rkey);
    self->size += res.inserted;
    return res;
}

#ifdef _i_is_map
    STC_API _m_result _c_MEMB(_insert_or_assign)(Self* self, _m_key key, _m_mapped mapped);
    #ifndef i_no_emplace
    STC_API _m_result _c_MEMB(_emplace_or_assign)(Self* self, _m_keyraw rkey, _m_rmapped rmapped);
    #endif

    STC_INLINE const _m_mapped* _c_MEMB(_at)(const Self* self, _m_keyraw rkey) {
        _m_result res = _c_MEMB(_bucket_lookup_)(self, &rkey);
        c_assert(res.ref);
        return &res.ref->second;
    }

    STC_INLINE _m_mapped* _c_MEMB(_at_mut)(Self* self, _m_keyraw rkey)
        { return (_m_mapped*)_c_MEMB(_at)(self, rkey); }
#endif // _i_is_map

#ifndef i_no_clone
    STC_INLINE void _c_MEMB(_copy)(Self *self, const Self* other) {
        if (self == other)
            return;
        _c_MEMB(_drop)(self);
        *self = _c_MEMB(_clone)(*other);
    }

    STC_INLINE _m_value _c_MEMB(_value_clone)(const Self* self, _m_value _val) {
        (void)self;
        *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
        _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
        return _val;
    }
#endif // !i_no_clone

#ifndef i_no_emplace
    STC_INLINE _m_result
    _c_MEMB(_emplace)(Self* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped)) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted) {
            *_i_keyref(_res.ref) = i_keyfrom(rkey);
            _i_MAP_ONLY( _res.ref->second = i_valfrom(rmapped); )
        }
        return _res;
    }
#endif // !i_no_emplace

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* val) {
    return _i_SET_ONLY( i_keytoraw(val) )
           _i_MAP_ONLY( c_literal(_m_raw){i_keytoraw((&val->first)), i_valtoraw((&val->second))} );
}

STC_INLINE void _c_MEMB(_value_drop)(const Self* self, _m_value* _val) {
    (void)self;
    i_keydrop(_i_keyref(_val));
    _i_MAP_ONLY( i_valdrop((&_val->second)); )
}

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->bucket_count = self->size = 0;
    self->meta = NULL; self->table = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

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

STC_API _m_iter _c_MEMB(_begin)(const Self* self);

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it) {
    while ((++it->ref, (++it->_mref)->dist == 0)) ;
    if (it->ref == it->_end) it->ref = NULL;
}

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref) _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_keyraw rkey) {
    _m_value* ref;
    if (self->size != 0 && (ref = _c_MEMB(_bucket_lookup_)(self, &rkey).ref) != NULL)
        return c_literal(_m_iter){ref,
                                  &self->table[self->bucket_count],
                                  &self->meta[ref - self->table]};
    return _c_MEMB(_end)(self);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const Self* self, _m_keyraw rkey) {
    return self->size ? _c_MEMB(_bucket_lookup_)(self, &rkey).ref : NULL;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(Self* self, _m_keyraw rkey)
    { return (_m_value*)_c_MEMB(_get)(self, rkey); }

STC_INLINE int
_c_MEMB(_erase)(Self* self, _m_keyraw rkey) {
    _m_value* ref;
    if (self->size != 0 && (ref = _c_MEMB(_bucket_lookup_)(self, &rkey).ref) != NULL)
        { _c_MEMB(_erase_entry)(self, ref); return 1; }
    return 0;
}

STC_INLINE _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _c_MEMB(_erase_entry)(self, it.ref);
    if (it._mref->dist == 0)
        _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    for (_m_iter i = _c_MEMB(_begin)(self); i.ref; _c_MEMB(_next)(&i)) {
        const _m_keyraw _raw = i_keytoraw(_i_keyref(i.ref));
        if (!_c_MEMB(_contains)(other, _raw)) return false;
    }
    return true;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF _m_iter _c_MEMB(_begin)(const Self* self) {
    _m_iter it = {self->table, self->table, self->meta};
    if (it.ref == NULL) return it;
    it._end += self->bucket_count;
    while (it._mref->dist == 0)
        ++it.ref, ++it._mref;
    if (it.ref == it._end) it.ref = NULL;
    return it;
}

STC_DEF float _c_MEMB(_max_load_factor)(const Self* self) {
    (void)self; return (float)(i_max_load_factor);
}

STC_DEF isize _c_MEMB(_capacity)(const Self* map) {
    return (isize)((float)map->bucket_count * (i_max_load_factor));
}

static void _c_MEMB(_wipe_)(Self* self) {
    if (self->size == 0)
        return;
    _m_value* d = self->table, *_end = &d[self->bucket_count];
    struct hmap_meta* m = self->meta;
    for (; d != _end; ++d)
        if ((m++)->dist)
            _c_MEMB(_value_drop)(self, d);
}

STC_DEF void _c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    if (self->bucket_count > 0) {
        _c_MEMB(_wipe_)(self);
        _i_free_n(self->meta, self->bucket_count + 1);
        _i_free_n(self->table, self->bucket_count);
    }
}

STC_DEF void _c_MEMB(_clear)(Self* self) {
    _c_MEMB(_wipe_)(self);
    self->size = 0;
    c_memset(self->meta, 0, c_sizeof(struct hmap_meta)*self->bucket_count);
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
#endif // _i_is_map

STC_DEF _m_result
_c_MEMB(_bucket_lookup_)(const Self* self, const _m_keyraw* rkeyptr) {
    const size_t _hash = i_hash(rkeyptr);
    const size_t _idxmask = (size_t)self->bucket_count - 1;
    _m_result _res = {.idx=_hash & _idxmask, .hashx=(uint8_t)((_hash >> 24) & _hashmask), .dist=1};

    while (_res.dist <= self->meta[_res.idx].dist) {
        if (self->meta[_res.idx].hashx == _res.hashx) {
            const _m_keyraw _raw = i_keytoraw(_i_keyref(&self->table[_res.idx]));
            if (i_eq((&_raw), rkeyptr)) {
                _res.ref = &self->table[_res.idx];
                break;
            }
        }
        _res.idx = (_res.idx + 1) & _idxmask;
        ++_res.dist;
    }
    return _res;
}

STC_DEF _m_result
_c_MEMB(_bucket_insert_)(const Self* self, const _m_keyraw* rkeyptr) {
    _m_result res = _c_MEMB(_bucket_lookup_)(self, rkeyptr);
    if (res.ref) // bucket exists
        return res;
    res.ref = &self->table[res.idx];
    res.inserted = true;
    struct hmap_meta mnew = {.hashx=(uint16_t)(res.hashx & _hashmask),
                             .dist=(uint16_t)(res.dist & _distmask)};
    struct hmap_meta mcur = self->meta[res.idx];
    self->meta[res.idx] = mnew;

    if (mcur.dist != 0) { // collision, reorder buckets
        size_t mask = (size_t)self->bucket_count - 1;
        _m_value dcur = *res.ref;
        for (;;) {
            res.idx = (res.idx + 1) & mask;
            ++mcur.dist;
            if (self->meta[res.idx].dist == 0)
                break;
            if (self->meta[res.idx].dist < mcur.dist) {
                c_swap(&mcur, &self->meta[res.idx]);
                c_swap(&dcur, &self->table[res.idx]);
            }
        }
        self->meta[res.idx] = mcur;
        self->table[res.idx] = dcur;
    }
    return res;
}


#ifndef i_no_clone
    STC_DEF Self
    _c_MEMB(_clone)(Self map) {
        if (map.bucket_count == 0)
            return c_literal(Self){0};
        Self out = map, *self = &out; // _i_new_n may refer self via i_aux
        const isize _mbytes = (map.bucket_count + 1)*c_sizeof *map.meta;
        out.table = (_m_value *)i_malloc(map.bucket_count*c_sizeof *out.table);
        out.meta = (struct hmap_meta *)i_malloc(_mbytes);

        if (out.table && out.meta) {
            c_memcpy(out.meta, map.meta, _mbytes);
            for (isize i = 0; i < map.bucket_count; ++i)
                if (map.meta[i].dist)
                    out.table[i] = _c_MEMB(_value_clone)(self, map.table[i]);
            return out;
        } else {
            if (out.meta) i_free(out.meta, _mbytes);
            if (out.table) _i_free_n(out.table, map.bucket_count);
            return c_literal(Self){0};
        }
    }
#endif

STC_DEF bool
_c_MEMB(_reserve)(Self* _self, const isize _newcap) {
    isize _newbucks = (isize)((float)_newcap / (i_max_load_factor)) + 4;
    _newbucks = c_next_pow2(_newbucks);

    if (_newcap < _self->size || _newbucks == _self->bucket_count)
        return true;
    Self map = *_self, *self = &map; (void)self;
    map.table = _i_new_n(_m_value, _newbucks);
    map.meta = _i_new_zeros(struct hmap_meta, _newbucks + 1);
    map.bucket_count = _newbucks;

    bool ok = map.table && map.meta;
    if (ok) {  // Rehash:
        map.meta[_newbucks].dist = _distmask; // end-mark for iter
        const _m_value* d = _self->table;
        const struct hmap_meta* m = _self->meta;

        for (isize i = 0; i < _self->bucket_count; ++i, ++d) if (m[i].dist != 0) {
            _m_keyraw r = i_keytoraw(_i_keyref(d));
            *_c_MEMB(_bucket_insert_)(&map, &r).ref = *d; // move element
        }
        c_swap(_self, &map);
    }
    _i_free_n(map.meta, map.bucket_count + (int)(map.meta != NULL));
    _i_free_n(map.table, map.bucket_count);
    return ok;
}

STC_DEF void
_c_MEMB(_erase_entry)(Self* self, _m_value* _val) {
    _m_value* d = self->table;
    struct hmap_meta *m = self->meta;
    size_t i = (size_t)(_val - d), j = i;
    size_t mask = (size_t)self->bucket_count - 1;

    _c_MEMB(_value_drop)(self, _val);
    for (;;) {
        j = (j + 1) & mask;
        if (m[j].dist < 2) // 0 => empty, 1 => PSL 0
            break;
        d[i] = d[j];
        m[i] = m[j];
        --m[i].dist;
        i = j;
    }
    m[i].dist = 0;
    --self->size;
}

#endif // i_implement
#undef i_max_load_factor
#undef _i_is_set
#undef _i_is_map
#undef _i_is_hash
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#include "sys/finalize.h"
