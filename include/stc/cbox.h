
/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#include <stc/cstr.h>
#include <stc/algo/raii.h> // c_auto

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
#include <stc/cbox.h>

int main(void) {
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
#include "priv/linkage.h"

#ifndef CBOX_H_INCLUDED
#define CBOX_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define cbox_null {0}
#endif // CBOX_H_INCLUDED

#define _i_prefix cbox_
#define _i_cbox
#include "priv/template.h"
typedef i_keyraw _cx_raw;

#ifndef i_is_forward
_cx_DEFTYPES(_c_cbox_types, _cx_Self, i_key);
#endif

// constructors (take ownership)
STC_INLINE _cx_Self _cx_MEMB(_init)(void)
    { return c_LITERAL(_cx_Self){NULL}; }

STC_INLINE long _cx_MEMB(_use_count)(const _cx_Self* self)
    { return (long)(self->get != NULL); }

STC_INLINE _cx_Self _cx_MEMB(_from_ptr)(_cx_value* p)
    { return c_LITERAL(_cx_Self){p}; }

// c++: std::make_unique<i_key>(val)
STC_INLINE _cx_Self _cx_MEMB(_make)(_cx_value val) {
    _cx_Self ptr = {_i_alloc(_cx_value)};
    *ptr.get = val; return ptr;
}

STC_INLINE _cx_raw _cx_MEMB(_toraw)(const _cx_Self* self)
    { return i_keyto(self->get); }

// destructor
STC_INLINE void _cx_MEMB(_drop)(_cx_Self* self) {
    if (self->get) {
        i_keydrop(self->get);
        i_free(self->get);
    }
}

STC_INLINE _cx_Self _cx_MEMB(_move)(_cx_Self* self) {
    _cx_Self ptr = *self; 
    self->get = NULL;
    return ptr;
}

STC_INLINE _cx_value* _cx_MEMB(_release)(_cx_Self* self)
    { return _cx_MEMB(_move)(self).get; }

STC_INLINE void _cx_MEMB(_reset)(_cx_Self* self) {
    _cx_MEMB(_drop)(self);
    self->get = NULL;
}

// take ownership of p
STC_INLINE void _cx_MEMB(_reset_to)(_cx_Self* self, _cx_value* p) {
    _cx_MEMB(_drop)(self);
    self->get = p;
}

#ifndef i_no_emplace
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_raw raw)
    { return _cx_MEMB(_make)(i_keyfrom(raw)); }
#else
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_value val)
    { return _cx_MEMB(_make)(val); }
#endif

#if !defined i_no_clone
    STC_INLINE _cx_Self _cx_MEMB(_clone)(_cx_Self other) {
        if (!other.get)
            return other;
        _cx_Self out = {_i_alloc(i_key)};
        *out.get = i_keyclone((*other.get));
        return out;
    }
#endif // !i_no_clone

// take ownership of unowned
STC_INLINE void _cx_MEMB(_take)(_cx_Self* self, _cx_Self unowned) {
    _cx_MEMB(_drop)(self);
    *self = unowned;
}
// transfer ownership from moved; set moved to NULL
STC_INLINE void _cx_MEMB(_assign)(_cx_Self* self, _cx_Self* moved) {
    if (moved->get == self->get)
        return;
    _cx_MEMB(_drop)(self);
    *self = *moved;
    moved->get = NULL;
}

#if defined i_use_cmp
    STC_INLINE int _cx_MEMB(_raw_cmp)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_cmp((&rx), (&ry));
    }

    STC_INLINE bool _cx_MEMB(_raw_eq)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_eq((&rx), (&ry));
    }

    #ifndef i_no_hash
    STC_INLINE uint64_t _cx_MEMB(_raw_hash)(const _cx_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { _cx_raw rx = i_keyto(self->get); return i_hash((&rx)); }
    #endif // i_no_hash

#else

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        return c_default_cmp(&self->get, &other->get);
    }
    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        return self->get == other->get;
    }
    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { return c_default_hash(&self->get); }
#endif // i_use_cmp

#include "priv/template2.h"
#undef _i_cbox
