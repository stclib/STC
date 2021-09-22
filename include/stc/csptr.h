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
    #define c_atomic_increment(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_decrement(v) __atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define c_atomic_increment(v) (void)_InterlockedIncrement(v)
    #define c_atomic_decrement(v) _InterlockedDecrement(v)
#elif defined(__i386__) || defined(__x86_64__)
    STC_INLINE void c_atomic_increment(atomic_count_t* v)
        { __asm__ __volatile__("lock; incq %0" :"=m"(*v) :"m"(*v)); }
    STC_INLINE atomic_count_t c_atomic_decrement(atomic_count_t* v) {
        atomic_count_t r;
        __asm__ __volatile__("lock; xadd %0, %1" :"=r"(r) :"m"(*v), "0"(-1));
        return r - 1;
    }
#endif

#define csptr_null {NULL, NULL}
#endif // CSPTR_H_INCLUDED

#ifndef i_prefix
#define i_prefix csptr_
#endif
#include "template.h"

#ifdef i_nonatomic
  #define cx_increment(v) (++*(v))
  #define cx_decrement(v) (--*(v))
#else
  #define cx_increment(v) c_atomic_increment(v)
  #define cx_decrement(v) c_atomic_decrement(v)
#endif
#ifndef i_fwd
cx_deftypes(_c_csptr_types, Self, i_val);
#endif
#define cx_csptr_rep struct cx_memb(_rep_)
cx_csptr_rep { atomic_count_t counter; cx_value_t value; };

STC_INLINE Self
cx_memb(_init)(void) { return c_make(Self){NULL, NULL}; }

STC_INLINE atomic_count_t
cx_memb(_use_count)(Self ptr) { return ptr.use_count ? *ptr.use_count : 0; }

STC_INLINE Self
cx_memb(_from)(cx_value_t* p) {
    Self ptr = {p};
    if (p) *(ptr.use_count = c_new(atomic_count_t)) = 1;
    return ptr;
}

STC_INLINE Self
cx_memb(_make)(cx_value_t val) {
    Self ptr; cx_csptr_rep *rep = c_new(cx_csptr_rep);
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}

STC_INLINE Self
cx_memb(_clone)(Self ptr) {
    if (ptr.use_count) cx_increment(ptr.use_count);
    return ptr;
}

STC_INLINE Self
cx_memb(_move)(Self* self) {
    Self ptr = *self;
    self->get = NULL, self->use_count = NULL;
    return ptr;
}

STC_INLINE void
cx_memb(_del)(Self* self) {
    if (self->use_count && cx_decrement(self->use_count) == 0) {
        i_valdel(self->get);
        if (self->get != &((cx_csptr_rep *)self->use_count)->value)
            c_free(self->get);
        c_free(self->use_count);
    }
}

STC_INLINE void
cx_memb(_reset)(Self* self) {
    cx_memb(_del)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void
cx_memb(_reset_from)(Self* self, cx_value_t* p) {
    cx_memb(_del)(self);
    *self = cx_memb(_from)(p);
}

STC_INLINE void
cx_memb(_reset_with)(Self* self, cx_value_t val) {
    cx_memb(_del)(self);
    *self = cx_memb(_make)(val);
}

STC_INLINE void
cx_memb(_copy)(Self* self, Self ptr) {
    if (ptr.use_count) cx_increment(ptr.use_count);
    cx_memb(_del)(self); *self = ptr;
}

STC_INLINE void
cx_memb(_take)(Self* self, Self ptr) {
    if (self->get != ptr.get) cx_memb(_del)(self);
    *self = ptr;
}

STC_INLINE int
cx_memb(_compare)(const Self* x, const Self* y) {
    return i_cmp(x->get, y->get);
}

#undef cx_csptr_rep
#undef cx_increment
#undef cx_decrement
#undef i_nonatomic
#include "template.h"