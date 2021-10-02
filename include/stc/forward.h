/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stddef.h>

#define forward_carr2(CX, VAL) _c_carr2_types(CX, VAL)
#define forward_carr3(CX, VAL) _c_carr3_types(CX, VAL)
#define forward_cdeq(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_clist(CX, VAL) _c_clist_types(CX, VAL)
#define forward_cmap(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, c_true, c_false)
#define forward_csmap(CX, KEY, VAL) _c_aatree_types(CX, KEY, VAL, c_true, c_false)
#define forward_cset(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, c_false, c_true)
#define forward_csset(CX, KEY) _c_aatree_types(CX, KEY, KEY, c_false, c_true)
#define forward_csptr(CX, VAL) _c_csptr_types(CX, VAL)
#define forward_cpque(CX, VAL) _c_cpque_types(CX, VAL)
#define forward_cstack(CX, VAL) _c_cstack_types(CX, VAL)
#define forward_cqueue(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_cvec(CX, VAL) _c_cvec_types(CX, VAL)

#ifndef MAP_SIZE_T
#define MAP_SIZE_T uint32_t
#endif
#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_carr2_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct { SELF##_value_t *ref; } SELF##_iter_t; \
    typedef struct { SELF##_value_t **data; size_t xdim, ydim; } SELF

#define _c_carr3_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct { SELF##_value_t *ref; } SELF##_iter_t; \
    typedef struct { SELF##_value_t ***data; size_t xdim, ydim, zdim; } SELF

#define _c_cdeq_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct {SELF##_value_t *ref; } SELF##_iter_t; \
    typedef struct {SELF##_value_t *_base, *data;} SELF

#define _c_clist_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct SELF##_node_t SELF##_node_t; \
\
    typedef struct { \
        SELF##_value_t *ref; \
        SELF##_node_t *const *_last, *prev; \
    } SELF##_iter_t; \
\
    typedef struct { \
        SELF##_node_t *last; \
    } SELF

#define _c_chash_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key_t; \
    typedef VAL SELF##_mapped_t; \
    typedef MAP_SIZE_T SELF##_size_t; \
\
    typedef SET_ONLY( SELF##_key_t ) \
            MAP_ONLY( struct SELF##_value_t ) \
    SELF##_value_t; \
\
    typedef struct { \
        SELF##_value_t *ref; \
        bool inserted; \
    } SELF##_result_t; \
\
    typedef struct { \
        SELF##_value_t *ref; \
        uint8_t* _hx; \
    } SELF##_iter_t; \
\
    typedef struct { \
        SELF##_value_t* table; \
        uint8_t* _hashx; \
        SELF##_size_t size, bucket_count; \
        float max_load_factor; \
    } SELF

#define _c_aatree_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key_t; \
    typedef VAL SELF##_mapped_t; \
    typedef MAP_SIZE_T SELF##_size_t; \
    typedef struct SELF##_node_t SELF##_node_t; \
\
    typedef SET_ONLY( SELF##_key_t ) \
            MAP_ONLY( struct SELF##_value_t ) \
    SELF##_value_t; \
\
    typedef struct { \
        SELF##_value_t *ref; \
        bool inserted; \
    } SELF##_result_t; \
\
    typedef struct { \
        SELF##_value_t *ref; \
        SELF##_node_t *_d; \
        int _top; \
        SELF##_size_t _tn, _st[36]; \
    } SELF##_iter_t; \
\
    typedef struct { \
        SELF##_node_t *nodes; \
    } SELF

#define _c_csptr_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
\
    typedef struct { \
        SELF##_value_t* get; \
        long* use_count; \
    } SELF

#define _c_cstack_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct { SELF##_value_t *ref; } SELF##_iter_t; \
    typedef struct SELF { \
        SELF##_value_t* data; \
        size_t size, capacity; \
    } SELF

#define _c_cpque_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct SELF { \
        SELF##_value_t* data; \
        size_t size, capacity; \
    } SELF

#define _c_cvec_types(SELF, VAL) \
    typedef VAL SELF##_value_t; \
    typedef struct { SELF##_value_t *ref; } SELF##_iter_t; \
    typedef struct { SELF##_value_t *data; } SELF

#endif // STC_FORWARD_H_INCLUDED
