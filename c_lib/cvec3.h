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

#define cvec3_data(v)         (&(v).x)

#define declare_CVec3(tag, T) \
    typedef struct CVec3##tag { T x, y, z; } CVec3##tag; \
    static CVec3##tag cvec3##tag##_null = {0, 0, 0}; \
    static CVec3##tag cvec3##tag##_axis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; \
 \
    static inline CVec3##tag cvec3##tag(T x, T y, T z) { CVec3##tag v = {x, y, z}; return v; } \
    static inline CVec3##tag cvec3##tag##_init(const T* a) { CVec3##tag v = {a[0], a[1], a[2]}; return v; } \
 \
    static inline CVec3##tag cvec3##tag##_set(CVec3##tag* self, T x, T y, T z) { \
         self->x = x, self->y = y, self->z = z; return *self; \
    } \
    static inline CVec3##tag cvec3##tag##_setv(CVec3##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2]; return *self; \
    } \
    static inline CVec3##tag cvec3##tag##_add(CVec3##tag* self, CVec3##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z; return *self; \
    } \
    static inline CVec3##tag cvec3##tag##_subtract(CVec3##tag* self, CVec3##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z; return *self; \
    } \
    static inline CVec3##tag cvec3##tag##_scale(CVec3##tag* self, double s) { \
         self->x = (T)(self->x*s), self->y = (T)(self->x*s), self->z = (T)(self->x*s); \
         return *self; \
    } \
    static inline double cvec3##tag##_length(CVec3##tag v) { \
        return sqrt(_cvec3_DOT(v, v)); \
    } \
    static inline double cvec3##tag##_length2(CVec3##tag v) { \
        return _cvec3_DOT(v, v); \
    } \
    static inline CVec3##tag cvec3##tag##_plus(CVec3##tag u, CVec3##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z; return u; \
    } \
    static inline CVec3##tag cvec3##tag##_minus(CVec3##tag u, CVec3##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z; return u; \
    } \
    static inline CVec3##tag cvec3##tag##_mult(CVec3##tag v, double s) { \
        v.x = (T)(s*v.x), v.y = (T)(s*v.y), v.z = (T)(s*v.z); return v; \
    } \
    static inline CVec3##tag cvec3##tag##_multInverse(CVec3##tag v, double s) { \
        v.x = (T)(s/v.x), v.y = (T)(s/v.y), v.z = (T)(s/v.z); return v; \
    } \
    static inline CVec3##tag cvec3##tag##_neg(CVec3##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z; return v; \
    } \
    static inline CVec3##tag cvec3##tag##_unit(CVec3##tag v) { \
        double s = 1.0 / sqrt(_cvec3_DOT(v, v)); \
        v.x = (T)(v.x*s), v.y = (T)(v.y*s), v.z = (T)(v.z*s); return v; \
    } \
    static inline double cvec3##tag##_dot(CVec3##tag u, CVec3##tag v) { \
        return _cvec3_DOT(u, v); \
    } \
    static inline CVec3##tag cvec3##tag##_cross(CVec3##tag u, CVec3##tag v) { \
        CVec3##tag c = {_cvec3_CROSS(u, v)}; \
        return c; \
    } \
    static inline double cvec3##tag##_triple(CVec3##tag u, CVec3##tag v, CVec3##tag w) { \
        CVec3##tag c = {_cvec3_CROSS(u, v)}; \
        return _cvec3_DOT(c, w); \
    } \
    /* Reflect u on plane with given normal vector n */ \
    static inline CVec3##tag cvec3##tag##_reflect(CVec3##tag u, CVec3##tag pn) { \
        double dot2 = 2.0 * _cvec3_DOT(u, pn); \
        CVec3##tag w = {(T)(u.x - dot2*pn.x), (T)(u.y - dot2*pn.y), (T)(u.z - dot2*pn.z)}; \
        return w; \
    } \
    /* Signed distance between point u and a plane (pp, pn), pn normalized. */ \
    static inline double cvec3##tag##_distanceToPlane(CVec3##tag u, CVec3##tag pp, CVec3##tag pn) { \
        u.x -= pp.x, u.y -= pp.y, u.z -= pp.z; \
        return _cvec3_DOT(u, pn); \
    } \
    /* Linear interpolation */ \
    static inline CVec3##tag cvec3##tag##_lerp(CVec3##tag u, CVec3##tag v, double t) { \
        double m = 1.0 - t; \
        CVec3##tag w = {(T)(m*u.x + t*v.x), (T)(m*u.y + t*v.y), (T)(m*u.z + t*v.z)}; \
        return w; \
    } \
    static inline bool cvec3##tag##_rayPlaneIntersection(CVec3##tag* out, CVec3##tag u, CVec3##tag dir, \
                                                         CVec3##tag pp, CVec3##tag pn) { \
        double d = _cvec3_DOT(dir, pn); if (d == 0) return false; \
        double t = (_cvec3_DOT(pp, pn) - _cvec3_DOT(u, pn)) / d; \
        if (t < 0) return false; \
        u.x += (T) (dir.x*t), u.y += (T) (dir.y*t), u.z += (T) (dir.z*t); \
        *out = u; return true; \
    } \
    static inline bool cvec3##tag##_equals(CVec3##tag u, CVec3##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        return u.z == v.z; \
    } \
    static inline int cvec3##tag##_compare(CVec3##tag* u, CVec3##tag* v) { \
        if (u->x != v->x) return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y) return 1 - ((u->y < v->y)<<1); \
        return u->z == v->z ? 0 : 1 - ((u->z < v->z)<<1); \
    } \
    typedef T cvec3_##tag##_value_t

#define _cvec3_DOT(u, v) ((double) u.x*v.x + u.y*v.y + u.z*v.z)
#define _cvec3_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z
#define _cvec3_CROSS(u, v) u.y*v.z - v.y*u.z, u.z*v.x - v.z*u.x, u.x*v.y - u.y*v.x
    
declare_CVec3(d, double);
declare_CVec3(f, float);
declare_CVec3(i, int32_t);
declare_CVec3(ui, uint32_t);
declare_CVec3(s, int16_t);
declare_CVec3(us, uint16_t);
declare_CVec3(ub, uint8_t);


#endif
