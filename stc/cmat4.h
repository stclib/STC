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

#ifndef CMAT4__H__
#define CMAT4__H__

#include "cmat3.h"
#include "cvec4.h"

#define cmat4_identity_init {1, 0, 0, 0,  0, 1, 0, 0, \
                             0, 0, 1, 0,  0, 0, 0, 1}
#define cmat4_zero_init     {0, 0, 0, 0,  0, 0, 0, 0, \
                             0, 0, 0, 0,  0, 0, 0, 0}
                              
#define cmat4f_identity     ((CMat4f) cmat4_identity_init)
#define cmat4d_identity     ((CMat4d) cmat4_identity_init)
#define cmat4f_zero         ((CMat4f) cmat4_zero_init)
#define cmat4d_zero         ((CMat4d) cmat4_zero_init)

#define declare_CMat4(tag, T) \
    typedef const T (*CMat4##tag##ConstRef)[4]; \
 \
    typedef union CMat4##tag { \
        CVec4##tag v[4]; \
        T m[4][4], arr[4*4]; \
    } CMat4##tag; \
 \
    static inline CMat4##tag \
    cmat4##tag##_mult(CMat4##tag##ConstRef mat1, CMat4##tag##ConstRef mat2) { \
        const T *a = mat1[0], *b = mat2[0]; \
        return (CMat4##tag) { \
            a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3], \
            a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3], \
            a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3], \
            a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3], \
            a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7], \
            a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7], \
            a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7], \
            a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7], \
            a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11], \
            a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11], \
            a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11], \
            a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11], \
            a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15], \
            a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15], \
            a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15], \
            a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15], \
        }; \
    } \
 \
    static inline CMat4##tag* \
    cmat4##tag##_scale(CMat4##tag* self, T s) { \
        T* a = self->arr; \
        for (int i = 0; i < 16; ++i) *a++ *= s; \
        return self; \
    } \
    static inline CMat4##tag \
    cmat4##tag##_scalarMult(CMat4##tag##ConstRef m, T s) { \
        CMat4##tag dst; \
        T *c = dst.arr; const T *a = m[0]; \
        for (int i = 0; i < 16; ++i) *c++ = *a++ * s; \
        return dst; \
    } \
    static inline CMat4##tag \
    cmat4##tag##_compMult(CMat4##tag##ConstRef m1, CMat4##tag##ConstRef m2) { \
        CMat4##tag dst; \
        T *c = dst.arr; const T *a = m1[0], *b = m2[0]; \
        for (int i = 0; i < 16; ++i) *c++ = *a++ * *b++; \
        return dst; \
    } \
    static inline CVec4##tag \
    cmat4##tag##_vecMult(CMat4##tag##ConstRef m, CVec4##tag v) { \
        return (CVec4##tag) { \
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w, \
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w, \
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w, \
            m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w, \
        }; \
    } \
 \
    static inline CMat4##tag \
    cmat4##tag##_transpose(CMat4##tag##ConstRef m) { \
        return (CMat4##tag) { \
            m[0][0], m[1][0], m[2][0], m[3][0], \
            m[0][1], m[1][1], m[2][1], m[3][1], \
            m[0][2], m[1][2], m[2][2], m[3][2], \
            m[0][3], m[1][3], m[2][3], m[3][3], \
        }; \
    } \
 \
    static inline CMat3##tag \
    cmat4##tag##_transpose3(CMat4##tag##ConstRef m) { \
        return (CMat3##tag) { \
            m[0][0], m[1][0], m[2][0], \
            m[0][1], m[1][1], m[2][1], \
            m[0][2], m[1][2], m[2][2], \
        }; \
    } \
 \
    static inline CMat3##tag \
    cmat4##tag##_to3(CMat4##tag##ConstRef m) { \
        return (CMat3##tag) { \
            m[0][0], m[0][1], m[0][2], \
            m[1][0], m[1][1], m[1][2], \
            m[2][0], m[2][1], m[2][2], \
        }; \
    } \
 \
    static inline T \
    cmat4##tag##_trace(CMat4##tag##ConstRef m) { \
        return m[0][0] + m[1][1] + m[2][2] + m[3][3]; \
    } \
 \
    static inline T \
    cmat4##tag##_determinant(CMat4##tag##ConstRef mat) { \
        const T *p = mat[0]; \
        T a, b, c, d, e, f; \
        a = p[10] * p[15] - p[14] * p[11], \
        b = p[ 9] * p[15] - p[13] * p[11], \
        c = p[ 9] * p[14] - p[13] * p[10], \
        d = p[ 8] * p[15] - p[12] * p[11], \
        e = p[ 8] * p[14] - p[12] * p[10], \
        f = p[ 8] * p[13] - p[12] * p[ 9]; \
        return p[0] * (p[5] * a - p[6] * b + p[7] * c) \
             - p[1] * (p[4] * a - p[6] * d + p[7] * e) \
             + p[2] * (p[4] * b - p[5] * d + p[7] * f) \
             - p[3] * (p[4] * c - p[5] * e + p[6] * f); \
    } \
 \
 \
    static inline CMat4##tag \
    cmat4##tag##_inverse(CMat4##tag##ConstRef mat) { \
        CMat4##tag dst; \
        const T *p = mat[0]; \
        T a, b, c, d, e, f, det; \
        a = p[10] * p[15] - p[14] * p[11], \
        b = p[ 9] * p[15] - p[13] * p[11], \
        c = p[ 9] * p[14] - p[13] * p[10], \
        d = p[ 8] * p[15] - p[12] * p[11], \
        e = p[ 8] * p[14] - p[12] * p[10], \
        f = p[ 8] * p[13] - p[12] * p[ 9]; \
        dst.m[0][0] =  (p[5] * a - p[6] * b + p[7] * c); \
        dst.m[1][0] = -(p[4] * a - p[6] * d + p[7] * e); \
        dst.m[2][0] =  (p[4] * b - p[5] * d + p[7] * f); \
        dst.m[3][0] = -(p[4] * c - p[5] * e + p[6] * f); \
 \
        dst.m[0][1] = -(p[1] * a - p[2] * b + p[3] * c); \
        dst.m[1][1] =  (p[0] * a - p[2] * d + p[3] * e); \
        dst.m[2][1] = -(p[0] * b - p[1] * d + p[3] * f); \
        dst.m[3][1] =  (p[0] * c - p[1] * e + p[2] * f); \
 \
        a = p[6] * p[15] - p[14] * p[7], \
        b = p[5] * p[15] - p[13] * p[7], \
        c = p[5] * p[14] - p[13] * p[6], \
        d = p[4] * p[15] - p[12] * p[7], \
        e = p[4] * p[14] - p[12] * p[6], \
        f = p[4] * p[13] - p[12] * p[5]; \
        dst.m[0][2] =  (p[1] * a - p[2] * b + p[3] * c); \
        dst.m[1][2] = -(p[0] * a - p[2] * d + p[3] * e); \
        dst.m[2][2] =  (p[0] * b - p[1] * d + p[3] * f); \
        dst.m[3][2] = -(p[0] * c - p[1] * e + p[2] * f); \
 \
        a = p[6] * p[11] - p[10] * p[7], \
        b = p[5] * p[11] - p[ 9] * p[7], \
        c = p[5] * p[10] - p[ 9] * p[6], \
        d = p[4] * p[11] - p[ 8] * p[7], \
        e = p[4] * p[10] - p[ 8] * p[6], \
        f = p[4] * p[ 9] - p[ 8] * p[5]; \
        dst.m[0][3] = -(p[1] * a - p[2] * b + p[3] * c); \
        dst.m[1][3] =  (p[0] * a - p[2] * d + p[3] * e); \
        dst.m[2][3] = -(p[0] * b - p[1] * d + p[3] * f); \
        dst.m[3][3] =  (p[0] * c - p[1] * e + p[2] * f); \
 \
        det = p[0] * dst.m[0][0] + p[1] * dst.m[1][0] \
            + p[2] * dst.m[2][0] + p[3] * dst.m[3][0]; \
        return *cmat4##tag##_scale(&dst, 1.0f / det); \
    }  \
 \
    typedef T cmat4##tag##_value_t

declare_CMat4(d, double);
declare_CMat4(f, float);


#endif
