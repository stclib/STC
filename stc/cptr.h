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

#include "cdefs.h"

/* cptr: std::unique_ptr -like type: */
/*
#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr(name), p->last = cstr(last);
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
        printf("%s %s\n", (*i.val)->name.str, (*i.val)->last.str);
    cvec_pe_del(&vec);
}
*/

#define using_cptr(...) c_MACRO_OVERLOAD(using_cptr, __VA_ARGS__)

#define using_cptr_3(X, Value, valueDestroy) \
    typedef Value cptr_##X##_value_t; \
    typedef cptr_##X##_value_t *cptr_##X; \
\
    STC_INLINE void \
    cptr_##X##_del(cptr_##X* self) { \
        valueDestroy(*self); \
        c_free(*self); \
    } \
\
    STC_INLINE void \
    cptr_##X##_reset(cptr_##X* self, cptr_##X##_value_t* p) { \
        cptr_##X##_del(self); \
        *self = p; \
    } \
    typedef cptr_##X cptr_##X##_t

#define using_cptr_4(X, Value, valueDestroy, valueCompare) \
    using_cptr_3(X, Value, valueDestroy); \
    STC_INLINE int \
    cptr_##X##_compare(cptr_##X* x, cptr_##X* y) { \
        return valueCompare(*x, *y); \
    } \
    typedef int cptr_##X##_dud4

#endif
 