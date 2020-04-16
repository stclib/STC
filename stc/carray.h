// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CARRAY__H__
#define CARRAY__H__

#include <stdlib.h>
#include "cdefs.h"

/* // demo:    
#include <stc/carray.h>
declare_CArray(f, float);

int main()
{
    CArray3f a3 = carray3f_make(30, 20, 10);
    carray3f_data(a3, 5, 4)[3] = 10.2f;
    CArray2f a2 = carray3f_at(a3, 5);

    printf("%f\n", carray2f_value(a2, 4, 3));
    printf("%f\n", carray2f_data(a2, 4)[3]);
    printf("%f\n", carray2f_at(a2, 4).data[3]);
    
    printf("%f\n", carray3f_value(a3, 5, 4, 3));
    printf("%f\n", carray3f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3f_at2(a3, 5, 4).data[3]);
    
    carray_destroy(a3);
    carray_destroy(a2); // not needed, but no harm.
}
*/

#define carray_xdim(a) ((a).xdim)
#define carray_ydim(a) ({const uint32_t* d__ = &(a)._yxdim; d__[0] / d__[-1];})
#define carray_zdim(a) ((a)._zdim)
#define carray1_size(a) ((a).xdim)
#define carray2_size(a) ((a)._yxdim)
#define carray3_size(a) ({const uint32_t* d__ = &(a)._zdim; d__[0] * d__[-1];})
#define carray_destroy(a) free((a)._array)

#define declare_CArray(tag, T) \
    c_struct (CArray1##tag) { \
        T *data, *_array; \
        uint32_t xdim; \
    }; \
    c_struct (CArray2##tag) { \
        T *data, *_array; \
        uint32_t xdim, _yxdim; \
    }; \
    c_struct (CArray3##tag) { \
        T *data, *_array; \
        uint32_t xdim, _yxdim, _zdim; \
    }; \
 \
    static inline CArray1##tag \
    carray1##tag##_make(size_t xdim) { \
        T* m = c_new_2(T, xdim); \
        return (CArray1##tag) {m, m, xdim}; \
    } \
    static inline CArray2##tag \
    carray2##tag##_make(size_t ydim, size_t xdim) { \
        T* m = c_new_2(T, ydim*xdim); \
        return (CArray2##tag) {m, m, xdim, ydim*xdim}; \
    } \
    static inline CArray3##tag \
    carray3##tag##_make(size_t zdim, size_t ydim, size_t xdim) { \
        T* m = c_new_2(T, zdim*ydim*xdim); \
        return (CArray3##tag) {m, m, xdim, ydim*xdim, zdim}; \
    } \
 \
    static inline CArray1##tag \
    carray2##tag##_at(CArray2##tag a, size_t y) { \
        return (CArray1##tag) {a.data + y*a.xdim, NULL, a.xdim}; \
    } \
    static inline T* \
    carray2##tag##_data(CArray2##tag a, size_t y) { \
        return a.data + y*a.xdim; \
    } \
    static inline T \
    carray2##tag##_value(CArray2##tag a, size_t y, size_t x) { \
        return a.data[ y*a.xdim + x ]; \
    } \
 \
    static inline CArray2##tag \
    carray3##tag##_at(CArray3##tag a, size_t z) { \
        return (CArray2##tag) {a.data + z*a._yxdim, NULL, a.xdim, a._yxdim}; \
    } \
    static inline CArray1##tag \
    carray3##tag##_at2(CArray3##tag a, size_t z, size_t y) { \
        return (CArray1##tag) {a.data + z*a._yxdim + y*a.xdim, NULL, a.xdim}; \
    } \
    static inline T* \
    carray3##tag##_data(CArray3##tag a, size_t z, size_t y) { \
        return a.data + z*a._yxdim + y*a.xdim; \
    } \
    static inline T \
    carray3##tag##_value(CArray3##tag a, size_t z, size_t y, size_t x) { \
        return a.data[ z*a._yxdim + y*a.xdim + x ]; \
    } \
    typedef T carray_##tag##_t
    
#endif