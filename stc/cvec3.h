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

#ifndef CVEC3__H__
#define CVEC3__H__

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define c_sqrt_f(x) sqrtf(x)
#define c_sqrt_d(x) sqrt(x)
#define c_sin_f(x) sinf(x)
#define c_sin_d(x) sin(x)
#define c_cos_f(x) cosf(x)
#define c_cos_d(x) cos(x)
#define c_atan2_f(x, y) atan2f(x, y)
#define c_atan2_d(x, y) atan2(x, y)

#ifdef c_NO_ANONYMOUS_STRUCT
#define CVEC3_DEF(tag, T) struct CVec3##tag { T x, y, z; }
#else
#define CVEC3_DEF(tag, T) union CVec3##tag { struct { T x, y, z; }; T arr[3]; }
#endif

#define cvec3_arr(v) (&(v).x)
#define cvec3f_zero ((CVec3f) {0.f, 0.f, 0.f})
#define cvec3d_zero ((CVec3d) {0.0, 0.0, 0.0})


#define declare_CVec3(tag, T) \
    typedef CVEC3_DEF(tag, T) CVec3##tag; \
    static CVec3##tag \
    cvec3##tag##_axis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; \
 \
    static inline CVec3##tag \
    cvec3##tag(T x, T y, T z) { return (CVec3##tag) {x, y, z}; } \
    static inline CVec3##tag \
    cvec3##tag##_fromArray(const T* a) { return (CVec3##tag) {a[0], a[1], a[2]}; } \
 \
    static inline CVec3##tag* \
    cvec3##tag##_assign(CVec3##tag* self, T x, T y, T z) { \
         self->x = x, self->y = y, self->z = z; return self; \
    } \
    static inline CVec3##tag* \
    cvec3##tag##_assignArray(CVec3##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2]; return self; \
    } \
    static inline CVec3##tag* \
    cvec3##tag##_add(CVec3##tag* self, CVec3##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z; return self; \
    } \
    static inline CVec3##tag* \
    cvec3##tag##_sub(CVec3##tag* self, CVec3##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z; return self; \
    } \
    static inline CVec3##tag* \
    cvec3##tag##_scale(CVec3##tag* self, T s) { \
         self->x *= s, self->y *= s, self->z *= s; return self; \
    } \
    static inline T \
    cvec3##tag##_length(CVec3##tag v) { \
        return c_sqrt_##tag(_cvec3_DOT(v, v)); \
    } \
    static inline T \
    cvec3##tag##_length2(CVec3##tag v) { \
        return _cvec3_DOT(v, v); \
    } \
    static inline CVec3##tag \
    cvec3##tag##_plus(CVec3##tag u, CVec3##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z; return u; \
    } \
    static inline CVec3##tag \
    cvec3##tag##_minus(CVec3##tag u, CVec3##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z; return u; \
    } \
    static inline CVec3##tag \
    cvec3##tag##_mult(CVec3##tag v, T s) { \
        v.x *= s, v.y *= s, v.z *= s; return v; \
    } \
    static inline CVec3##tag \
    cvec3##tag##_neg(CVec3##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z; return v; \
    } \
    static inline CVec3##tag \
    cvec3##tag##_unit(CVec3##tag v) { \
        T s = c_sqrt_##tag(_cvec3_DOT(v, v)); \
        if (s < 1e-8) return cvec3##tag##_zero; \
        s = 1.0f / s; v.x *= s, v.y *= s, v.z *= s; \
        return v; \
    } \
    static inline T \
    cvec3##tag##_dot(CVec3##tag u, CVec3##tag v) { \
        return _cvec3_DOT(u, v); \
    } \
    static inline CVec3##tag \
    cvec3##tag##_cross(CVec3##tag u, CVec3##tag v) { \
        return (CVec3##tag) {_cvec3_CROSS(u, v)}; \
    } \
    static inline T \
    cvec3##tag##_triple(CVec3##tag u, CVec3##tag v, CVec3##tag w) { \
        CVec3##tag cross = {_cvec3_CROSS(u, v)}; \
        return _cvec3_DOT(cross, w); \
    } \
    /* Reflect u on plane with given normal vector pn */ \
    static inline CVec3##tag \
    cvec3##tag##_reflect(CVec3##tag u, CVec3##tag pn) { \
        T dot2 = 2.0f * _cvec3_DOT(u, pn); \
        u.x -= dot2 * pn.x, u.y -= dot2 * pn.y, u.z -= dot2 * pn.z; \
        return u; \
    } \
    /* Refract u incident on plane with given normal vector pn */ \
    static inline CVec3##tag \
    cvec3##tag##_refract(CVec3##tag u, CVec3##tag pn, T eta) { \
        T dot = _cvec3_DOT(pn, u); \
        T k = 1.0f - eta * eta * (1.0f - dot * dot); \
        if (k < 0.0f) return cvec3##tag##_zero; \
        cvec3##tag##_scale(&u, eta); \
        cvec3##tag##_scale(&pn, eta * dot + c_sqrt_##tag(k)); \
        return *cvec3##tag##_sub(&u, pn); \
    } \
    static inline T \
    cvec3##tag##_distance(CVec3##tag u, CVec3##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z; \
        return c_sqrt_##tag(_cvec3_DOT(u, u)); \
    } \
    /* Signed distance between point u and a plane (pp, pn), pn normalized. */ \
    static inline T \
    cvec3##tag##_distanceToPlane(CVec3##tag u, CVec3##tag pp, CVec3##tag pn) { \
        u.x -= pp.x, u.y -= pp.y, u.z -= pp.z; \
        return _cvec3_DOT(u, pn); \
    } \
    /* Linear interpolation */ \
    static inline CVec3##tag \
    cvec3##tag##_lerp(CVec3##tag u, CVec3##tag v, T t) { \
        T s = 1.0f - t; \
        u.x = s*u.x + t*v.x, u.y = s*u.y + t*v.y, u.z = s*u.z + t*v.z; \
        return u; \
    } \
    /* Swizzle */ \
    static inline CVec3##tag \
    cvec3##tag##_swizzle(CVec3##tag u, const char* swz) { \
        T* a = cvec3_arr(u); \
        return (CVec3##tag) {a[swz[0] - 'x'], a[swz[1] - 'x'], a[swz[2] - 'x']}; \
    } \
    static inline bool \
    cvec3##tag##_rayPlaneIntersection(CVec3##tag* out, CVec3##tag u, CVec3##tag dir, \
                                                         CVec3##tag pp, CVec3##tag pn) { \
        T d = _cvec3_DOT(dir, pn); if (d == 0) return false; \
        T t = (_cvec3_DOT(pp, pn) - _cvec3_DOT(u, pn)) / d; \
        if (t < 0) return false; \
        u.x += dir.x*t, u.y += dir.y*t, u.z += dir.z*t; \
        *out = u; return true; \
    } \
    static inline bool \
    cvec3##tag##_equals(CVec3##tag u, CVec3##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        return u.z == v.z; \
    } \
    static inline int \
    cvec3##tag##_compare(CVec3##tag* u, CVec3##tag* v) { \
        if (u->x != v->x)  return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y)  return 1 - ((u->y < v->y)<<1); \
        return u->z == v->z ? 0 : 1 - ((u->z < v->z)<<1); \
    } \
    typedef T cvec3##tag##_value_t

#define _cvec3_DOT(u, v) (u.x*v.x + u.y*v.y + u.z*v.z)
#define _cvec3_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z
#define _cvec3_CROSS(u, v) u.y*v.z - v.y*u.z, u.z*v.x - v.z*u.x, u.x*v.y - u.y*v.x
    
declare_CVec3(d, double);
declare_CVec3(f, float);

static inline CVec3f cvec3d_tof(CVec3d v) {
    return (CVec3f) {(float) v.x, (float) v.y, (float) v.z};
}
static inline CVec3d cvec3f_tod(CVec3f v) {
    return (CVec3d) {v.x, v.y, v.z};
}

#endif
