/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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

/* cbox: heap allocated boxed type
#include <stc/cstr.h>

typedef struct { cstr name, email; } Person;

Person Person_from(const char* name, const char* email) {
    return (Person){.name = cstr_from(name), .email = cstr_from(email)};
}
Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.email = cstr_clone(p.email);
    return p;
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->email));
    c_drop(cstr, &p->name, &p->email);
}

#define i_val_bind Person // bind Person clone+drop fn's
#define i_opt c_no_cmp // compare by .get addresses only
#define i_type PBox
#include <stc/cbox.h>

int main() {
    c_auto (PBox, p, q)
    {
        p = PBox_from(Person_from("John Smiths", "josmiths@gmail.com"));
        q = PBox_clone(p);
        cstr_assign(&q.get->name, "Joe Smiths");

        printf("%s %s.\n", cstr_str(&p.get->name), cstr_str(&p.get->email));
        printf("%s %s.\n", cstr_str(&q.get->name), cstr_str(&q.get->email));
    }
}
*/
#include "ccommon.h"

#ifndef CBOX_H_INCLUDED
#define CBOX_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define cbox_null {NULL}
#endif // CBOX_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cbox_
#endif
#include "template.h"
typedef i_valraw _cx_raw;

#if !c_option(c_is_fwd)
_cx_deftypes(_c_cbox_types, _cx_self, i_val);
#endif

// constructors (takes ownsership)
STC_INLINE _cx_self
_cx_memb(_init)(void) { return c_make(_cx_self){NULL}; }

STC_INLINE _cx_self
_cx_memb(_from_ptr)(i_val* p) { return c_make(_cx_self){p}; }

STC_INLINE _cx_self
_cx_memb(_from)(i_val val) { // c++: std::make_unique<i_val>(val)
    _cx_self ptr = {c_alloc(i_val)};
    *ptr.get = val; return ptr;
}

STC_INLINE i_val
_cx_memb(_toraw)(const _cx_self* self) { return *self->get; }

// destructor
STC_INLINE void
_cx_memb(_drop)(_cx_self* self) {
    if (self->get) { i_valdrop(self->get); c_free(self->get); }
}

STC_INLINE _cx_self
_cx_memb(_move)(_cx_self* self) {
    _cx_self ptr = *self; self->get = NULL;
    return ptr;
}

STC_INLINE void
_cx_memb(_reset)(_cx_self* self) {
    _cx_memb(_drop)(self); self->get = NULL;
}

// take ownership of val
STC_INLINE void
_cx_memb(_reset_from)(_cx_self* self, i_val val) {
    if (self->get) { i_valdrop(self->get); *self->get = val; }
    else self->get = c_new(i_val, val);
}

#if !defined _i_no_clone
    STC_INLINE _cx_self
    _cx_memb(_make)(_cx_raw raw) { return _cx_memb(_from)(i_valfrom(raw)); }

    STC_INLINE _cx_self
    _cx_memb(_clone)(_cx_self other) {
        if (!other.get) return other;
        i_valraw r = i_valto(other.get);
        return c_make(_cx_self){c_new(i_val, i_valfrom(r))};
    }

    STC_INLINE void
    _cx_memb(_copy)(_cx_self* self, _cx_self other) {
        if (self->get == other.get) return;
        _cx_memb(_drop)(self);
        *self = _cx_memb(_clone)(other);
    }
#endif // !_i_no_clone

STC_INLINE void
_cx_memb(_take)(_cx_self* self, _cx_self other) {
    if (other.get != self->get) _cx_memb(_drop)(self);
    *self = other;
}

STC_INLINE uint64_t
_cx_memb(_value_hash)(const _cx_value* x, size_t n) {
    #if c_option(c_no_cmp) && UINTPTR_MAX == UINT64_MAX
        return c_hash64(&x, 8);
    #elif c_option(c_no_cmp)
        return c_hash32(&x, 4);
    #else
        _cx_raw rx = i_valto(x);
        return i_hash((&rx), (sizeof rx));
    #endif
}

STC_INLINE int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    #if c_option(c_no_cmp)
        return c_default_cmp(&x, &y);
    #else
        _cx_raw rx = i_valto(x), ry = i_valto(y);
        return i_cmp((&rx), (&ry));
    #endif
}

STC_INLINE bool
_cx_memb(_value_eq)(const _cx_value* x, const _cx_value* y) {
    #if c_option(c_no_cmp)
        return x == y;
    #else
        _cx_raw rx = i_valto(x), ry = i_valto(y);
        return i_eq((&rx), (&ry));
    #endif
}
#include "template.h"
