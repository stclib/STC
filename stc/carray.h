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

/* // demo:    
#include <stc/carray.h>
declare_CArray(f, float);

int main()
{
    CArray3_f a3 = carray3f_make(30, 20, 10, 0.f);
    carray3_f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    CArray2_f a2 = carray3_f_at(a3, 5);   // sub-array reference (no data copy).

    printf("%f\n", carray2_f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2_f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2_f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3_f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3_f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3_f_at2(a3, 5, 4).data[3]);
    
    carray_f_destroy(&a2); // does nothing, since it is a sub-array.
    carray_f_destroy(&a3); // also invalidates a2.
}
*/

#define carray_xdim(a)  ((a).xdim)
#define carray_ydim(a)  ((a)._yxdim / (a).xdim)
#define carray_zdim(a)  ((a)._zdim)
#define carray1_size(a) ((a).xdim)
#define carray2_size(a) ((a)._yxdim)
#define carray3_size(a) ((a)._zdim * (a)._yxdim)

#define declare_CArray(...) c_MACRO_OVERLOAD(declare_CArray, __VA_ARGS__)

#define declare_CArray_2(tag, Value) \
    declare_CArray_3(tag, Value, c_emptyDestroy)


#define declare_CArray_3(tag, Value, valueDestroy) \
    typedef struct { \
        Value *data; uint32_t owned, xdim; \
    } CArray1_##tag; \
 \
    typedef struct { \
        Value *data; uint32_t owned, xdim; \
        size_t _yxdim; \
    } CArray2_##tag; \
 \
    typedef struct { \
        Value *data; uint32_t owned, xdim; \
        size_t _yxdim; uint32_t _zdim; \
    } CArray3_##tag; \
 \
    static inline void \
    carray1_##tag##_destroy(CArray1_##tag* self) { \
        const size_t n = carray1_size(*self); Value* a = self->data; \
        if (self->owned) {for (size_t i=0; i<n; ++i) valueDestroy(&a[i]); free(a);} \
    } \
    static inline void \
    carray2_##tag##_destroy(CArray2_##tag* self) { \
        const size_t n = carray2_size(*self); Value* a = self->data; \
        if (self->owned) {for (size_t i=0; i<n; ++i) valueDestroy(&a[i]); free(a);} \
    } \
    static inline void \
    carray3_##tag##_destroy(CArray3_##tag* self) { \
        const size_t n = carray3_size(*self); Value* a = self->data; \
        if (self->owned) {for (size_t i=0; i<n; ++i) valueDestroy(&a[i]); free(a);} \
    } \
 \
    static inline CArray1_##tag \
    carray1_##tag##_make(size_t xdim, Value val) { \
        Value* m = c_new_2(Value, xdim); \
        for (size_t i=0; i<xdim; ++i) m[i] = val; \
        CArray1_##tag a = {m, true, xdim}; \
        return a; \
    } \
    static inline CArray2_##tag \
    carray2_##tag##_make(size_t ydim, size_t xdim, Value val) { \
        size_t n = ydim * xdim; \
        Value* m = c_new_2(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        CArray2_##tag a = {m, true, xdim, ydim * xdim}; \
        return a; \
    } \
    static inline CArray3_##tag \
    carray3_##tag##_make(size_t zdim, size_t ydim, size_t xdim, Value val) { \
        size_t n = zdim * ydim * xdim; \
        Value* m = c_new_2(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        CArray3_##tag a = {m, true, xdim, ydim * xdim, zdim}; \
        return a; \
    } \
 \
    static inline CArray1_##tag \
    carray2_##tag##_at(CArray2_##tag a, size_t y) { \
        CArray1_##tag sub = {a.data + y*a.xdim, false, a.xdim}; \
        return sub; \
    } \
    static inline Value* \
    carray2_##tag##_data(CArray2_##tag a, size_t y) { \
        return a.data + y*a.xdim; \
    } \
    static inline Value \
    carray2_##tag##_value(CArray2_##tag a, size_t y, size_t x) { \
        return a.data[ y*a.xdim + x ]; \
    } \
 \
    static inline CArray2_##tag \
    carray3_##tag##_at(CArray3_##tag a, size_t z) { \
        CArray2_##tag sub = {a.data + z*a._yxdim, false, a.xdim, a._yxdim}; \
        return sub; \
    } \
    static inline CArray1_##tag \
    carray3_##tag##_at2(CArray3_##tag a, size_t z, size_t y) { \
        CArray1_##tag sub = {a.data + z*a._yxdim + y*a.xdim, false, a.xdim}; \
        return sub; \
    } \
    static inline Value* \
    carray3_##tag##_data(CArray3_##tag a, size_t z, size_t y) { \
        return a.data + z*a._yxdim + y*a.xdim; \
    } \
    static inline Value \
    carray3_##tag##_value(CArray3_##tag a, size_t z, size_t y, size_t x) { \
        return a.data[ z*a._yxdim + y*a.xdim + x ]; \
    } \
    typedef Value CArrayValue_##tag
   
#endif
