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

#define cvec4_data(v)         (&(v).x)

#define declare_CVec4(tag, T) \
    typedef struct CVec4##tag { T x, y, z, w; } CVec4##tag; \
    static CVec4##tag cvec4##tag##_null = {0, 0, 0, 0}; \
    static CVec4##tag cvec4##tag##_axis[4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}; \
 \
    static inline CVec4##tag cvec4##tag(T x, T y, T z, T w) { CVec4##tag v = {x, y, z, w}; return v; } \
    static inline CVec4##tag cvec4##tag##_init(const T* a) { CVec4##tag v = {a[0], a[1], a[2], a[3]}; return v; } \
 \
    static inline CVec4##tag cvec4##tag##_set(CVec4##tag* self, T x, T y, T z, T w) { \
         self->x = x, self->y = y, self->z = z, self->z = z, self->w = w; return *self; \
    } \
    static inline CVec4##tag cvec4##tag##_setv(CVec4##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2], self->w = a[3]; return *self; \
    } \
    static inline CVec4##tag cvec4##tag##_add(CVec4##tag* self, CVec4##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z, self->w += v.w; return *self; \
    } \
    static inline CVec4##tag cvec4##tag##_subtract(CVec4##tag* self, CVec4##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z, self->w -= v.w; return *self; \
    } \
    static inline CVec4##tag cvec4##tag##_scale(CVec4##tag* self, double s) { \
         self->x = (T)(self->x*s), self->y = (T)(self->y*s), self->z = (T)(self->z*s), self->w = (T)(self->w*s); \
         return *self; \
    } \
    static inline double cvec4##tag##_length(CVec4##tag v) { \
        return sqrt(_cvec4_DOT(v, v)); \
    } \
    static inline double cvec4##tag##_length2(CVec4##tag v) { \
        return _cvec4_DOT(v, v); \
    } \
    static inline CVec4##tag cvec4##tag##_plus(CVec4##tag u, CVec4##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z, u.w += v.w; return u; \
    } \
    static inline CVec4##tag cvec4##tag##_minus(CVec4##tag u, CVec4##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z, u.w -= v.w; return u; \
    } \
    static inline CVec4##tag cvec4##tag##_mult(CVec4##tag v, double s) { \
        v.x = (T)(s*v.x), v.y = (T)(s*v.y), v.z = (T)(s*v.z), v.w = (T)(s*v.w); return v; \
    } \
    static inline CVec4##tag cvec4##tag##_multInverse(CVec4##tag v, double s) { \
        v.x = (T)(s/v.x), v.y = (T)(s/v.y), v.z = (T)(s/v.z), v.w = (T)(s/v.w); return v; \
    } \
    static inline CVec4##tag cvec4##tag##_neg(CVec4##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z, v.w = -v.w; return v; \
    } \
    static inline CVec4##tag cvec4##tag##_unit(CVec4##tag v) { \
        double s = 1.0 / sqrt(_cvec4_DOT(v, v)); \
        v.x = (T)(s*v.x), v.y = (T)(s*v.y), v.z = (T)(s*v.z), v.w = (T)(s*v.w); return v; \
    } \
    static inline double cvec4##tag##_dot(CVec4##tag u, CVec4##tag v) { \
        return _cvec4_DOT(u, v); \
    } \
    /* Linear interpolation */ \
    static inline CVec4##tag cvec4##tag##_lerp(CVec4##tag u, CVec4##tag v, double t) { \
        double m = 1.0 - t; \
        u.x = (T)(m*u.x + t*v.x), u.y = (T)(m*u.y + t*v.y), u.z = (T)(m*u.z + t*v.z), u.w = (T)(m*u.w + t*v.w); \
        return u; \
    } \
    static inline bool cvec4##tag##_equals(CVec4##tag u, CVec4##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        if (u.z != v.z) return false; \
        return u.w == v.w; \
    } \
    static inline int cvec4##tag##_compare(CVec4##tag* u, CVec4##tag* v) { \
        if (u->x != v->x) return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y) return 1 - ((u->y < v->y)<<1); \
        if (u->z != v->z) return 1 - ((u->z < v->z)<<1); \
        return u->w == v->w ? 0 : 1 - ((u->w < v->w)<<1); \
    } \
    typedef T cvec4_##tag##_value_t

#define _cvec4_DOT(u, v) ((double) u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w)
#define _cvec4_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w
    
declare_CVec4(d, double);
declare_CVec4(f, float);
declare_CVec4(i, int32_t);
//declare_CVec4(ui, uint32_t);
//declare_CVec4(s, int16_t);
//declare_CVec4(us, uint16_t);
declare_CVec4(ub, uint8_t);

static inline CVec4d cvec4f_to4d(CVec4f v) {
	CVec4d u = {v.x, v.y, v.z, v.w}; return u;
}
static inline CVec4f cvec4d_to4f(CVec4d v) {
	CVec4f u = {(float) v.x, (float) v.y, (float) v.z, (float) v.w}; return u;
}
static inline CVec3d cvec4d_to3d(CVec4d v) {
	CVec3d u = {v.x, v.y, v.z}; return u;
}
static inline CVec4f cvec4d_to3f(CVec4d v) {
	CVec3f u = {(float) v.x, (float) v.y, (float) v.z}; return u;
}

#endif
