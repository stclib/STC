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
#ifndef CARRAY__H__
#define CARRAY__H__

#include <stdlib.h>
#include "ccommon.h"

/*
 Multi-dimensional generic array allocated as one block of heap-memory.
 // demo:
#include <stdio.h>
#include "stc/carray.h"
using_carray(f, float);

int main()
{
    carray3f a3 = carray3f_init(30, 20, 10, 0.0f);
    *carray3f_at(&a3, 5, 4, 3) = 10.2f;         // a3[5][4][3]
    carray2f a2 = carray3f_at1(&a3, 5);         // sub-array reference: a2 = a3[5]
    printf("%g\n", *carray2f_at(&a2, 4, 3));    // lookup a2[4][3] (=10.2f)
    printf("%g\n", *carray3f_at(&a3, 5, 4, 3)); // same data location, via a3 array.

    carray2f_del(&a2); // does nothing, since it is a sub-array.
    carray3f_del(&a3); // destroy a3, invalidates a2.
}
*/

#define using_carray(...) c_MACRO_OVERLOAD(using_carray, __VA_ARGS__)
#define using_carray_2(X, Value) \
    using_carray_4(X, Value, c_trivial_del, c_trivial_fromraw)

#define using_carray_4(X, Value, valueDel, valueClone) \
\
    typedef Value carray1##X##_value_t; \
    typedef carray1##X##_value_t carray2##X##_value_t, carray3##X##_value_t; \
\
    typedef struct { \
        Value *data; \
        size_t _xdim; \
    } carray1##X; \
\
    typedef struct { \
        Value *data; \
        size_t _xdim, _ydim; \
    } carray2##X, carray2##X##_t; \
\
    typedef struct { \
        Value *data; \
        size_t _xdim, _ydim, _zdim; \
    } carray3##X, carray3##X##_t; \
\
    STC_INLINE size_t \
    carray1##X##_size(carray1##X a) {return _carray_xdim(a);} \
    STC_INLINE size_t \
    carray2##X##_size(carray2##X a) {return _carray_xdim(a)*_carray_ydim(a);} \
    STC_INLINE size_t \
    carray3##X##_size(carray3##X a) {return _carray_xdim(a)*_carray_ydim(a)*_carray_zdim(a);} \
    STC_INLINE size_t \
    carray2##X##_ydim(carray2##X a) {return _carray_ydim(a);} \
    STC_INLINE size_t \
    carray3##X##_ydim(carray3##X a) {return _carray_ydim(a);} \
    STC_INLINE size_t \
    carray3##X##_zdim(carray3##X a) {return _carray_zdim(a);} \
\
    _using_carray_common(1, X, Value, valueDel, valueClone) \
    _using_carray_common(2, X, Value, valueDel, valueClone) \
    _using_carray_common(3, X, Value, valueDel, valueClone) \
\
    STC_INLINE carray1##X \
    carray1##X##_init(size_t xdim, Value val) { \
        Value* m = c_new_2(Value, xdim); \
        for (size_t i=0; i<xdim; ++i) m[i] = val; \
        carray1##X a = {m, xdim | _carray_OWN}; \
        return a; \
    } \
    STC_INLINE carray2##X \
    carray2##X##_init(size_t ydim, size_t xdim, Value val) { \
        const size_t n = ydim * xdim; \
        Value* m = c_new_2(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray2##X a = {m, xdim | _carray_OWN, ydim}; \
        return a; \
    } \
    STC_INLINE carray3##X \
    carray3##X##_init(size_t zdim, size_t ydim, size_t xdim, Value val) { \
        const size_t n = zdim * ydim * xdim; \
        Value* m = c_new_2(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray3##X a = {m, xdim | _carray_OWN, ydim, zdim}; \
        return a; \
    } \
\
    STC_INLINE carray1##X \
    carray1##X##_from(Value* array, size_t xdim) { \
        carray1##X a = {array, xdim}; \
        return a; \
    } \
    STC_INLINE carray2##X \
    carray2##X##_from(Value* array, size_t ydim, size_t xdim) { \
        carray2##X a = {array, xdim, ydim}; \
        return a; \
    } \
    STC_INLINE carray3##X \
    carray3##X##_from(Value* array, size_t zdim, size_t ydim, size_t xdim) { \
        carray3##X a = {array, xdim, ydim, zdim}; \
        return a; \
    } \
\
    STC_INLINE Value* \
    carray1##X##_at(carray1##X *a, size_t x) { return a->data + x; } \
    \
    STC_INLINE carray1##X \
    carray2##X##_at1(carray2##X *a, size_t y) { \
        carray1##X sub = {a->data + y*_carray_xdim(*a), _carray_xdim(*a)}; \
        return sub; \
    } \
    STC_INLINE Value* \
    carray2##X##_at(carray2##X *a, size_t y, size_t x) { \
        return a->data + y*_carray_xdim(*a) + x; \
    } \
\
    STC_INLINE carray2##X \
    carray3##X##_at1(carray3##X *a, size_t z) { \
        carray2##X sub = {a->data + z*_carray_ydim(*a)*_carray_xdim(*a), _carray_xdim(*a), _carray_ydim(*a)}; \
        return sub; \
    } \
    STC_INLINE carray1##X \
    carray3##X##_at2(carray3##X *a, size_t z, size_t y) { \
        carray1##X sub = {a->data + (z*_carray_ydim(*a) + y)*_carray_xdim(*a), _carray_xdim(*a)}; \
        return sub; \
    } \
    STC_INLINE Value* \
    carray3##X##_at(carray3##X *a, size_t z, size_t y, size_t x) { \
        return a->data + (z*_carray_ydim(*a) + y)*_carray_xdim(*a) + x; \
    } \
    typedef carray1##X carray1##X##_t


#define _carray_SUB (SIZE_MAX >> 1)
#define _carray_OWN (_carray_SUB + 1)
#define _carray_xdim(a) ((a)._xdim & _carray_SUB)
#define _carray_ydim(a) (a)._ydim
#define _carray_zdim(a) (a)._zdim

#define _using_carray_common(D, X, Value, valueDel, valueClone) \
    typedef struct { Value *ref; } carray##D##X##_iter_t; \
\
    STC_INLINE carray##D##X##_iter_t \
    carray##D##X##_begin(carray##D##X* a) { \
        carray##D##X##_iter_t it = {a->data}; return it; \
    } \
    STC_INLINE carray##D##X##_iter_t \
    carray##D##X##_end(carray##D##X* a) { \
        carray##D##X##_iter_t it = {a->data + carray##D##X##_size(*a)}; return it; \
    } \
    STC_INLINE void \
    carray##D##X##_next(carray##D##X##_iter_t* it) {++it->ref;} \
\
    STC_INLINE void \
    carray##D##X##_del(carray##D##X* self) { \
        if (self->_xdim & _carray_OWN) { \
            c_foreach_3 (i, carray##D##X, *self) \
                valueDel(i.ref); \
            c_free(self->data); \
        } \
    } \
    STC_INLINE carray##D##X \
    carray##D##X##_clone(carray##D##X arr) { \
        carray##D##X c = arr; size_t k = 0; \
        c.data = c_new_2(Value, carray##D##X##_size(arr)); \
        c_foreach_3 (i, carray##D##X, arr) \
            c.data[k++] = valueClone(*i.ref); \
        return c; \
    } \
    STC_INLINE size_t \
    carray##D##X##_xdim(carray##D##X a) {return _carray_xdim(a);} \

#endif
