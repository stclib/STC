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

#ifndef STC_TYPES_H_INCLUDED
#define STC_TYPES_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define declare_rc(C, KEY) declare_arc(C, KEY)
#define declare_rc2(SELF, VAL) declare_arc2(SELF, VAL)
#define declare_list(C, KEY) _declare_list(C, KEY,)
#define declare_stack(C, KEY) _declare_stack(C, KEY,)
#define declare_vec(C, KEY) _declare_stack(C, KEY,)
#define declare_pqueue(C, KEY) _declare_stack(C, KEY,)
#define declare_queue(C, KEY) _declare_queue(C, KEY,)
#define declare_deque(C, KEY) _declare_queue(C, KEY,)
#define declare_hashmap(C, KEY, VAL) _declare_htable(C, c_true, c_false, KEY, VAL,)
#define declare_hashset(C, KEY) _declare_htable(C, c_false, c_true, KEY, KEY,)
#define declare_sortedmap(C, KEY, VAL) _declare_aatree(C, c_true, c_false, KEY, VAL,)
#define declare_sortedset(C, KEY) _declare_aatree(C, c_false, c_true, KEY, KEY,)

#define declare_list_aux(C, KEY, AUX) _declare_list(C, KEY, AUX aux;)
#define declare_stack_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_vec_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_pqueue_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_queue_aux(C, KEY, AUX) _declare_queue(C, KEY, AUX aux;)
#define declare_deque_aux(C, KEY, AUX) _declare_queue(C, KEY, AUX aux;)
#define declare_hashmap_aux(C, KEY, VAL) _declare_htable(C, c_true, c_false, KEY, VAL, AUX aux;)
#define declare_hashset_aux(C, KEY) _declare_htable(C, c_false, c_true, KEY, KEY, AUX aux;)
#define declare_sortedmap_aux(C, KEY, VAL) _declare_aatree(C, c_true, c_false, KEY, VAL, AUX aux;)
#define declare_sortedset_aux(C, KEY) _declare_aatree(C, c_false, c_true, KEY, KEY, AUX aux;)

typedef struct { uint32_t state, codep; } cutf8_decode_t;

// csview : non-null terminated string view
typedef const char csview_value;
typedef struct csview {
    csview_value* buf;
    ptrdiff_t size;
} csview;

typedef union {
    struct { csview chr; csview_value* end; cutf8_decode_t dec; } u8;
    csview chr;
    csview_value* ref;
} csview_iter;

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(STRLIT) c_sv_2(STRLIT, c_litstrlen(STRLIT))
#define c_sv_2(str, n) (c_literal(csview){str, n})
#define c_svfmt "%.*s"
#define c_svarg(sv) (int)(sv).size, (sv).buf // printf(c_svfmt "\n", c_svarg(sv));

// zsview : zero-terminated string view
typedef csview_value zsview_value;
typedef struct zsview {
    zsview_value* str;
    ptrdiff_t size;
} zsview;

typedef union {
    struct { csview chr; cutf8_decode_t dec; } u8;
    csview chr;
    zsview_value* ref;
} zsview_iter;

#define c_zv(literal) (c_literal(zsview){literal, c_litstrlen(literal)})

// cstr : zero-terminated owning string (short string optimized - sso)
typedef char cstr_value;
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[ sizeof(cstr_buf) - 1 ]; uint8_t size; } sml;
    struct { cstr_value* data; uintptr_t size; uintptr_t ncap; } lon;
} cstr;

typedef union {
    struct { csview chr; cutf8_decode_t dec; } u8;
    csview chr; // utf8 character/codepoint
    const cstr_value* ref;
} cstr_iter;

// non-owning char pointer
typedef const char* cstr_raw;
#define cstr_raw_cmp(x, y) strcmp(*(x), *(y))
#define cstr_raw_eq(x, y)  (cstr_raw_cmp(x, y) == 0)
#define cstr_raw_hash(vp)  c_hash_str(*(vp))


#define declare_arc(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_ctrl SELF##_ctrl; \
\
    typedef union SELF { \
        SELF##_value* get; \
        SELF##_ctrl* ctrl; \
    } SELF

#define declare_arc2(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_ctrl SELF##_ctrl; \
\
    typedef struct SELF { \
        SELF##_value* get; \
        SELF##_ctrl* ctrl2; \
    } SELF

#define declare_box(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _declare_queue(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *cbuf; \
        ptrdiff_t start, end, capmask; \
        AUXDEF \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        ptrdiff_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _declare_list(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *const *_last, *prev; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_node *last; \
        AUXDEF \
    } SELF

#define _declare_htable(SELF, MAP_ONLY, SET_ONLY, KEY, VAL, AUXDEF) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value, SELF##_entry; \
\
    typedef struct { \
        SELF##_value *ref; \
        size_t idx; \
        bool inserted; \
        uint8_t hashx; \
        uint16_t dist; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref, *_end; \
        struct hmap_meta *_mref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* table; \
        struct hmap_meta* meta; \
        ptrdiff_t size, bucket_count; \
        AUXDEF \
    } SELF

#define _declare_aatree(SELF, MAP_ONLY, SET_ONLY, KEY, VAL, AUXDEF) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value, SELF##_entry; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *_d; \
        int _top; \
        int32_t _tn, _st[36]; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_node *nodes; \
        int32_t root, disp, head, size, capacity; \
        AUXDEF \
    } SELF

#define _declare_inplace_stack(SELF, VAL, CAP, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { ptrdiff_t size; SELF##_value data[CAP]; AUXDEF } SELF

#define _declare_stack(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; ptrdiff_t size, capacity; AUXDEF } SELF

#endif // STC_TYPES_H_INCLUDED
