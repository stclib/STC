/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
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
/*
// https://stackoverflow.com/questions/70935435/how-to-create-variants-in-rust
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/algorithm.h>

c_union (Action,
    (ActionSpeak, cstr),
    (ActionQuit, bool),
    (ActionRunFunc, struct {
        int32_t (*func)(int32_t, int32_t);
        int32_t v1, v2;
    })
);

void Action_drop(Action* self) {
    c_when (self, ActionSpeak, s) {
        cstr_drop(s);
    }
}

void my_action(Action* action) {
    c_when (action) {
        c_is(ActionSpeak, s) {
            printf("Asked to speak: %s\n", cstr_str(s));
        }
        c_is(ActionQuit) {
            printf("Asked to quit!\n");
        }
        c_is(ActionRunFunc, r) {
            int32_t res = r->func(r->v1, r->v2);
            printf("v1: %d, v2: %d, res: %d\n", r->v1, r->v2, res);
        }
        c_otherwise assert(!"no match");
    }
}

int32_t add(int32_t a, int32_t b) {
    return a + b;
}

int main(void) {
    Action act1 = c_variant(ActionSpeak, cstr_from("Hello"));
    Action act2 = c_variant(ActionQuit, 1);
    Action act3 = c_variant(ActionRunFunc, {add, 5, 6});

    my_action(&act1);
    my_action(&act2);
    my_action(&act3);

    c_drop(Action, &act1, &act2, &act3);
}
*/
#ifndef STC_SUMTYPE_H_INCLUDED
#define STC_SUMTYPE_H_INCLUDED

#include "../common.h"

#define _c_EMPTY()
#define _c_LOOP_INDIRECTION() _c_LOOP
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP0(f,U,x,...) f c_EXPAND((U, c_EXPAND x)) _c_LOOP_INDIRECTION _c_EMPTY()()(f,U,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_CHECK(x,...) c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_E0(...) __VA_ARGS__
#define _c_E1(...) _c_E0(_c_E0(_c_E0(_c_E0(_c_E0(_c_E0(__VA_ARGS__))))))
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define _c_EVAL(...) _c_E2(_c_E2(_c_E2(__VA_ARGS__))) // currently supports up to 130 variants
#define _c_LOOP(f,U,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS(c_EXPAND x)))(f,U,x,__VA_ARGS__)

#define _c_enum_1(x,...) (x=__LINE__*1000, __VA_ARGS__)
#define _c_vartuple_enum(U, Tag, ...) Tag,
#define _c_vartuple_type(U, Tag, ...) typedef __VA_ARGS__ Tag##_type; typedef U Tag##_sumtype;
#define _c_vartuple_item(U, Tag, ...) struct { enum U##_tag id; Tag##_type item; } Tag;

#define c_union(U, ...) \
    typedef union U U; \
    enum U##_tag { _c_EVAL(_c_LOOP(_c_vartuple_enum, U, _c_enum_1 __VA_ARGS__, (0),)) }; \
    _c_EVAL(_c_LOOP(_c_vartuple_type, U, __VA_ARGS__, (0),)) \
    union U { \
        int tag_id; \
        enum U##_tag tag; \
        _c_EVAL(_c_LOOP(_c_vartuple_item, U, __VA_ARGS__, (0),)) \
    }

#define c_sumtype c_union // [deprecated]?

#define c_when(...) c_MACRO_OVERLOAD(c_when, __VA_ARGS__)
#define c_when_3(varptr, Tag, x) \
    for (Tag##_type *x = &(varptr)->Tag.item \
        ; x && c_container_of(x, Tag##_sumtype, Tag.item)->Tag.id == Tag \
        ; x = NULL)

#define c_is(...) c_MACRO_OVERLOAD(c_is, __VA_ARGS__)
#define c_is_1(Tag) break; case Tag:
#define c_or_is(Tag) ; case Tag:

#define c_otherwise \
    break; default:

#ifdef STC_HAS_TYPEOF
    #define c_when_1(varptr) \
        for (c_typeof(varptr) _vp1 = (varptr); _vp1; _vp1 = NULL) \
        switch (_vp1->tag)

    #define c_is_2(Tag, x) \
        break; case Tag: \
        for (c_typeof(_vp1->Tag.item)* x = &_vp1->Tag.item; x; x = NULL)
#else
    #define c_when_1(varptr) \
        for (const int* _vp_id = &(varptr)->tag_id; _vp_id; _vp_id = NULL) \
            switch (*_vp_id)

    #define c_is_2(Tag, x) \
        break; case Tag: \
        for (Tag##_type *x = &c_container_of(_vp_id, Tag##_sumtype, tag_id)->Tag.item; x; x = NULL)
#endif

#define c_variant(Tag, ...) \
    (c_literal(Tag##_sumtype){.Tag={.id=Tag, .item=__VA_ARGS__}})

#define c_is_variant(varptr, Tag) \
    ((varptr)->Tag.id == Tag)

#define c_get_if(varptr, Tag) \
    ((varptr)->Tag.id == Tag ? &(varptr)->Tag.item : NULL)

#define c_variant_id(varptr) \
    ((varptr)->tag)

#endif // STC_SUMTYPE_H_INCLUDED
