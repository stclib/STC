/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CARRAY__H__
#define CARRAY__H__

#include <stdlib.h>
#include "cdefs.h"

/*
 Multi-dimensional generic array allocated as one block of heap-memory.
 // demo:    
#include <stc/carray.h>
declare_carray(f, float);

int main()
{
    carray3f a3 = carray3f_make(30, 20, 10, 0.f);
    *carray3f_at(a3, 5, 4, 3) = 10.2f;         // a3[5][4][3]
    carray2f a2 = carray3f_at1(a3, 5);         // sub-array reference: a2 = a3[5]
    printf("%f\n", *carray2f_at(a2, 4, 3));    // lookup a2[4][3] (=10.2f)
    printf("%f\n", *carray3f_at(a3, 5, 4, 3)); // same data location, via a3 array.

    carray2f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3f_destroy(&a3); // also invalidates a2.
}
*/

#define carray1_xdim(a)  ((a)._xdim & _carray_SUB)
#define carray1_size(a) carray1_xdim(a)

#define carray2_xdim(a)  carray1_xdim(a)
#define carray2_ydim(a)  _carray2_ydim(&(a)._yxdim)
#define carray2_size(a) ((a)._yxdim)

#define carray3_xdim(a)  carray1_xdim(a)
#define carray3_ydim(a)  carray2_ydim(a)
#define carray3_zdim(a)  ((a)._zdim)
#define carray3_size(a) _carray3_size(&(a)._zdim)

#define _carray_SUB (SIZE_MAX >> 1)
#define _carray_OWN (_carray_SUB + 1)

STC_INLINE size_t _carray2_ydim(const size_t* yxdim) {
    return yxdim[0] / (yxdim[-1] & _carray_SUB);
}
STC_INLINE size_t _carray3_size(const size_t* zdim) {
    return zdim[0] * zdim[-1];
}

#define declare_carray_common(D, tag, Value, valueDestroy) \
    typedef struct { Value *item, *end; } carray##D##tag##_iter_t; \
 \
    STC_INLINE carray##D##tag##_iter_t \
    carray##D##tag##_begin(carray##D##tag* a) { \
        carray##D##tag##_iter_t it = {a->data, a->data + carray##D##_size(*a)}; return it; \
    } \
    STC_INLINE void \
    carray##D##tag##_next(carray##D##tag##_iter_t* it) { ++it->item; } \
 \
    STC_INLINE void \
    carray##D##tag##_destroy(carray##D##tag* self) { \
        if (self->_xdim & _carray_OWN) { \
            c_foreach (i, carray##D##tag, *self) \
                valueDestroy(i.item); \
            free(self->data); \
        } \
    }

#define declare_carray(...) c_MACRO_OVERLOAD(declare_carray, __VA_ARGS__)
#define declare_carray_2(tag, Value) \
    declare_carray_3(tag, Value, c_default_destroy)


#define declare_carray_3(tag, Value, valueDestroy) \
    typedef struct carray1##tag { \
        Value *data; \
        size_t _xdim; \
    } carray1##tag; \
 \
    typedef struct carray2##tag { \
        Value *data; \
        size_t _xdim, _yxdim; \
    } carray2##tag; \
 \
    typedef struct carray3##tag { \
        Value *data; \
        size_t _xdim, _yxdim, _zdim; \
    } carray3##tag; \
 \
    declare_carray_common(1, tag, Value, valueDestroy) \
    declare_carray_common(2, tag, Value, valueDestroy) \
    declare_carray_common(3, tag, Value, valueDestroy) \
 \
    STC_INLINE carray1##tag \
    carray1##tag##_make(size_t xdim, Value val) { \
        Value* m = c_new_n(Value, xdim); \
        for (size_t i=0; i<xdim; ++i) m[i] = val; \
        carray1##tag a = {m, xdim | _carray_OWN}; \
        return a; \
    } \
    STC_INLINE carray2##tag \
    carray2##tag##_make(size_t ydim, size_t xdim, Value val) { \
        const size_t n = ydim * xdim; \
        Value* m = c_new_n(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray2##tag a = {m, xdim | _carray_OWN, ydim * xdim}; \
        return a; \
    } \
    STC_INLINE carray3##tag \
    carray3##tag##_make(size_t zdim, size_t ydim, size_t xdim, Value val) { \
        const size_t n = zdim * ydim * xdim; \
        Value* m = c_new_n(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray3##tag a = {m, xdim | _carray_OWN, ydim * xdim, zdim}; \
        return a; \
    } \
 \
    STC_INLINE carray1##tag \
    carray1##tag##_from(size_t xdim, Value* array, bool own) { \
        carray1##tag a = {array, xdim | (own ? _carray_OWN : 0)}; \
        return a; \
    } \
    STC_INLINE carray2##tag \
    carray2##tag##_from(size_t ydim, size_t xdim, Value* array, bool own) { \
        carray2##tag a = {array, xdim | (own ? _carray_OWN : 0), ydim * xdim}; \
        return a; \
    } \
    STC_INLINE carray3##tag \
    carray3##tag##_from(size_t zdim, size_t ydim, size_t xdim, Value* array, bool own) { \
        carray3##tag a = {array, xdim | (own ? _carray_OWN : 0), ydim * xdim, zdim}; \
        return a; \
    } \
 \
    STC_INLINE Value* \
    carray1##tag##_at(carray1##tag *a, size_t x) { return a->data + x; } \
 \
    STC_INLINE carray1##tag \
    carray2##tag##_at1(carray2##tag *a, size_t y) { \
        carray1##tag sub = {a->data + y*carray2_xdim(*a), carray2_xdim(*a)}; \
        return sub; \
    } \
    STC_INLINE Value* \
    carray2##tag##_at(carray2##tag *a, size_t y, size_t x) { \
        return a->data + y*carray2_xdim(*a) + x; \
    } \
 \
    STC_INLINE carray2##tag \
    carray3##tag##_at1(carray3##tag *a, size_t z) { \
        carray2##tag sub = {a->data + z*a->_yxdim, carray3_xdim(*a), a->_yxdim}; \
        return sub; \
    } \
    STC_INLINE carray1##tag \
    carray3##tag##_at2(carray3##tag *a, size_t z, size_t y) { \
        carray1##tag sub = {a->data + z*a->_yxdim + y*carray3_xdim(*a), carray3_xdim(*a)}; \
        return sub; \
    } \
    STC_INLINE Value* \
    carray3##tag##_at(carray3##tag *a, size_t z, size_t y, size_t x) { \
        return a->data + z*a->_yxdim + y*carray3_xdim(*a) + x; \
    } \
    typedef Value carray1##tag##_value_t; \
    typedef carray1##tag##_value_t carray2##tag##_value_t, carray3##tag##_value_t
   
#endif
