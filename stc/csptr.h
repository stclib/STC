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
#ifndef CSPTR__H__
#define CSPTR__H__

#include "cdefs.h"

/* csptr: std::shared_ptr -like type: */
/*
#include <stc/csptr.h>
#include <stc/cstr.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr(name), p->last = cstr(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(pe, Person, Person_del);

int main() {
    csptr_pe p = csptr_pe_make(Person_make(c_new(Person), "Joe", "Jordan"));
    csptr_pe q = csptr_pe_share(p); // share the pointer

    printf("%s %s: %d\n", q.get->name.str, q.get->last.str, *q.use_count);
    c_del(csptr_pe, &p, &q);
}
*/
typedef long atomic_count_t;
#if defined(__GNUC__) || defined(__clang__)
    static inline void atomic_increment(atomic_count_t* pw) {__atomic_add_fetch(pw, 1, __ATOMIC_SEQ_CST);}
    static inline atomic_count_t atomic_decrement(atomic_count_t* pw) {return __atomic_sub_fetch(pw, 1, __ATOMIC_SEQ_CST);}
#elif defined(_MSC_VER)
    #include <intrin.h>
    static inline void atomic_increment(atomic_count_t* pw) {_InterlockedIncrement(pw);}
    static inline atomic_count_t atomic_decrement(atomic_count_t* pw) {return _InterlockedDecrement(pw);}
#elif defined(__i386__) || defined(__x86_64__)
    static inline void atomic_increment(atomic_count_t* pw) {
        __asm__ (
            "lock\n\t"
            "incl %0":
            "=m"( *pw ): // ++*pw // output (%0)
            "m"( *pw ):  // input (%1)
            "cc"         // clobbers
        );
    }
    static inline atomic_count_t atomic_decrement(atomic_count_t* pw) {
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
    using_csptr_3(X, Value, c_default_del)

#define using_csptr_3(X, Value, valueDestroy) \
    typedef Value csptr_##X##_value_t; \
    typedef struct { csptr_##X##_value_t* get; atomic_count_t* use_count; } csptr_##X, csptr_##X##_t; \
\
    STC_INLINE csptr_##X \
    csptr_##X##_make(csptr_##X##_value_t* p) { \
        csptr_##X ptr = {p}; \
        if (p) *(ptr.use_count = c_new_1(atomic_count_t)) = 1; \
        return ptr; \
    } \
    STC_INLINE csptr_##X \
    csptr_##X##_share(csptr_##X ptr) { \
        if (ptr.get) atomic_increment(ptr.use_count); \
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
        *self = csptr_##X##_make(p); \
    } \
\
    STC_INLINE csptr_##X##_value_t* \
    csptr_##X##_get(csptr_##X x) {return x.get;} \
\
    typedef csptr_##X csptr_##X##_t


#define using_csptr_4(X, Value, valueDestroy, valueCompare) \
    using_csptr_3(X, Value, valueDestroy); \
    STC_INLINE int \
    csptr_##X##_compare(csptr_##X* x, csptr_##X* y) { \
        return valueCompare(x->get, y->get); \
    } \
    typedef int csptr_##X##_dud4

#define using_csptr_5(X, Value, valueDestroy, valueCompare, hash) \
    using_csptr_4(X, Value, valueDestroy, valueCompare); \
    STC_INLINE int \
    csptr_##X##_equals(csptr_##X* x, csptr_##X* y) { \
        return valueCompare(x->get, y->get) == 0; \
    } \
    STC_INLINE uint32_t \
    csptr_##X##_hash(csptr_##X* self, size_t len) { \
        return hash(&self->get, len); \
    } \
    typedef int csptr_##X##_dud5

#endif
