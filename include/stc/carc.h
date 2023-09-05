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
#define i_implement
#include <stc/cstr.h>

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
#include <stc/carc.h>

int main(void) {
    ArcPers p = ArcPers_from(Person_make("John", "Smiths"));
    ArcPers q = ArcPers_clone(p); // share the pointer

    printf("%s %s. uses: %ld\n", cstr_str(&q.get->name), cstr_str(&q.get->last), *q.use_count);
    c_drop(ArcPers, &p, &q);
}
*/
#include "priv/linkage.h"

#ifndef CARC_H_INCLUDED
#define CARC_H_INCLUDED
#include "ccommon.h"
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

#define carc_null {0}
#endif // CARC_H_INCLUDED

#define _i_prefix carc_
#define _i_carc
#include "priv/template.h"
typedef i_keyraw _cx_raw;

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
_cx_DEFTYPES(_c_carc_types, _cx_Self, i_key);
#endif
struct _cx_MEMB(_rep_) { catomic_long counter; i_key value; };

STC_INLINE _cx_Self _cx_MEMB(_init)(void) 
    { return c_LITERAL(_cx_Self){NULL, NULL}; }

STC_INLINE long _cx_MEMB(_use_count)(const _cx_Self* self)
    { return self->use_count ? *self->use_count : 0; }

STC_INLINE _cx_Self _cx_MEMB(_from_ptr)(_cx_value* p) {
    _cx_Self ptr = {p};
    if (p) 
        *(ptr.use_count = _i_alloc(catomic_long)) = 1;
    return ptr;
}

// c++: std::make_shared<_cx_value>(val)
STC_INLINE _cx_Self _cx_MEMB(_make)(_cx_value val) {
    _cx_Self ptr;
    struct _cx_MEMB(_rep_)* rep = _i_alloc(struct _cx_MEMB(_rep_));
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}

STC_INLINE _cx_raw _cx_MEMB(_toraw)(const _cx_Self* self)
    { return i_keyto(self->get); }

STC_INLINE _cx_Self _cx_MEMB(_move)(_cx_Self* self) {
    _cx_Self ptr = *self;
    self->get = NULL, self->use_count = NULL;
    return ptr;
}

STC_INLINE void _cx_MEMB(_drop)(_cx_Self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_keydrop(self->get);
        if ((char *)self->get != (char *)self->use_count + offsetof(struct _cx_MEMB(_rep_), value))
            i_free(self->get);
        i_free((long*)self->use_count);
    }
}

STC_INLINE void _cx_MEMB(_reset)(_cx_Self* self) {
    _cx_MEMB(_drop)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void _cx_MEMB(_reset_to)(_cx_Self* self, _cx_value* p) {
    _cx_MEMB(_drop)(self);
    *self = _cx_MEMB(_from_ptr)(p);
}

#ifndef i_no_emplace
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_raw raw)
    { return _cx_MEMB(_make)(i_keyfrom(raw)); }
#else
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_value val)
    { return _cx_MEMB(_make)(val); }
#endif    

// does not use i_keyclone, so OK to always define.
STC_INLINE _cx_Self _cx_MEMB(_clone)(_cx_Self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    return ptr;
}

// take ownership of unowned
STC_INLINE void _cx_MEMB(_take)(_cx_Self* self, _cx_Self unowned) {
    _cx_MEMB(_drop)(self);
    *self = unowned;
}
// share ownership with ptr
STC_INLINE void _cx_MEMB(_assign)(_cx_Self* self, _cx_Self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    _cx_MEMB(_drop)(self);
    *self = ptr;
}

#if defined i_use_cmp
    STC_INLINE int _cx_MEMB(_raw_cmp)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_cmp((&rx), (&ry));
    }

    STC_INLINE bool _cx_MEMB(_raw_eq)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_eq((&rx), (&ry));
    }

    #ifndef i_no_hash
    STC_INLINE uint64_t _cx_MEMB(_raw_hash)(const _cx_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { _cx_raw rx = i_keyto(self->get); return i_hash((&rx)); }
    #endif // i_no_hash

#else

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        return c_default_cmp(&self->get, &other->get);
    }
    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        return self->get == other->get;
    }
    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { return c_default_hash(&self->get); }
#endif // i_use_cmp

#include "priv/template2.h"
#undef i_no_atomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#undef _i_carc
