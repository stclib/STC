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

#ifndef CMAT3__H__
#define CMAT3__H__

#include "cvec3.h"

#define cmat3_identity_init {1, 0, 0,  0, 1, 0,  0, 0, 1}
#define cmat3_zero_init     {0, 0, 0,  0, 0, 0,  0, 0, 0}
                              
#define cmat3f_identity     ((CMat3f) cmat3_identity_init)
#define cmat3d_identity     ((CMat3d) cmat3_identity_init)
#define cmat3f_zero         ((CMat3f) cmat3_zero_init)
#define cmat3d_zero         ((CMat3d) cmat3_zero_init)

#define declare_CMat3(tag, T) \
    typedef T (*CMat3##tag##Raw)[3]; \
    typedef const T (*CMat3##tag##ConstRaw)[3]; \
 \
    typedef union CMat3##tag { \
        CVec3##tag v[3]; \
        T m[3][3], arr[3*3]; \
    } CMat3##tag; \
 \
    static inline CMat3##tag \
    cmat3##tag##_mult(CMat3##tag##ConstRaw m1, CMat3##tag##ConstRaw m2) { \
        const T *a = m1[0], *b = m2[0]; \
        return (CMat3##tag) { \
            a[0] * b[0] + a[3] * b[1] + a[6] * b[2], \
            a[1] * b[0] + a[4] * b[1] + a[7] * b[2], \
            a[2] * b[0] + a[5] * b[1] + a[8] * b[2], \
            a[0] * b[3] + a[3] * b[4] + a[6] * b[5], \
            a[1] * b[3] + a[4] * b[4] + a[7] * b[5], \
            a[2] * b[3] + a[5] * b[4] + a[8] * b[5], \
            a[0] * b[6] + a[3] * b[7] + a[6] * b[8], \
            a[1] * b[6] + a[4] * b[7] + a[7] * b[8], \
            a[2] * b[6] + a[5] * b[7] + a[8] * b[8], \
        }; \
    } \
 \
    static inline CMat3##tag* \
    cmat3##tag##_scale(CMat3##tag* self, T s) { \
        T* a = self->arr; \
        a[0] *= s, a[1] *= s, a[2] *= s, \
        a[3] *= s, a[4] *= s, a[5] *= s, \
        a[6] *= s, a[7] *= s, a[8] *= s; \
        return self; \
    } \
    static inline CMat3##tag \
    cmat3##tag##_scalarMult(CMat3##tag##ConstRaw m, T s) { \
        CMat3##tag dst; \
        T *c = dst.arr; const T *a = m[0]; \
        for (int i = 0; i < 9; ++i) *c++ = *a++ * s; \
        return dst; \
    } \
    static inline CMat3##tag \
    cmat3##tag##_compMult(CMat3##tag##ConstRaw m1, CMat3##tag##ConstRaw m2) { \
        CMat3##tag dst; \
        T *c = dst.arr; const T *a = m1[0], *b = m2[0]; \
        for (int i = 0; i < 9; ++i) *c++ = *a++ * *b++; \
        return dst; \
    } \
    static inline CVec3##tag \
    cmat3##tag##_vecMult(CMat3##tag##ConstRaw m, CVec3##tag v) { \
        return (CVec3##tag) { \
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z, \
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z, \
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z, \
        }; \
    } \
 \
    static inline CMat3##tag \
    cmat3##tag##_transpose(CMat3##tag##ConstRaw m) { \
        return (CMat3##tag) { \
            m[0][0], m[1][0], m[2][0], \
            m[0][1], m[1][1], m[2][1], \
            m[0][2], m[1][2], m[2][2], \
        }; \
    } \
 \
    static inline T \
    cmat3##tag##_trace(CMat3##tag##ConstRaw m) { \
        return m[0][0] + m[1][1] + m[2][2]; \
    } \
 \
 \
    static inline T \
    cmat3##tag##_determinant(CMat3##tag##ConstRaw m) { \
        const T *a = m[0]; \
        return a[0] * (a[4] * a[8] - a[7] * a[5]) \
             - a[3] * (a[1] * a[8] - a[2] * a[7]) \
             + a[6] * (a[1] * a[5] - a[2] * a[4]); \
    } \
 \
    static inline CMat3##tag \
    cmat3##tag##_inverse(CMat3##tag##ConstRaw m) { \
        const T *a = m[0]; \
        T k = 1.0f / cmat3##tag##_determinant(m); \
        return (CMat3##tag) { \
             (a[4] * a[8] - a[5] * a[7]) * k, \
            -(a[1] * a[8] - a[7] * a[2]) * k, \
             (a[1] * a[5] - a[4] * a[2]) * k, \
            -(a[3] * a[8] - a[6] * a[5]) * k, \
             (a[0] * a[8] - a[2] * a[6]) * k, \
            -(a[0] * a[5] - a[3] * a[2]) * k, \
             (a[3] * a[7] - a[6] * a[4]) * k, \
            -(a[0] * a[7] - a[6] * a[1]) * k, \
             (a[0] * a[4] - a[1] * a[3]) * k, \
        }; \
    }  \
 \
    typedef T cmat3##tag##_value_t

declare_CMat3(d, double);
declare_CMat3(f, float);


#endif
