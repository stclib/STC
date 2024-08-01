
/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
#define i_implement
#include "stc/cstr.h"

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

#define i_type PBox
#define i_valclass Person // bind Person clone+drop fn's
#include "stc/box.h"

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

#ifndef STC_BOX_H_INCLUDED
#define STC_BOX_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>

#define cbox_null {0}
#endif // STC_BOX_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix box_
#endif
#include "priv/template.h"
typedef i_keyraw _m_raw;

#ifndef i_is_forward
_c_DEFTYPES(_c_box_types, _i_self, i_key);
#endif

// constructors (take ownership)
STC_INLINE _i_self _c_MEMB(_init)(void)
    { return c_LITERAL(_i_self){NULL}; }

STC_INLINE long _c_MEMB(_use_count)(const _i_self* self)
    { return (long)(self->get != NULL); }

STC_INLINE _i_self _c_MEMB(_from_ptr)(_m_value* p)
    { return c_LITERAL(_i_self){p}; }

// c++: std::make_unique<i_key>(val)
STC_INLINE _i_self _c_MEMB(_make)(_m_value val) {
    _i_self box = {_i_malloc(_m_value, 1)};
    *box.get = val;
    return box;
}

STC_INLINE _m_raw _c_MEMB(_toraw)(const _i_self* self)
    { return i_keyto(self->get); }

// destructor
STC_INLINE void _c_MEMB(_drop)(const _i_self* cself) {
    _i_self* self = (_i_self*)cself;
    i_keydrop(self->get);
    i_free(self->get, c_sizeof *self->get);
}

STC_INLINE _i_self _c_MEMB(_move)(_i_self* self) {
    _i_self box = *self;
    self->get = NULL;
    return box;
}

STC_INLINE _m_value* _c_MEMB(_release)(_i_self* self)
    { return _c_MEMB(_move)(self).get; }

// take ownership of p
STC_INLINE void _c_MEMB(_reset_to)(_i_self* self, _m_value* p) {
    _c_MEMB(_drop)(self);
    self->get = p;
}

STC_INLINE _i_self _c_MEMB(_from)(_m_raw raw)
    { return _c_MEMB(_make)(i_keyfrom(raw)); }

#if !defined i_no_clone
    STC_INLINE _i_self _c_MEMB(_clone)(_i_self other) {
        _i_self out = {_i_malloc(_m_value, 1)};
        *out.get = i_keyclone((*other.get));
        return out;
    }
#endif // !i_no_clone

// take ownership of unowned
STC_INLINE void _c_MEMB(_take)(_i_self* self, _i_self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}
// transfer ownership from moved; set moved to NULL
STC_INLINE void _c_MEMB(_assign)(_i_self* self, _i_self* moved) {
    if (moved->get == self->get)
        return;
    _c_MEMB(_drop)(self);
    *self = *moved;
    moved->get = NULL;
}

#if defined _i_has_cmp
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _c_MEMB(_cmp)(const _i_self* self, const _i_self* other) {
        _m_raw rawx = i_keyto(self->get), rawy = i_keyto(other->get);
        return i_cmp((&rawx), (&rawy));
    }
#else
    STC_INLINE int _c_MEMB(_cmp)(const _i_self* self, const _i_self* other) {
        const _m_value *x = self->get, *y = other->get;
        return (x > y) - (x < y);
    }
#endif

#if defined _i_has_eq
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _c_MEMB(_eq)(const _i_self* self, const _i_self* other) {
        _m_raw rawx = i_keyto(self->get), rawy = i_keyto(other->get);
        return i_eq((&rawx), (&rawy));
    }
#else
    STC_INLINE bool _c_MEMB(_eq)(const _i_self* self, const _i_self* other)
        { return self->get == other->get; }
#endif

#if !defined i_no_hash && defined _i_has_eq
    STC_INLINE uint64_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _c_MEMB(_hash)(const _i_self* self) {
        _m_raw raw = i_keyto(self->get);
        return i_hash((&raw));
    }
#else
    STC_INLINE uint64_t _c_MEMB(_hash)(const _i_self* self)
        { return c_default_hash(&self->get); }
#endif // i_no_hash

#include "priv/linkage2.h"
#include "priv/template2.h"
