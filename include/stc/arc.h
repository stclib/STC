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

/* carc: atomic reference counted shared_ptr
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

// @wmww: Now fixed rare memleak by adding 4 bytes when allocating the counter alone.
struct _arc_metadata { catomic_long counter; };
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
#ifndef i_declared
_c_DEFTYPES(_c_arc_types, Self, i_key);
#endif
struct _c_MEMB(_rep_) { struct _arc_metadata metadata; i_key value; };

STC_INLINE Self _c_MEMB(_init)(void)
    { return c_literal(Self){NULL, NULL}; }

STC_INLINE long _c_MEMB(_use_count)(const Self* self)
    { return self->use_count ? *self->use_count : 0; }


// c++: std::make_shared<_m_value>(val)
STC_INLINE Self _c_MEMB(_make)(_m_value val) {
    Self unowned;
    struct _c_MEMB(_rep_)* rep = _i_malloc(struct _c_MEMB(_rep_), 1);
    *(unowned.use_count = &rep->metadata.counter) = 1;
    *(unowned.get = &rep->value) = val; // (.use_count, .get) are OFFSET bytes apart.
    return unowned;
}

STC_INLINE Self _c_MEMB(_from_ptr)(_m_value* ptr) {
    enum {OFFSET = offsetof(struct _c_MEMB(_rep_), value)};
    Self unowned = {ptr};
    if (ptr) {
        // Adds 4 dummy bytes to ensure that the if-test in _drop() is safe.
        struct _arc_metadata* meta = (struct _arc_metadata*)i_malloc(OFFSET + 4);
        *(unowned.use_count = &meta->counter) = 1;
    }
    return unowned;
}

STC_INLINE Self _c_MEMB(_from)(_m_raw raw)
    { return _c_MEMB(_make)(i_keyfrom(raw)); }

STC_INLINE _m_raw _c_MEMB(_toraw)(const Self* self)
    { return i_keytoraw(self->get); }

// destructor
STC_INLINE void _c_MEMB(_drop)(const Self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        enum {OFFSET = offsetof(struct _c_MEMB(_rep_), value)};
        i_keydrop(self->get);

        if ((char*)self->use_count + OFFSET == (char*)self->get) {
            i_free((void*)self->use_count, c_sizeof(struct _c_MEMB(_rep_))); // _make()
        } else {
            i_free((void*)self->use_count, OFFSET + 4); // _from_ptr()
            i_free(self->get, c_sizeof *self->get);
        }
    }
}

// move ownership to receiving arc
STC_INLINE Self _c_MEMB(_move)(Self* self) {
    Self arc = *self;
    memset(self, 0, sizeof *self);
    return arc; // now unowned
}

// take ownership of pointer p
STC_INLINE void _c_MEMB(_reset_to)(Self* self, _m_value* ptr) {
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_from_ptr)(ptr);
}

// take ownership of unowned arc
STC_INLINE void _c_MEMB(_take)(Self* self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

// make shared ownership with owned arc
STC_INLINE void _c_MEMB(_assign)(Self* self, const Self* owned) {
    if (owned->use_count) _i_atomic_inc(owned->use_count);
    _c_MEMB(_drop)(self);
    *self = *owned;
}

// clone by sharing. Does not use i_keyclone, so OK to always define.
STC_INLINE Self _c_MEMB(_clone)(Self owned) {
    if (owned.use_count) _i_atomic_inc(owned.use_count);
    return owned;
}

#if defined _i_has_cmp
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return i_cmp(rx, ry); }
#endif

#if defined _i_has_eq
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return i_eq(rx, ry); }
#endif

#if !defined i_no_hash && defined _i_has_eq
    STC_INLINE size_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return i_hash(rx); }
#endif // i_no_hash

#undef i_no_atomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#undef _i_is_arc
#include "priv/linkage2.h"
#include "priv/template2.h"
