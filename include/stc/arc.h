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

/* carc: atomic reference counted shared_ptr
#define i_implement
#include "stc/cstr.h"

typedef struct { cstr name, last; } Person;

Person Person_make(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    cstr_drop(&p->name);
    cstr_drop(&p->last);
}

#define i_type ArcPers
#define i_val_class Person    // clone, drop, cmp, hash
#include "stc/arc.h"

int main(void) {
    ArcPers p = ArcPers_from(Person_make("John", "Smiths"));
    ArcPers q = ArcPers_clone(p); // share the pointer

    printf("%s %s. uses: %ld\n", cstr_str(&q.get->name), cstr_str(&q.get->last), *q.use_count);
    c_drop(ArcPers, &p, &q);
}
*/
#include "priv/linkage.h"

#ifndef STC_ARC_H_INCLUDED
#define STC_ARC_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>

#if defined _MSC_VER
    #include <intrin.h>
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#elif defined __GNUC__ || defined __clang__
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#else
    #include <stdatomic.h>
    #define c_atomic_inc(v) (void)atomic_fetch_add(v, 1)
    #define c_atomic_dec_and_test(v) (atomic_fetch_sub(v, 1) == 1)
#endif

#define carc_null {0}
#endif // STC_ARC_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix arc_
#endif
#define _i_carc
#include "priv/template.h"
typedef i_keyraw _m_raw;

#if c_option(c_no_atomic)
  #define i_no_atomic
#endif
#if !defined i_no_atomic
  #define _i_atomic_inc(v)          c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#else
  #define _i_atomic_inc(v)          (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#endif
#ifndef i_is_forward
_c_DEFTYPES(_c_arc_types, i_type, i_key);
#endif
struct _c_MEMB(_metadata_) { catomic_long counter; bool value_included; };
struct _c_MEMB(_rep_) { struct _c_MEMB(_metadata_) metadata; i_key value; };

STC_INLINE i_type _c_MEMB(_init)(void)
    { return c_LITERAL(i_type){NULL, NULL}; }

STC_INLINE long _c_MEMB(_use_count)(const i_type* self)
    { return self->use_count ? *self->use_count : 0; }

STC_INLINE i_type _c_MEMB(_from_ptr)(_m_value* p) {
    i_type arc = {p};
    if (p) {
        struct _c_MEMB(_metadata_)* metadata = _i_alloc(struct _c_MEMB(_metadata_));
        metadata->value_included = false;
        *(arc.use_count = &metadata->counter) = 1;
    }
    return arc;
}

// c++: std::make_shared<_m_value>(val)
STC_INLINE i_type _c_MEMB(_make)(_m_value val) {
    i_type arc;
    struct _c_MEMB(_rep_)* rep = _i_alloc(struct _c_MEMB(_rep_));
    rep->metadata.value_included = true;
    *(arc.use_count = &rep->metadata.counter) = 1;
    *(arc.get = &rep->value) = val;
    return arc;
}

STC_INLINE _m_raw _c_MEMB(_toraw)(const i_type* self)
    { return i_keyto(self->get); }

STC_INLINE i_type _c_MEMB(_move)(i_type* self) {
    i_type arc = *self;
    self->get = NULL, self->use_count = NULL;
    return arc;
}

STC_INLINE void _c_MEMB(_drop)(const i_type* cself) {
    i_type* self = (i_type*)cself;
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_keydrop(self->get);
        if (!c_container_of(self->use_count, struct _c_MEMB(_metadata_), counter)->value_included) {
            i_free(self->get, c_sizeof *self->get);
            i_free(self->use_count, c_sizeof(long));
        } else { // allocated combined counter+value with _make()
            i_free(self->use_count, c_sizeof(struct _c_MEMB(_rep_)));
        }
    }
}

STC_INLINE void _c_MEMB(_reset)(i_type* self) {
    _c_MEMB(_drop)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void _c_MEMB(_reset_to)(i_type* self, _m_value* p) {
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_from_ptr)(p);
}

#ifndef i_no_emplace
STC_INLINE i_type _c_MEMB(_from)(_m_raw raw)
    { return _c_MEMB(_make)(i_keyfrom(raw)); }
#else
STC_INLINE i_type _c_MEMB(_from)(_m_value val)
    { return _c_MEMB(_make)(val); }
#endif

// does not use i_keyclone, so OK to always define.
STC_INLINE i_type _c_MEMB(_clone)(i_type arc) {
    if (arc.use_count)
        _i_atomic_inc(arc.use_count);
    return arc;
}

// take ownership of unowned
STC_INLINE void _c_MEMB(_take)(i_type* self, i_type unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}
// share ownership with arc
STC_INLINE void _c_MEMB(_assign)(i_type* self, i_type arc) {
    if (arc.use_count)
        _i_atomic_inc(arc.use_count);
    _c_MEMB(_drop)(self);
    *self = arc;
}

#if defined _i_has_cmp
    STC_INLINE int _c_MEMB(_raw_cmp)(const _m_raw* rx, const _m_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _c_MEMB(_cmp)(const i_type* self, const i_type* other) {
        _m_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_cmp((&rx), (&ry));
    }
#else
    STC_INLINE int _c_MEMB(_cmp)(const i_type* self, const i_type* other)
        { return c_default_cmp(&self->get, &other->get); }
#endif

#if defined _i_has_eq
    STC_INLINE bool _c_MEMB(_raw_eq)(const _m_raw* rx, const _m_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _c_MEMB(_eq)(const i_type* self, const i_type* other) {
        _m_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_eq((&rx), (&ry));
    }
#else
    STC_INLINE bool _c_MEMB(_eq)(const i_type* self, const i_type* other)
        { return self->get == other->get; }
#endif

#ifndef i_no_hash
    STC_INLINE uint64_t _c_MEMB(_raw_hash)(const _m_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _c_MEMB(_hash)(const i_type* self) {
        _m_raw rx = i_keyto(self->get);
        return i_hash((&rx));
    }
#else
    STC_INLINE uint64_t _c_MEMB(_hash)(const i_type* self)
        { return c_default_hash(&self->get); }
#endif // i_no_hash

#undef i_no_atomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#undef _i_carc
#include "priv/template2.h"
#include "priv/linkage2.h"
