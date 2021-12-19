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

/* csptr: shared_ptr type
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", p->name.str, p->last.str);
    c_drop(cstr, &p->name, &p->last);
}

#define i_tag person
#define i_val Person
#define i_valdrop Person_drop
#include <stc/csptr.h>

int main() {
    csptr_person p = csptr_person_new(Person_new("John", "Smiths"));
    csptr_person q = csptr_person_clone(p); // share the pointer

    printf("%s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
    c_drop(csptr_person, &p, &q);
}
*/

#ifndef CSPTR_H_INCLUDED
#define CSPTR_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#else
    #include <stdatomic.h>
    #define c_atomic_inc(v) (void)atomic_fetch_add(v, 1)
    #define c_atomic_dec_and_test(v) (atomic_fetch_sub(v, 1) == 1)
#endif

#define csptr_null {NULL, NULL}
#define _cx_csptr_rep struct _cx_memb(_rep_)
#endif // CSPTR_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix csptr_
#endif
#define _i_has_internal_clone
#include "template.h"
typedef i_valraw _cx_rawvalue;

#if !c_option(c_no_atomic)
  #define _i_atomic_inc(v)          c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#else
  #define _i_atomic_inc(v)          (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#endif
#if !c_option(c_is_fwd)
_cx_deftypes(_c_csptr_types, _cx_self, i_val);
#endif
_cx_csptr_rep { long counter; i_val value; };

STC_INLINE _cx_self
_cx_memb(_init)(void) { return c_make(_cx_self){NULL, NULL}; }

STC_INLINE long
_cx_memb(_use_count)(_cx_self ptr) { return ptr.use_count ? *ptr.use_count : 0; }

STC_INLINE _cx_self
_cx_memb(_with)(_cx_value* p) {
    _cx_self ptr = {p};
    if (p) *(ptr.use_count = c_alloc(long)) = 1;
    return ptr;
}

STC_INLINE _cx_self
_cx_memb(_view)(const _cx_value* p) {
    _cx_self ptr = {(_cx_value*) p};
    return ptr;
}

STC_INLINE _cx_self
_cx_memb(_new)(i_val val) {
    _cx_self ptr; _cx_csptr_rep *rep = c_alloc(_cx_csptr_rep);
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}
STC_INLINE _cx_self _cx_memb(_make)(i_val val) // [deprecated]
    { return _cx_memb(_new)(val); }

STC_INLINE _cx_self // does not use i_valfrom, so we can bypass c_no_clone
_cx_memb(_clone)(_cx_self ptr) {
    if (ptr.use_count) _i_atomic_inc(ptr.use_count);
    return ptr;
}

STC_INLINE _cx_self
_cx_memb(_move)(_cx_self* self) {
    _cx_self ptr = *self;
    self->get = NULL, self->use_count = NULL;
    return ptr;
}

STC_INLINE void
_cx_memb(_drop)(_cx_self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_valdrop(self->get);
        if (self->get != &((_cx_csptr_rep *)self->use_count)->value)
            c_free(self->get);
        c_free(self->use_count);
    }
}

STC_INLINE void
_cx_memb(_reset)(_cx_self* self) {
    _cx_memb(_drop)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void
_cx_memb(_reset_with)(_cx_self* self, _cx_value* p) {
    _cx_memb(_drop)(self);
    *self = _cx_memb(_with)(p);
}

STC_INLINE void
_cx_memb(_reset_new)(_cx_self* self, i_val val) {
    _cx_memb(_drop)(self);
    *self = _cx_memb(_new)(val);
}

#if !c_option(c_no_clone)
    STC_INLINE _cx_self _cx_memb(_from)(i_valraw raw) { 
        return _cx_memb(_new)(i_valfrom(raw));
    }

    STC_INLINE i_valraw
    _cx_memb(_toraw)(const _cx_self* self) { 
        return i_valto(self->get);
    }
#endif

STC_INLINE void
_cx_memb(_copy)(_cx_self* self, _cx_self ptr) {
    if (ptr.use_count) _i_atomic_inc(ptr.use_count);
    _cx_memb(_drop)(self); *self = ptr;
}

STC_INLINE void
_cx_memb(_take)(_cx_self* self, _cx_self ptr) {
    if (self->get != ptr.get) _cx_memb(_drop)(self);
    *self = ptr;
}

STC_INLINE uint64_t
_cx_memb(_hash)(const _cx_self* self, size_t n) {
    #if c_option(c_no_cmp) && SIZE_MAX >> 32
        return c_hash64(&self->get, 8);
    #elif c_option(c_no_cmp)
        return c_hash32(&self->get, 4);
    #else
        return i_hash(self->get, sizeof *self->get);
    #endif
}

STC_INLINE int
_cx_memb(_cmp)(const _cx_self* x, const _cx_self* y) {
    #if c_option(c_no_cmp)
        return c_default_cmp(&x->get, &y->get);
    #else
        return i_cmp(x->get, y->get);
    #endif
}

STC_INLINE bool
_cx_memb(_equalto)(const _cx_self* x, const _cx_self* y) {
    return !_cx_memb(_cmp)(x, y);
}
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#include "template.h"
