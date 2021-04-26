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
#ifndef CSPTR_H_INCLUDED
#define CSPTR_H_INCLUDED

#include "ccommon.h"

/* csptr: std::shared_ptr -like type:

#include <stc/csptr.h>
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(pe, Person, c_no_compare, Person_del);

int main() {
    csptr_pe p = csptr_pe_from(Person_make(c_new(Person), "John", "Smiths"));
    csptr_pe q = csptr_pe_clone(p); // share the pointer

    printf("%s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
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

#define csptr_null {NULL, NULL}

#define using_csptr(...) c_MACRO_OVERLOAD(using_csptr, __VA_ARGS__)

#define using_csptr_2(X, Value) \
            using_csptr_3(X, Value, c_default_compare)
#define using_csptr_3(X, Value, valueCompare) \
            using_csptr_4(X, Value, valueCompare, c_trivial_del)
#define using_csptr_4(X, Value, valueCompare, valueDel) \
            _c_using_csptr(csptr_##X, Value, valueCompare, valueDel)


#define _c_using_csptr(CX, Value, valueCompare, valueDel) \
    typedef Value CX##_value_t; \
\
    typedef struct { \
        CX##_value_t* get; \
        atomic_count_t* use_count; \
    } CX; \
\
    STC_INLINE CX \
    CX##_from(CX##_value_t* p) { \
        CX ptr = {p}; \
        if (p) *(ptr.use_count = c_new(atomic_count_t)) = 1; \
        return ptr; \
    } \
\
    STC_INLINE CX \
    CX##_make(CX##_value_t val) { \
        CX ptr = {c_new(CX##_value_t), c_new(atomic_count_t)}; \
        *ptr.get = val, *ptr.use_count = 1; return ptr; \
    } \
\
    STC_INLINE CX \
    CX##_clone(CX ptr) { \
        if (ptr.use_count) atomic_increment(ptr.use_count); \
        return ptr; \
    } \
\
    STC_INLINE CX \
    CX##_move(CX* self) { \
        CX ptr = *self; \
        self->get = NULL, self->use_count = NULL; \
        return ptr; \
    } \
\
    STC_INLINE void \
    CX##_del(CX* self) { \
        if (self->use_count && atomic_decrement(self->use_count) == 0) { \
            valueDel(self->get); \
            c_free(self->use_count); \
            c_free(self->get); \
        } \
    } \
\
    STC_INLINE void \
    CX##_reset(CX* self) { \
        CX##_del(self); \
        self->use_count = NULL, self->get = NULL; \
    } \
\
    STC_INLINE CX##_value_t* \
    CX##_reset_with(CX* self, CX##_value_t* p) { \
        CX##_del(self); \
        *self = CX##_from(p); \
        return self->get; \
    } \
\
    STC_INLINE int \
    CX##_compare(CX* x, CX* y) { \
        return valueCompare(x->get, y->get); \
    } \
\
    STC_INLINE bool \
    CX##_equals(CX* x, CX* y) { \
        return valueCompare(x->get, y->get) == 0; \
    } \
    struct stc_trailing_semicolon

#endif
