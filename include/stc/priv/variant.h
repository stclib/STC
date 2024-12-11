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

c_sumtype (Action,
    (ActionSpeak, cstr),
    (ActionQuit, _Bool),
    (ActionRunFunc, struct {
        int32 (*func)(int32, int32);
        int32 v1, v2;
    })
);

void Action_drop(Action* self) {
    c_match (self) {
        c_of(ActionSpeak, x) cstr_drop(x);
    }
}

void action(Action* action) {
    c_match (action) {
        c_of(ActionSpeak, s) {
            printf("Asked to speak: %s\n", cstr_str(s));
        }
        c_of(ActionQuit, _) {
            printf("Asked to quit!\n");
        }
        c_of(ActionRunFunc, r) {
            int32 res = r->func(r->v1, r->v2);
            printf("v1: %d, v2: %d, res: %d\n", r->v1, r->v2, res);
        }
        c_otherwise assert(!"no match");
    }
}

int32 add(int32 a, int32 b) {
    return a + b;
}

int main(void) {
    Action act1 = c_variant(ActionSpeak, cstr_from("Hello"));
    Action act2 = c_variant(ActionQuit, 1);
    Action act3 = c_variant(ActionRunFunc, {add, 5, 6});

    action(&act1);
    action(&act2);
    action(&act3);

    c_drop(Action, &act1, &act2, &act3);
}
*/
#ifndef STC_VARIANT_H_INCLUDED
#define STC_VARIANT_H_INCLUDED

#include "stc/common.h"

#define _c_EMPTY()
#define _c_CALL(f, ...) f(__VA_ARGS__)
#define _c_LOOP_INDIRECTION() c_LOOP
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP0(f,T,x,...) _c_CALL(f,T,c_EXPAND x) _c_LOOP_INDIRECTION _c_EMPTY()()(f,T,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_TUPLE_AT_1(x,y,...) y
#define _c_CHECK(x,...) _c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_E1(...) __VA_ARGS__
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define c_EVAL(...) _c_E2(_c_E2(_c_E2(_c_E2(_c_E2(_c_E2(__VA_ARGS__))))))
#define c_LOOP(f,T,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS x))(f,T,x,__VA_ARGS__)

#define _c_vartuple_tag(T, Choice, ...) Choice##_tag,
#define _c_vartuple_type(T, Choice, ...) typedef __VA_ARGS__ Choice##_type; typedef T Choice##_sumtype;
#define _c_vartuple_var(T, Choice, ...) struct { uint8_t _tag; Choice##_type _var; } Choice;

typedef union { struct { uint8_t _tag; } _current; } c_base_variant;

#define c_sumtype(T, ...) \
    typedef union T T; \
    c_EVAL(c_LOOP(_c_vartuple_type, T,  __VA_ARGS__, (0),)) \
    enum { T##_nulltag, c_EVAL(c_LOOP(_c_vartuple_tag, T, __VA_ARGS__, (0),)) }; \
    union T { \
        struct { uint8_t _tag; } _current; \
        c_EVAL(c_LOOP(_c_vartuple_var, T, __VA_ARGS__, (0),)) \
    }

#define c_match(variant) \
    for (c_base_variant* _match = (c_base_variant *)(variant) + 0*sizeof((variant)->_current._tag) \
         ; _match != NULL ; _match = NULL) \
    switch (_match->_current._tag)

#define c_of(Choice, x) \
    break; case Choice##_tag: \
    for (Choice##_type *x = &((Choice##_sumtype *)_match)->Choice._var; x != NULL; x = NULL)

#define c_otherwise \
    break; default:

#define c_variant(Choice, ...) \
    ((Choice##_sumtype){.Choice={._tag=Choice##_tag, ._var=__VA_ARGS__}})

#define c_variant_holds(Choice, variant) \
    ((variant)->Choice._tag == Choice##_tag)

#define c_variant_tag(variant) \
    ((variant)->_current._tag)

#endif // STC_VARIANT_H_INCLUDED
