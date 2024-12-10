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
/*
// https://stackoverflow.com/questions/70935435/how-to-create-variants-in-rust
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/algorithm.h"

typedef struct {
    int32 (*func)(int32, int32);
    int32 v1, v2;
} RunFunc;

c_variant_type (Action,
    (ActionSpeak, cstr),
    (ActionQuit, bool),
    (ActionRunFunc, RunFunc)
);

void Action_drop(Action* self) {
    c_match (self) {
        c_of(ActionSpeak, x) cstr_drop(x);
    }
}

int32 add(int32 a, int32 b) {
    return a + b;
}

int main(void) {
    Action act1 = c_variant(ActionSpeak, cstr_from("Hello"));
    Action act2 = c_variant(ActionQuit, true);
    Action act3 = c_variant(ActionRunFunc, {add, 29, 13});

    c_assert(c_variant_holds(ActionQuit, &act2));
    printf("ActionQuit tag: %d\n", c_variant_tag(&act2));

    c_foritems (i, Action*, {&act1, &act2, &act3})
    c_match (*i.ref) {
        c_of(ActionSpeak, x) {
            printf("Asked to speak: %s\n", cstr_str(x));
        }
        c_of(ActionQuit, x) {
            printf("Asked to quit!\n");
        }
        c_of(ActionRunFunc, r) {
            int32 res = r->func(r->v1, r->v2);
            printf("v1: %d, v2: %d, res: %d\n", r->v1, r->v2, res);
        }
        c_otherwise assert(!"no match");
    }

    c_drop(Action, &act1, &act2, &act3);
}
*/
#ifndef STC_VARIANT_H_INCLUDED
#define STC_VARIANT_H_INCLUDED

#include "stc/common.h"

#define c_STRIP_PARENS(X) _c_STRIP_PARENS( _c_E1 X )
#define c_CALL(f, ...) f(__VA_ARGS__)
#define _c_STRIP_PARENS(X) X
#define _c_EMPTY()
#define _c_TUPLE_AT_1(x,y,...) y
#define _c_CHECK(x,...) _c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP_INDIRECTION() c_LOOP
#define _c_LOOP0(T,f,x,...) c_CALL(f, T, c_STRIP_PARENS(x)) _c_LOOP_INDIRECTION _c_EMPTY()() (T,f,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_E1(...) __VA_ARGS__
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define c_EVAL(...) _c_E2(_c_E2(_c_E2(_c_E2(_c_E2(_c_E2(__VA_ARGS__))))))
#define c_LOOP(T,f,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS x))(T,f,x,__VA_ARGS__)

#define _c_vartuple_tag(T, Ident, Type) Ident##_tag,
#define _c_vartuple_type(T, Ident, Type) typedef Type Ident##_type; typedef T Ident##_variant;
#define _c_vartuple_var(T, Ident, Type) struct { uint8_t _tag; Ident##_type _var; } Ident;

#define c_variant_type(T, ...) \
    typedef union T T; \
    c_EVAL(c_LOOP(T, _c_vartuple_type, __VA_ARGS__, (0))) \
    enum { T##_nulltag, c_EVAL(c_LOOP(T, _c_vartuple_tag, __VA_ARGS__, (0))) }; \
    union T { \
        struct { uint8_t _tag; } _current; \
        c_EVAL(c_LOOP(T, _c_vartuple_var, __VA_ARGS__, (0))) \
    }

#define c_match(variant) \
    for (void *_match = (void *)(variant); _match != NULL; _match = NULL) \
    switch ((variant)->_current._tag)

#define c_of(Ident, x) \
    break; case Ident##_tag: \
    for (Ident##_type *x = &((Ident##_variant *)_match)->Ident._var; x != NULL; x = NULL)

#define c_otherwise \
    break; default:

#define c_variant(Ident, ...) \
    ((Ident##_variant){.Ident={._tag=Ident##_tag, ._var=__VA_ARGS__}})

#define c_variant_holds(Ident, variant) \
    ((variant)->Ident._tag == Ident##_tag)

#define c_variant_tag(variant) \
    ((variant)->_current._tag)

#endif // STC_VARIANT_H_INCLUDED
