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
#ifndef CARR__H__
#define CARR__H__

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
    carray3f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    carray2f a2 = carray3f_at(a3, 5);   // sub-array reference (no data copy).

    printf("%f\n", carray2f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3f_at2(a3, 5, 4).data[3]);
    
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

static inline size_t _carray2_ydim(const size_t* yxdim) {
    return yxdim[0] / (yxdim[-1] & _carray_SUB);
}
static inline size_t _carray3_size(const size_t* zdim) {
    return zdim[0] * zdim[-1];
}


#define declare_carray(...) c_MACRO_OVERLOAD(declare_carray, __VA_ARGS__)

#define declare_carray_2(tag, Value) \
    declare_carray_3(tag, Value, c_defaultDestroy)


#define declare_carray_3(tag, Value, valueDestroy) \
    typedef struct { \
        Value *data; \
        size_t _xdim; \
    } carray1##tag; \
 \
    typedef struct { \
        Value *data; \
        size_t _xdim, _yxdim; \
    } carray2##tag; \
 \
    typedef struct { \
        Value *data; \
        size_t _xdim, _yxdim, _zdim; \
    } carray3##tag; \
 \
    static inline carray1##tag \
    carray1##tag##_make(size_t xdim, Value val) { \
        Value* m = c_new_N(Value, xdim); \
        for (size_t i=0; i<xdim; ++i) m[i] = val; \
        carray1##tag a = {m, xdim | _carray_OWN}; \
        return a; \
    } \
    static inline carray2##tag \
    carray2##tag##_make(size_t ydim, size_t xdim, Value val) { \
        const size_t n = ydim * xdim; \
        Value* m = c_new_N(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray2##tag a = {m, xdim | _carray_OWN, ydim * xdim}; \
        return a; \
    } \
    static inline carray3##tag \
    carray3##tag##_make(size_t zdim, size_t ydim, size_t xdim, Value val) { \
        const size_t n = zdim * ydim * xdim; \
        Value* m = c_new_N(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        carray3##tag a = {m, xdim | _carray_OWN, ydim * xdim, zdim}; \
        return a; \
    } \
 \
    static inline carray1##tag \
    carray1##tag##_from(size_t xdim, Value* array, bool own) { \
        carray1##tag a = {array, xdim | (own ? _carray_OWN : 0)}; \
        return a; \
    } \
    static inline carray2##tag \
    carray2##tag##_from(size_t ydim, size_t xdim, Value* array, bool own) { \
        carray2##tag a = {array, xdim | (own ? _carray_OWN : 0), ydim * xdim}; \
        return a; \
    } \
    static inline carray3##tag \
    carray3##tag##_from(size_t zdim, size_t ydim, size_t xdim, Value* array, bool own) { \
        carray3##tag a = {array, xdim | (own ? _carray_OWN : 0), ydim * xdim, zdim}; \
        return a; \
    } \
 \
    static inline void \
    carray1##tag##_destroy(carray1##tag* self) { \
        if (self->_xdim & _carray_OWN) { \
            size_t n = carray1_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
    static inline void \
    carray2##tag##_destroy(carray2##tag* self) { \
        if (self->_xdim & _carray_OWN) { \
            size_t n = carray2_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
    static inline void \
    carray3##tag##_destroy(carray3##tag* self) { \
        if (self->_xdim & _carray_OWN) { \
            size_t n = carray3_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
 \
    static inline carray1##tag \
    carray2##tag##_at(carray2##tag a, size_t y) { \
        carray1##tag sub = {a.data + y*carray2_xdim(a), carray2_xdim(a)}; \
        return sub; \
    } \
    static inline Value* \
    carray2##tag##_data(carray2##tag a, size_t y) { \
        return a.data + y*carray2_xdim(a); \
    } \
    static inline Value \
    carray2##tag##_value(carray2##tag a, size_t y, size_t x) { \
        return a.data[ y*carray2_xdim(a) + x ]; \
    } \
 \
    static inline carray2##tag \
    carray3##tag##_at(carray3##tag a, size_t z) { \
        carray2##tag sub = {a.data + z*a._yxdim, carray3_xdim(a), a._yxdim}; \
        return sub; \
    } \
    static inline carray1##tag \
    carray3##tag##_at2(carray3##tag a, size_t z, size_t y) { \
        carray1##tag sub = {a.data + z*a._yxdim + y*carray3_xdim(a), carray3_xdim(a)}; \
        return sub; \
    } \
    static inline Value* \
    carray3##tag##_data(carray3##tag a, size_t z, size_t y) { \
        return a.data + z*a._yxdim + y*carray3_xdim(a); \
    } \
    static inline Value \
    carray3##tag##_value(carray3##tag a, size_t z, size_t y, size_t x) { \
        return a.data[ z*a._yxdim + y*carray3_xdim(a) + x ]; \
    } \
    typedef Value carrayValue_##tag
   
#endif
