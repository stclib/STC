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

Person Person_init(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_del(Person* p) {
    printf("del: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

#define i_tag person
#define i_val Person
#define i_valdel Person_del
#include <stc/csptr.h>

int main() {
    csptr_person p = csptr_person_make(Person_init("John", "Smiths"));
    csptr_person q = csptr_person_clone(p); // share the pointer

    printf("%s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
    c_del(csptr_person, &p, &q);
}
*/

#ifndef CSPTR_H_INCLUDED
#define CSPTR_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>

typedef long atomic_count_t;
#if defined(__GNUC__) || defined(__clang__)
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#elif defined(__i386__) || defined(__x86_64__)
    STC_INLINE void c_atomic_inc(atomic_count_t* v)
        { asm volatile("lock; incq %0" :"=m"(*v) :"m"(*v)); }
    STC_INLINE bool c_atomic_dec_and_test(atomic_count_t* v) {
        unsigned char c;
        asm volatile("lock; decq %0; sete %1" :"=m"(*v), "=qm"(c) :"m"(*v) :"memory");
        return !c;
    }
#endif

#define csptr_null {NULL, NULL}
#define _cx_csptr_rep struct _cx_memb(_rep_)
#endif // CSPTR_H_INCLUDED

#ifndef i_prefix
#define i_prefix csptr_
#endif
#include "template.h"

#ifdef i_nonatomic
  #define _i_atomic_inc(v) (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#else
  #define _i_atomic_inc(v) c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#endif
#ifndef i_fwd
_cx_deftypes(_c_csptr_types, _cx_self, i_val);
#endif
_cx_csptr_rep { atomic_count_t counter; _cx_value value; };

STC_INLINE _cx_self
_cx_memb(_init)(void) { return c_make(_cx_self){NULL, NULL}; }

STC_INLINE atomic_count_t
_cx_memb(_use_count)(_cx_self ptr) { return ptr.use_count ? *ptr.use_count : 0; }

STC_INLINE _cx_self
_cx_memb(_from)(_cx_value* p) {
    _cx_self ptr = {p};
    if (p) *(ptr.use_count = c_alloc(atomic_count_t)) = 1;
    return ptr;
}

STC_INLINE _cx_self
_cx_memb(_make)(_cx_value val) {
    _cx_self ptr; _cx_csptr_rep *rep = c_alloc(_cx_csptr_rep);
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}

STC_INLINE _cx_self
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
_cx_memb(_del)(_cx_self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_valdel(self->get);
        if (self->get != &((_cx_csptr_rep *)self->use_count)->value)
            c_free(self->get);
        c_free(self->use_count);
    }
}

STC_INLINE void
_cx_memb(_reset)(_cx_self* self) {
    _cx_memb(_del)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void
_cx_memb(_reset_from)(_cx_self* self, _cx_value* p) {
    _cx_memb(_del)(self);
    *self = _cx_memb(_from)(p);
}

STC_INLINE void
_cx_memb(_reset_with)(_cx_self* self, _cx_value val) {
    _cx_memb(_del)(self);
    *self = _cx_memb(_make)(val);
}

STC_INLINE void
_cx_memb(_copy)(_cx_self* self, _cx_self ptr) {
    if (ptr.use_count) _i_atomic_inc(ptr.use_count);
    _cx_memb(_del)(self); *self = ptr;
}

STC_INLINE void
_cx_memb(_take)(_cx_self* self, _cx_self ptr) {
    if (self->get != ptr.get) _cx_memb(_del)(self);
    *self = ptr;
}

#ifndef i_cmp_none
STC_INLINE int
_cx_memb(_compare)(const _cx_self* x, const _cx_self* y) {
#ifdef i_cmp_default
    return c_default_compare(&x->get, &y->get);
#else
    return i_cmp(x->get, y->get);
#endif
}
#endif
#undef i_nonatomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#include "template.h"
