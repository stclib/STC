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

/* carc: atomic reference counted shared_ptr
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_make(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    cstr_drop(&p->name);
    cstr_drop(&p->last);
}

#define i_type ArcPers
#define i_key Person
#define i_keydrop Person_drop
#include <stc/carc.h>

int main() {
    ArcPers p = ArcPers_from(Person_make("John", "Smiths"));
    ArcPers q = ArcPers_clone(p); // share the pointer

    printf("%s %s. uses: %ld\n", cstr_str(&q.get->name), cstr_str(&q.get->last), *q.use_count);
    c_drop(ArcPers, &p, &q);
}
*/
#include "ccommon.h"

#ifndef CARC_H_INCLUDED
#define CARC_H_INCLUDED
#include "forward.h"
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
    typedef long catomic_long;
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER)
    #include <intrin.h>
    typedef long catomic_long;
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#else
    #include <stdatomic.h>
    typedef _Atomic long catomic_long;
    #define c_atomic_inc(v) (void)atomic_fetch_add(v, 1)
    #define c_atomic_dec_and_test(v) (atomic_fetch_sub(v, 1) == 1)
#endif

#define carc_NULL {NULL, NULL}
#endif // CARC_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix carc_
#endif
#ifdef i_eq
#define _i_eq
#endif
#include "priv/template.h"
typedef i_keyraw _cx_raw;

#if !c_option(c_no_atomic)
  #define _i_atomic_inc(v)          c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#else
  #define _i_atomic_inc(v)          (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#endif
#ifndef i_is_forward
_cx_deftypes(_c_carc_types, _cx_self, i_key);
#endif
struct _cx_memb(_rep_) { catomic_long counter; i_key value; };

STC_INLINE _cx_self _cx_memb(_init)(void) 
    { return c_LITERAL(_cx_self){NULL, NULL}; }

STC_INLINE long _cx_memb(_use_count)(const _cx_self* self)
    { return self->use_count ? *self->use_count : 0; }

STC_INLINE _cx_self _cx_memb(_from_ptr)(_cx_value* p) {
    _cx_self ptr = {p};
    if (p) 
        *(ptr.use_count = _i_alloc(catomic_long)) = 1;
    return ptr;
}

// c++: std::make_shared<_cx_value>(val)
STC_INLINE _cx_self _cx_memb(_make)(_cx_value val) {
    _cx_self ptr;
    struct _cx_memb(_rep_)* rep = _i_alloc(struct _cx_memb(_rep_));
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}

STC_INLINE _cx_raw _cx_memb(_toraw)(const _cx_self* self)
    { return i_keyto(self->get); }

STC_INLINE _cx_self _cx_memb(_move)(_cx_self* self) {
    _cx_self ptr = *self;
    self->get = NULL, self->use_count = NULL;
    return ptr;
}

STC_INLINE void _cx_memb(_drop)(_cx_self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_keydrop(self->get);
        if ((char *)self->get != (char *)self->use_count + offsetof(struct _cx_memb(_rep_), value))
            i_free(self->get);
        i_free((long*)self->use_count);
    }
}

STC_INLINE void _cx_memb(_reset)(_cx_self* self) {
    _cx_memb(_drop)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void _cx_memb(_reset_to)(_cx_self* self, _cx_value* p) {
    _cx_memb(_drop)(self);
    *self = _cx_memb(_from_ptr)(p);
}

#ifndef i_no_emplace
STC_INLINE _cx_self _cx_memb(_from)(_cx_raw raw)
    { return _cx_memb(_make)(i_keyfrom(raw)); }
#else
STC_INLINE _cx_self _cx_memb(_from)(_cx_value val)
    { return _cx_memb(_make)(val); }
#endif    

// does not use i_keyclone, so OK to always define.
STC_INLINE _cx_self _cx_memb(_clone)(_cx_self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    return ptr;
}

// take ownership of unowned
STC_INLINE void _cx_memb(_take)(_cx_self* self, _cx_self unowned) {
    _cx_memb(_drop)(self);
    *self = unowned;
}
// share ownership with ptr
STC_INLINE void _cx_memb(_assign)(_cx_self* self, _cx_self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    _cx_memb(_drop)(self);
    *self = ptr;
}

#ifndef i_no_cmp
STC_INLINE int _cx_memb(_raw_cmp)(const _cx_raw* rx, const _cx_raw* ry)
    { return i_cmp(rx, ry); }

STC_INLINE int _cx_memb(_cmp)(const _cx_self* self, const _cx_self* other) {
    _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
    return i_cmp((&rx), (&ry));
}
#endif

#ifdef _i_eq
STC_INLINE bool _cx_memb(_raw_eq)(const _cx_raw* rx, const _cx_raw* ry)
    { return i_eq(rx, ry); }

STC_INLINE bool _cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
    return i_eq((&rx), (&ry));
}
#elif !defined i_no_cmp
STC_INLINE bool _cx_memb(_raw_eq)(const _cx_raw* rx, const _cx_raw* ry)
    { return i_cmp(rx, ry) == 0; }

STC_INLINE bool _cx_memb(_eq)(const _cx_self* self, const _cx_self* other)
    { return _cx_memb(_cmp)(self, other) == 0; }
#endif

#ifndef i_no_hash
STC_INLINE uint64_t _cx_memb(_raw_hash)(const _cx_raw* rx)
    { return i_hash(rx); }

STC_INLINE uint64_t _cx_memb(_hash)(const _cx_self* self)
    { _cx_raw rx = i_keyto(self->get); return i_hash((&rx)); }
#endif

#undef _i_eq
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#include "priv/template2.h"
