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
#include <stc/carr.h>
declare_CArr(f, float);

int main()
{
    CArr3_f a3 = carr3_f_make(30, 20, 10, 0.f);
    carr3_f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    CArr2_f a2 = carr3_f_at(a3, 5);   // sub-array reference (no data copy).

    printf("%f\n", carr2_f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carr2_f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carr2_f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carr3_f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carr3_f_data(a3, 5, 4)[3]);
    printf("%f\n", carr3_f_at2(a3, 5, 4).data[3]);
    
    carr2_f_destroy(&a2); // does nothing, since it is a sub-array.
    carr3_f_destroy(&a3); // also invalidates a2.
}
*/

#define carr1_xdim(a)  ((a)._xdim & _carr_SUB)
#define carr1_size(a) carr1_xdim(a)

#define carr2_xdim(a)  carr1_xdim(a)
#define carr2_ydim(a)  _carr2_ydim(&(a)._yxdim)
#define carr2_size(a) ((a)._yxdim)

#define carr3_xdim(a)  carr1_xdim(a)
#define carr3_ydim(a)  carr2_ydim(a)
#define carr3_zdim(a)  ((a)._zdim)
#define carr3_size(a) _carr3_size(&(a)._zdim)

#define _carr_SUB (SIZE_MAX >> 1)
#define _carr_OWN (_carr_SUB + 1)

static inline size_t _carr2_ydim(const size_t* yxdim) {
    return yxdim[0] / (yxdim[-1] & _carr_SUB);
}
static inline size_t _carr3_size(const size_t* zdim) {
    return zdim[0] * zdim[-1];
}


#define declare_CArr(...) c_MACRO_OVERLOAD(declare_CArr, __VA_ARGS__)

#define declare_CArr_2(tag, Value) \
    declare_CArr_3(tag, Value, c_defaultDestroy)


#define declare_CArr_3(tag, Value, valueDestroy) \
    typedef struct { \
        Value *data; \
        size_t _xdim; \
    } CArr1_##tag; \
 \
    typedef struct { \
        Value *data; \
        size_t _xdim, _yxdim; \
    } CArr2_##tag; \
 \
    typedef struct { \
        Value *data; \
        size_t _xdim, _yxdim, _zdim; \
    } CArr3_##tag; \
 \
    static inline CArr1_##tag \
    carr1_##tag##_make(size_t xdim, Value val) { \
        Value* m = c_new_N(Value, xdim); \
        for (size_t i=0; i<xdim; ++i) m[i] = val; \
        CArr1_##tag a = {m, xdim | _carr_OWN}; \
        return a; \
    } \
    static inline CArr2_##tag \
    carr2_##tag##_make(size_t ydim, size_t xdim, Value val) { \
        const size_t n = ydim * xdim; \
        Value* m = c_new_N(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        CArr2_##tag a = {m, xdim | _carr_OWN, ydim * xdim}; \
        return a; \
    } \
    static inline CArr3_##tag \
    carr3_##tag##_make(size_t zdim, size_t ydim, size_t xdim, Value val) { \
        const size_t n = zdim * ydim * xdim; \
        Value* m = c_new_N(Value, n); \
        for (size_t i=0; i<n; ++i) m[i] = val; \
        CArr3_##tag a = {m, xdim | _carr_OWN, ydim * xdim, zdim}; \
        return a; \
    } \
 \
    static inline CArr1_##tag \
    carr1_##tag##_from(size_t xdim, Value* array, bool own) { \
        CArr1_##tag a = {array, xdim | (own ? _carr_OWN : 0)}; \
        return a; \
    } \
    static inline CArr2_##tag \
    carr2_##tag##_from(size_t ydim, size_t xdim, Value* array, bool own) { \
        CArr2_##tag a = {array, xdim | (own ? _carr_OWN : 0), ydim * xdim}; \
        return a; \
    } \
    static inline CArr3_##tag \
    carr3_##tag##_from(size_t zdim, size_t ydim, size_t xdim, Value* array, bool own) { \
        CArr3_##tag a = {array, xdim | (own ? _carr_OWN : 0), ydim * xdim, zdim}; \
        return a; \
    } \
 \
    static inline void \
    carr1_##tag##_destroy(CArr1_##tag* self) { \
        if (self->_xdim & _carr_OWN) { \
            size_t n = carr1_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
    static inline void \
    carr2_##tag##_destroy(CArr2_##tag* self) { \
        if (self->_xdim & _carr_OWN) { \
            size_t n = carr2_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
    static inline void \
    carr3_##tag##_destroy(CArr3_##tag* self) { \
        if (self->_xdim & _carr_OWN) { \
            size_t n = carr3_size(*self); Value* a = self->data; \
            while (n--) valueDestroy(&a[n]); free(a); \
        } \
    } \
 \
    static inline CArr1_##tag \
    carr2_##tag##_at(CArr2_##tag a, size_t y) { \
        CArr1_##tag sub = {a.data + y*carr2_xdim(a), carr2_xdim(a)}; \
        return sub; \
    } \
    static inline Value* \
    carr2_##tag##_data(CArr2_##tag a, size_t y) { \
        return a.data + y*carr2_xdim(a); \
    } \
    static inline Value \
    carr2_##tag##_value(CArr2_##tag a, size_t y, size_t x) { \
        return a.data[ y*carr2_xdim(a) + x ]; \
    } \
 \
    static inline CArr2_##tag \
    carr3_##tag##_at(CArr3_##tag a, size_t z) { \
        CArr2_##tag sub = {a.data + z*a._yxdim, carr3_xdim(a), a._yxdim}; \
        return sub; \
    } \
    static inline CArr1_##tag \
    carr3_##tag##_at2(CArr3_##tag a, size_t z, size_t y) { \
        CArr1_##tag sub = {a.data + z*a._yxdim + y*carr3_xdim(a), carr3_xdim(a)}; \
        return sub; \
    } \
    static inline Value* \
    carr3_##tag##_data(CArr3_##tag a, size_t z, size_t y) { \
        return a.data + z*a._yxdim + y*carr3_xdim(a); \
    } \
    static inline Value \
    carr3_##tag##_value(CArr3_##tag a, size_t z, size_t y, size_t x) { \
        return a.data[ z*a._yxdim + y*carr3_xdim(a) + x ]; \
    } \
    typedef Value CArrValue_##tag
   
#endif
