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

#ifndef CVEC4__H__
#define CVEC4__H__

#include "cvec3.h"

#ifdef c_NO_ANONYMOUS_STRUCT
#define CVEC4_DEF(tag, T) struct CVec4##tag { T x, y, z, w; }
#else
#define CVEC4_DEF(tag, T) union CVec4##tag { struct { T x, y, z, w; }; T arr[4]; }
#endif

#define cvec4_arr(v) (&(v).x)
#define cvec4f_zero ((CVec4f) {0.f, 0.f, 0.f, 0.f})
#define cvec4d_zero ((CVec4d) {0.0, 0.0, 0.0, 0.0})

#define declare_CVec4(tag, T) \
    typedef CVEC4_DEF(tag, T) CVec4##tag; \
    static CVec4##tag cvec4##tag##_axis[4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}; \
 \
    static inline CVec4##tag \
    cvec4##tag(T x, T y, T z, T w) { return (CVec4##tag) {x, y, z, w}; } \
    static inline CVec4##tag \
    cvec4##tag##_fromArray(const T* a) { return (CVec4##tag) {a[0], a[1], a[2], a[3]}; } \
 \
    static inline CVec4##tag* \
    cvec4##tag##_assign(CVec4##tag* self, T x, T y, T z, T w) { \
         self->x = x, self->y = y, self->z = z, self->z = z, self->w = w; return self; \
    } \
    static inline CVec4##tag* \
    cvec4##tag##_assignArray(CVec4##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2], self->w = a[3]; return self; \
    } \
    static inline CVec4##tag* \
    cvec4##tag##_add(CVec4##tag* self, CVec4##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z, self->w += v.w; return self; \
    } \
    static inline CVec4##tag* \
    cvec4##tag##_sub(CVec4##tag* self, CVec4##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z, self->w -= v.w; return self; \
    } \
    static inline CVec4##tag* \
    cvec4##tag##_scale(CVec4##tag* self, T s) { \
         self->x *= s, self->y *= s, self->z *= s, self->w *= s; return self; \
    } \
    static inline T \
    cvec4##tag##_length(CVec4##tag v) { \
        return c_sqrt_##tag(_cvec4_DOT(v, v)); \
    } \
    static inline T \
    cvec4##tag##_length2(CVec4##tag v) { \
        return _cvec4_DOT(v, v); \
    } \
    static inline T \
    cvec4##tag##_distance(CVec4##tag u, CVec4##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z, u.w -= v.w; \
        return c_sqrt_##tag(_cvec4_DOT(u, u)); \
    } \
    static inline CVec4##tag \
    cvec4##tag##_plus(CVec4##tag u, CVec4##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z, u.w += v.w; return u; \
    } \
    static inline CVec4##tag \
    cvec4##tag##_minus(CVec4##tag u, CVec4##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z, u.w -= v.w; return u; \
    } \
    static inline CVec4##tag \
    cvec4##tag##_mult(CVec4##tag v, T s) { \
        v.x *= s, v.y *= s, v.z *= s, v.w *= s; return v; \
    } \
    static inline CVec4##tag \
    cvec4##tag##_neg(CVec4##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z, v.w = -v.w; return v; \
    } \
    static inline CVec4##tag \
    cvec4##tag##_unit(CVec4##tag v) { \
        T s = c_sqrt_##tag(_cvec4_DOT(v, v)); \
        if (s < 1e-8) return cvec4##tag##_zero; \
        s = 1.0f / s; v.x *= s, v.y *= s, v.z *= s, v.w *= s; \
        return v; \
    } \
    static inline T \
    cvec4##tag##_dot(CVec4##tag u, CVec4##tag v) { \
        return _cvec4_DOT(u, v); \
    } \
    static inline CVec3##tag \
    cvec4##tag##_to3(CVec4##tag u) { \
        return (CVec3##tag) {u.x, u.y, u.z}; \
    } \
    static inline CVec4##tag \
    cvec3##tag##_to4(CVec3##tag u, T w) { \
        return (CVec4##tag) {u.x, u.y, u.z, w}; \
    } \
    /* Swizzle */ \
    static inline CVec4##tag \
    cvec4##tag##_swizzle(CVec4##tag u, const char* swz) { \
        const T* a = cvec4_arr(u); \
        return (CVec4##tag) {a[(swz[0] - 'x') & 3], \
                             a[(swz[1] - 'x') & 3], \
                             a[(swz[2] - 'x') & 3], \
                             a[(swz[3] - 'x') & 3]}; \
    } \
    static inline CVec3##tag \
    cvec4##tag##_swizzle3(CVec4##tag u, const char* swz) { \
        const T* a = cvec4_arr(u); \
        return (CVec3##tag) {a[(swz[0] - 'x') & 3], \
                             a[(swz[1] - 'x') & 3], \
                             a[(swz[2] - 'x') & 3]}; \
    } \
    /* Linear interpolation */ \
    static inline CVec4##tag \
    cvec4##tag##_lerp(CVec4##tag u, CVec4##tag v, T t) { \
        T s = 1.0f - t; \
        u.x = s*u.x + t*v.x, u.y = s*u.y + t*v.y, \
        u.z = s*u.z + t*v.z, u.w = s*u.w + t*v.w; \
        return u; \
    } \
    static inline bool \
    cvec4##tag##_equals(CVec4##tag u, CVec4##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        if (u.z != v.z) return false; \
        return u.w == v.w; \
    } \
    static inline int \
    cvec4##tag##_compare(CVec4##tag* u, CVec4##tag* v) { \
        if (u->x != v->x)  return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y)  return 1 - ((u->y < v->y)<<1); \
        if (u->z != v->z)  return 1 - ((u->z < v->z)<<1); \
        return u->w == v->w ? 0 : 1 - ((u->w < v->w)<<1); \
    } \
    typedef T cvec4##tag##_value_t

#define _cvec4_DOT(u, v) (u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w)
#define _cvec4_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w
    
declare_CVec4(d, double);
declare_CVec4(f, float);

static inline CVec4d
cvec4f_tod(CVec4f v) {
    return (CVec4d) {v.x, v.y, v.z, v.w};
}
static inline CVec4f
cvec4d_tof(CVec4d v) {
    return (CVec4f) {(float) v.x, (float) v.y, (float) v.z, (float) v.w};
}

#endif
