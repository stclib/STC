
/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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

#define T PBox, Person, (c_keyclass) // bind Person clone+drop fn's
#include <stc/box.h>

int main(void) {
    PBox p = PBox_from(Person_from("John Smiths", "josmiths@gmail.com"));
    PBox q = PBox_clone(p);
    cstr_assign(&q.get->name, "Joe Smiths");

    printf("%s %s.\n", cstr_str(&p.get->name), cstr_str(&p.get->email));
    printf("%s %s.\n", cstr_str(&q.get->name), cstr_str(&q.get->email));

    c_drop(PBox, &p, &q);
}
*/
#include "priv/linkage.h"
#include "types.h"

#ifndef STC_BOX_H_INCLUDED
#define STC_BOX_H_INCLUDED
#include "common.h"
#include <stdlib.h>

#define cbox_null {0}
#endif // STC_BOX_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix box_
#endif
#define _i_is_box
#include "priv/template.h"
typedef i_keyraw _m_raw;

#ifndef i_declared
_c_DEFTYPES(declare_box, Self, i_key);
#endif

// constructors (take ownership)
STC_INLINE Self _c_MEMB(_init)(void)
    { return c_literal(Self){0}; }

STC_INLINE long _c_MEMB(_use_count)(const Self* self)
    { return (long)(self->get != NULL); }


// c++: std::make_unique<i_key>(val)
STC_INLINE Self _c_MEMB(_make)(_m_value val) {
    Self box = {_i_new_n(_m_value, 1)};
    *box.get = val;
    return box;
}

STC_INLINE Self _c_MEMB(_from_ptr)(_m_value* p)
    { return c_literal(Self){p}; }

STC_INLINE Self _c_MEMB(_from)(_m_raw raw)
    { return _c_MEMB(_make)(i_keyfrom(raw)); }

STC_INLINE _m_raw _c_MEMB(_toraw)(const Self* self)
    { return i_keytoraw(self->get); }

// destructor
STC_INLINE void _c_MEMB(_drop)(const Self* self) {
    if (self->get) {
        i_keydrop(self->get);
        i_free(self->get, c_sizeof *self->get);
    }
}

// move ownership to receiving box
STC_INLINE Self _c_MEMB(_move)(Self* self) {
    Self box = *self;
    self->get = NULL;
    return box;
}

// release owned pointer, must be manually freed by receiver
STC_INLINE _m_value* _c_MEMB(_release)(Self* self)
    { return _c_MEMB(_move)(self).get; }

// take ownership of pointer p
STC_INLINE void _c_MEMB(_reset_to)(Self* self, _m_value* p) {
    _c_MEMB(_drop)(self);
    self->get = p;
}

// take ownership of unowned box
STC_INLINE void _c_MEMB(_take)(Self* self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

// transfer ownership from other; set other to NULL
STC_INLINE void _c_MEMB(_assign)(Self* self, Self* owned) {
    if (owned->get == self->get)
        return;
    _c_MEMB(_drop)(self);
    *self = *owned;
    owned->get = NULL;
}

#ifndef i_no_clone
    STC_INLINE Self _c_MEMB(_clone)(Self other) {
        if (other.get == NULL) return other;
        Self out = {_i_new_n(_m_value, 1)};
        *out.get = i_keyclone((*other.get));
        return out;
    }
#endif // !i_no_clone


#if defined _i_has_cmp
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return i_cmp(rx, ry); }
#endif

#if defined _i_has_eq
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return i_eq(rx, ry); }
#endif

#if !defined i_no_hash && defined _i_has_eq
    STC_INLINE size_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return i_hash(rx); }
#endif // i_no_hash
#undef _i_is_box
#include "sys/finalize.h"
