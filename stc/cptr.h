/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CPTR__H__
#define CPTR__H__

/* cptr: std::unique_ptr -like type */
/*
#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = strcmp(p->name.str, q->name.str);
    return cmp == 0 ? strcmp(p->last.str, q->last.str) : cmp;
}

using_cptr(pe, Person, Person_del, Person_compare);
using_cvec(pe, Person*, cptr_pe_del, cptr_pe_compare);

int main() {
    cvec_pe vec = cvec_pe_init();
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Joe", "Jordan"));
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Jane", "Jacobs"));

    c_foreach (i, cvec_pe, vec)
        printf("%s %s\n", (*i.ref)->name.str, (*i.ref)->last.str);
    cvec_pe_del(&vec);
}
*/
#include "ccommon.h"

#define using_cptr(...) c_MACRO_OVERLOAD(using_cptr, __VA_ARGS__)

#define using_cptr_2(X, Value) \
    using_cptr_3(X, Value, c_default_compare)

#define using_cptr_3(X, Value, valueCompare) \
    using_cptr_5(X, Value, valueCompare, c_default_del, c_default_clone)

#define using_cptr_5(X, Value, valueCompare, valueDestroy, valueClone) \
    typedef Value cptr_##X##_value_t; \
    typedef cptr_##X##_value_t *cptr_##X; \
\
    STC_INLINE void \
    cptr_##X##_del(cptr_##X* self) { \
        valueDestroy(*self); \
        c_free(*self); \
    } \
    STC_INLINE cptr_##X \
    cptr_##X##_clone(cptr_##X ptr) { \
        cptr_##X clone = c_new_1(Value); \
        *clone = valueClone(*ptr); \
        return clone; \
    } \
\
    STC_INLINE void \
    cptr_##X##_reset(cptr_##X* self, cptr_##X##_value_t* p) { \
        cptr_##X##_del(self); \
        *self = p; \
    } \
\
    STC_INLINE int \
    cptr_##X##_compare(cptr_##X* x, cptr_##X* y) { \
        return valueCompare(*x, *y); \
    } \
    typedef cptr_##X cptr_##X##_t



/* csptr: std::shared_ptr -like type: */
/*
#include <stc/cptr.h>
#include <stc/cstr.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(pe, Person, Person_del);

int main() {
    csptr_pe p = csptr_pe_make(Person_make(c_new(Person), "Joe", "Jordan"));
    csptr_pe q = csptr_pe_clone(p); // share the pointer

    printf("%s %s: %d\n", q.get->name.str, q.get->last.str, *q.use_count);
    c_del(csptr_pe, &p, &q);
}
*/
typedef long atomic_count_t;
#if defined(__GNUC__) || defined(__clang__)
    STC_INLINE void atomic_increment(atomic_count_t* pw) {__atomic_add_fetch(pw, 1, __ATOMIC_SEQ_CST);}
    STC_INLINE atomic_count_t atomic_decrement(atomic_count_t* pw) {return __atomic_sub_fetch(pw, 1, __ATOMIC_SEQ_CST);}
#elif defined(_MSC_VER)
    #include <intrin.h>
    STC_INLINE void atomic_increment(atomic_count_t* pw) {_InterlockedIncrement(pw);}
    STC_INLINE atomic_count_t atomic_decrement(atomic_count_t* pw) {return _InterlockedDecrement(pw);}
#elif defined(__i386__) || defined(__x86_64__)
    STC_INLINE void atomic_increment(atomic_count_t* pw) {
        __asm__ (
            "lock\n\t"
            "incl %0":
            "=m"( *pw ): // ++*pw // output (%0)
            "m"( *pw ):  // input (%1)
            "cc"         // clobbers
        );
    }
    STC_INLINE atomic_count_t atomic_decrement(atomic_count_t* pw) {
        int r;
        __asm__ __volatile__ (
            "lock\n\t"
            "xadd %1, %0":
            "=m"( *pw ), "=r"( r ): // int r = *pw; // outputs (%0, %1)
            "m"( *pw ), "1"( -1 ):  // *pw += -1;   // inputs (%2, %3 == %1)
            "memory", "cc"          // clobbers
        );
        return r - 1;
    }
#endif

#define using_csptr(...) c_MACRO_OVERLOAD(using_csptr, __VA_ARGS__)

#define using_csptr_2(X, Value) \
    using_csptr_3(X, Value, c_default_compare)

#define using_csptr_3(X, Value, valueCompare) \
    using_csptr_4(X, Value, valueCompare, c_default_del)

#define using_csptr_5(X, Value, valueCompare, valueDestroy, dummyValueClone) \
    using_csptr_4(X, Value, valueCompare, valueDestroy)

#define using_csptr_4(X, Value, valueCompare, valueDestroy) \
    typedef Value csptr_##X##_value_t; \
    typedef struct { csptr_##X##_value_t* get; atomic_count_t* use_count; } csptr_##X; \
\
    STC_INLINE csptr_##X \
    csptr_##X##_from(csptr_##X##_value_t* p) { \
        csptr_##X ptr = {p}; \
        if (p) *(ptr.use_count = c_new_1(atomic_count_t)) = 1; \
        return ptr; \
    } \
    STC_INLINE csptr_##X \
    csptr_##X##_make(csptr_##X##_value_t val) { \
        csptr_##X ptr = {c_new_1(csptr_##X##_value_t), c_new_1(atomic_count_t)}; \
        *ptr.get = val, *ptr.use_count = 1; return ptr; \
    } \
    STC_INLINE csptr_##X \
    csptr_##X##_clone(csptr_##X ptr) { \
        if (ptr.use_count) atomic_increment(ptr.use_count); \
        return ptr; \
    } \
\
    STC_INLINE void \
    csptr_##X##_del(csptr_##X* self) { \
        if (self->use_count && atomic_decrement(self->use_count) == 0) { \
            c_free(self->use_count); \
            valueDestroy(self->get); \
            c_free(self->get); \
        } \
    } \
    STC_INLINE void \
    csptr_##X##_reset(csptr_##X* self, csptr_##X##_value_t* p) { \
        csptr_##X##_del(self); \
        *self = csptr_##X##_from(p); \
    } \
\
    STC_INLINE int \
    csptr_##X##_compare(csptr_##X* x, csptr_##X* y) { \
        return valueCompare(x->get, y->get); \
    } \
    typedef csptr_##X csptr_##X##_t

#endif
