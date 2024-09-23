/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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

/* carc: atomic reference counted shared_ptr
#define i_implement
#include "stc/cstr.h"

typedef struct { cstr name, last; } Person;

Person Person_make(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    cstr_drop(&p->name);
    cstr_drop(&p->last);
}

#define i_type ArcPers
#define i_valclass Person    // clone, drop, cmp, hash
#include "stc/arc.h"

int main(void) {
    ArcPers p = ArcPers_from(Person_make("John", "Smiths"));
    ArcPers q = ArcPers_clone(p); // share the pointer

    printf("%s %s. uses: %ld\n", cstr_str(&q.get->name), cstr_str(&q.get->last), *q.use_count);
    c_drop(ArcPers, &p, &q);
}
*/
#include "priv/linkage.h"

#ifndef STC_ARC_H_INCLUDED
#define STC_ARC_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER || defined i_no_atomic
    typedef long catomic_long;
#else // try with C11
    typedef _Atomic(long) catomic_long;
#endif
#if defined _MSC_VER
    #include <intrin.h>
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#elif defined __GNUC__ || defined __clang__
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#else // try with C11
    #include <stdatomic.h>
    #define c_atomic_inc(v) (void)atomic_fetch_add(v, 1)
    #define c_atomic_dec_and_test(v) (atomic_fetch_sub(v, 1) == 1)
#endif

// @wmww, thanks for finding & fixing rare memleak with the metadata struct!
struct _arc_metadata { catomic_long counter; bool value_included; };
#endif // STC_ARC_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix arc_
#endif
#define _i_is_arc
#include "priv/template.h"
typedef i_keyraw _m_raw;

#if c_option(c_no_atomic)
  #define i_no_atomic
#endif
#if !defined i_no_atomic
  #define _i_atomic_inc(v)          c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#else
  #define _i_atomic_inc(v)          (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#endif
#ifndef i_is_forward
_c_DEFTYPES(_c_arc_types, Self, i_key);
#endif
struct _c_MEMB(_rep_) { struct _arc_metadata metadata; i_key value; };

STC_INLINE Self _c_MEMB(_init)(void)
    { return c_literal(Self){NULL, NULL}; }

STC_INLINE long _c_MEMB(_use_count)(const Self* self)
    { return self->use_count ? *self->use_count : 0; }

STC_INLINE Self _c_MEMB(_from_ptr)(_m_value* ptr) {
    Self arc = {ptr};
    if (ptr) {
        struct _arc_metadata* metadata = _i_malloc(struct _arc_metadata, 1);
        metadata->value_included = false;
        *(arc.use_count = &metadata->counter) = 1;
    }    
    return arc;
}

// c++: std::make_shared<_m_value>(val)
STC_INLINE Self _c_MEMB(_make)(_m_value val) {
    Self arc;
    struct _c_MEMB(_rep_)* rep = _i_malloc(struct _c_MEMB(_rep_), 1);
    rep->metadata.value_included = true;
    *(arc.use_count = &rep->metadata.counter) = 1;
    *(arc.get = &rep->value) = val;
    return arc;
}

STC_INLINE _m_raw _c_MEMB(_toraw)(const Self* self)
    { return i_keytoraw(self->get); }

STC_INLINE Self _c_MEMB(_move)(Self* self) {
    Self arc = *self;
    self->get = NULL, self->use_count = NULL;
    return arc;
}

STC_INLINE void _c_MEMB(_drop)(const Self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_keydrop(self->get);
        // assume counter is first member of structs _arc_metadata and _rep_:
        if (((struct _arc_metadata *)self->use_count)->value_included) {
            i_free(self->use_count, c_sizeof(struct _c_MEMB(_rep_))); // _make()
        } else {                                                      
            i_free(self->use_count, c_sizeof(struct _arc_metadata)); // _from_ptr()
            i_free(self->get, c_sizeof *self->get);
        }
    }
}

STC_INLINE void _c_MEMB(_reset_to)(Self* self, _m_value* ptr) {
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_from_ptr)(ptr);
}

STC_INLINE Self _c_MEMB(_from)(_m_raw raw)
    { return _c_MEMB(_make)(i_keyfrom(raw)); }

// does not use i_keyclone, so OK to always define.
STC_INLINE Self _c_MEMB(_clone)(Self arc) {
    if (arc.use_count) _i_atomic_inc(arc.use_count);
    return arc;
}

// take ownership of unowned
STC_INLINE void _c_MEMB(_take)(Self* self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}
// share ownership with arc
STC_INLINE void _c_MEMB(_assign)(Self* self, Self arc) {
    if (arc.use_count) _i_atomic_inc(arc.use_count);
    _c_MEMB(_drop)(self);
    *self = arc;
}

#if defined _i_has_cmp
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _c_MEMB(_cmp)(const Self* self, const Self* other) {
        _m_raw rawx = i_keytoraw(self->get), rawy = i_keytoraw(other->get);
        return i_cmp((&rawx), (&rawy));
    }
#else
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return (rx > ry) - (rx < ry); }

    STC_INLINE int _c_MEMB(_cmp)(const Self* self, const Self* other) {
        const _m_value *x = self->get, *y = other->get;
        return (x > y) - (x < y);
    }
#endif

#if defined _i_has_eq
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other) {
        _m_raw rawx = i_keytoraw(self->get), rawy = i_keytoraw(other->get);
        return i_eq((&rawx), (&rawy));
    }
#else
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return rx == ry; }

    STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other)
        { return self->get == other->get; }
#endif

#if !defined i_no_hash && defined _i_has_eq
    STC_INLINE uint64_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _c_MEMB(_hash)(const Self* self) {
        _m_raw raw = i_keytoraw(self->get);
        return i_hash((&raw));
    }
#else
    STC_INLINE uint64_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return c_default_hash(&rx); }

    STC_INLINE uint64_t _c_MEMB(_hash)(const Self* self)
        { return c_default_hash(&self->get); }
#endif // i_no_hash

#undef i_no_atomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#undef _i_is_arc
#include "priv/linkage2.h"
#include "priv/template2.h"
