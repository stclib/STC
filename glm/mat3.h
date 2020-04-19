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

#ifndef GLM_MAT3__H__
#define GLM_MAT3__H__

#include "vec3.h"

#define glm_mat3_identity_init {1, 0, 0,  0, 1, 0,  0, 0, 1}
#define glm_mat3_zero_init     {0, 0, 0,  0, 0, 0,  0, 0, 0}
                              
#define glm_mat3f_identity     ((glm_mat3f) glm_mat3_identity_init)
#define glm_mat3d_identity     ((glm_mat3d) glm_mat3_identity_init)
#define glm_mat3f_zero         ((glm_mat3f) glm_mat3_zero_init)
#define glm_mat3d_zero         ((glm_mat3d) glm_mat3_zero_init)

#define declare_glm_mat3(tag, T) \
    typedef const T (*glm_mat3##tag##m)[3]; \
 \
    typedef union glm_mat3##tag { \
        glm_vec3##tag v[3]; \
        T m[3][3], arr[3*3]; \
    } glm_mat3##tag; \
 \
    static inline glm_mat3##tag \
    glm_mat3##tag##_mult(glm_mat3##tag##m m1, glm_mat3##tag##m m2) { \
        const T *a = m1[0], *b = m2[0]; \
        return (glm_mat3##tag) { \
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
    static inline glm_mat3##tag* \
    glm_mat3##tag##_scale(glm_mat3##tag* self, T s) { \
        T* a = self->arr; \
        a[0] *= s, a[1] *= s, a[2] *= s, \
        a[3] *= s, a[4] *= s, a[5] *= s, \
        a[6] *= s, a[7] *= s, a[8] *= s; \
        return self; \
    } \
    static inline glm_mat3##tag \
    glm_mat3##tag##_scalarMult(glm_mat3##tag##m m, T s) { \
        glm_mat3##tag dst; \
        T *c = dst.arr; const T *a = m[0]; \
        for (int i = 0; i < 9; ++i) *c++ = *a++ * s; \
        return dst; \
    } \
    static inline glm_mat3##tag \
    glm_mat3##tag##_compMult(glm_mat3##tag##m m1, glm_mat3##tag##m m2) { \
        glm_mat3##tag dst; \
        T *c = dst.arr; const T *a = m1[0], *b = m2[0]; \
        for (int i = 0; i < 9; ++i) *c++ = *a++ * *b++; \
        return dst; \
    } \
    static inline glm_vec3##tag \
    glm_mat3##tag##_vecMult(glm_mat3##tag##m m, glm_vec3##tag v) { \
        return (glm_vec3##tag) { \
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z, \
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z, \
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z, \
        }; \
    } \
 \
    static inline glm_mat3##tag \
    glm_mat3##tag##_transpose(glm_mat3##tag##m m) { \
        return (glm_mat3##tag) { \
            m[0][0], m[1][0], m[2][0], \
            m[0][1], m[1][1], m[2][1], \
            m[0][2], m[1][2], m[2][2], \
        }; \
    } \
 \
    static inline T \
    glm_mat3##tag##_trace(glm_mat3##tag##m m) { \
        return m[0][0] + m[1][1] + m[2][2]; \
    } \
 \
 \
    static inline T \
    glm_mat3##tag##_determinant(glm_mat3##tag##m m) { \
        const T *a = m[0]; \
        return a[0] * (a[4] * a[8] - a[7] * a[5]) \
             - a[3] * (a[1] * a[8] - a[2] * a[7]) \
             + a[6] * (a[1] * a[5] - a[2] * a[4]); \
    } \
 \
    static inline glm_mat3##tag \
    glm_mat3##tag##_inverse(glm_mat3##tag##m m) { \
        const T *a = m[0]; \
        T k = 1.0f / glm_mat3##tag##_determinant(m); \
        return (glm_mat3##tag) { \
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
    typedef T glm_mat3##tag##_value_t

declare_glm_mat3(d, double);
declare_glm_mat3(f, float);


#endif
