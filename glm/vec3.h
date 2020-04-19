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

#ifndef GLM_VEC3__H__
#define GLM_VEC3__H__

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define glm_sqrt_f(x) sqrtf(x)
#define glm_sqrt_d(x) sqrt(x)
#define glm_sin_f(x) sinf(x)
#define glm_sin_d(x) sin(x)
#define glm_cos_f(x) cosf(x)
#define glm_cos_d(x) cos(x)
#define glm_atan2_f(x, y) atan2f(x, y)
#define glm_atan2_d(x, y) atan2(x, y)

#ifdef GLM_NO_ANONYMOUS_STRUCT
#define _glm_VEC3(tag, T) struct glm_vec3##tag { T x, y, z; }
#else
#define _glm_VEC3(tag, T) union glm_vec3##tag { struct { T x, y, z; }; T arr[3]; }
#endif

#define glm_vec3_arr(v) (&(v).x)
#define glm_vec3f_zero ((glm_vec3f) {0.f, 0.f, 0.f})
#define glm_vec3d_zero ((glm_vec3d) {0.0, 0.0, 0.0})

#define declare_glm_vec3(tag, T) \
    typedef _glm_VEC3(tag, T) glm_vec3##tag; \
    static glm_vec3##tag \
    glm_vec3##tag##_axis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; \
 \
    static inline glm_vec3##tag \
    glm_vec3##tag##_init(T x, T y, T z) { return (glm_vec3##tag) {x, y, z}; } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_fromArray(const T* a) { return (glm_vec3##tag) {a[0], a[1], a[2]}; } \
 \
    static inline glm_vec3##tag* \
    glm_vec3##tag##_assign(glm_vec3##tag* self, T x, T y, T z) { \
         self->x = x, self->y = y, self->z = z; return self; \
    } \
    static inline glm_vec3##tag* \
    glm_vec3##tag##_assignArray(glm_vec3##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2]; return self; \
    } \
    static inline glm_vec3##tag* \
    glm_vec3##tag##_add(glm_vec3##tag* self, glm_vec3##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z; return self; \
    } \
    static inline glm_vec3##tag* \
    glm_vec3##tag##_sub(glm_vec3##tag* self, glm_vec3##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z; return self; \
    } \
    static inline glm_vec3##tag* \
    glm_vec3##tag##_scale(glm_vec3##tag* self, T s) { \
         self->x *= s, self->y *= s, self->z *= s; return self; \
    } \
    static inline T \
    glm_vec3##tag##_length(glm_vec3##tag v) { \
        return glm_sqrt_##tag(_glm_vec3_DOT(v, v)); \
    } \
    static inline T \
    glm_vec3##tag##_length2(glm_vec3##tag v) { \
        return _glm_vec3_DOT(v, v); \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_plus(glm_vec3##tag u, glm_vec3##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z; return u; \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_minus(glm_vec3##tag u, glm_vec3##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z; return u; \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_mult(glm_vec3##tag v, T s) { \
        v.x *= s, v.y *= s, v.z *= s; return v; \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_neg(glm_vec3##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z; return v; \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_normalize(glm_vec3##tag v) { \
        T s = 1.0f / glm_sqrt_##tag(_glm_vec3_DOT(v, v)); \
        s; v.x *= s, v.y *= s, v.z *= s; \
        return v; \
    } \
    static inline T \
    glm_vec3##tag##_dot(glm_vec3##tag u, glm_vec3##tag v) { \
        return _glm_vec3_DOT(u, v); \
    } \
    static inline glm_vec3##tag \
    glm_vec3##tag##_cross(glm_vec3##tag u, glm_vec3##tag v) { \
        return (glm_vec3##tag) {_glm_vec3_CROSS(u, v)}; \
    } \
    static inline T \
    glm_vec3##tag##_triple(glm_vec3##tag u, glm_vec3##tag v, glm_vec3##tag w) { \
        glm_vec3##tag cross = {_glm_vec3_CROSS(u, v)}; \
        return _glm_vec3_DOT(cross, w); \
    } \
    /* Reflect u on plane with given normal vector pn */ \
    static inline glm_vec3##tag \
    glm_vec3##tag##_reflect(glm_vec3##tag u, glm_vec3##tag pn) { \
        T dot2 = 2.0f * _glm_vec3_DOT(u, pn); \
        u.x -= dot2 * pn.x, u.y -= dot2 * pn.y, u.z -= dot2 * pn.z; \
        return u; \
    } \
    /* Refract u incident on plane with given normal vector pn */ \
    static inline glm_vec3##tag \
    glm_vec3##tag##_refract(glm_vec3##tag u, glm_vec3##tag pn, T eta) { \
        T dot = _glm_vec3_DOT(pn, u); \
        T k = 1.0f - eta * eta * (1.0f - dot * dot); \
        if (k < 0.0f) return glm_vec3##tag##_zero; \
        glm_vec3##tag##_scale(&u, eta); \
        glm_vec3##tag##_scale(&pn, eta * dot + glm_sqrt_##tag(k)); \
        return *glm_vec3##tag##_sub(&u, pn); \
    } \
    static inline T \
    glm_vec3##tag##_distance(glm_vec3##tag u, glm_vec3##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z; \
        return glm_sqrt_##tag(_glm_vec3_DOT(u, u)); \
    } \
    /* Signed distance between point u and a plane (pp, pn), pn normalized. */ \
    static inline T \
    glm_vec3##tag##_distanceToPlane(glm_vec3##tag u, glm_vec3##tag pp, glm_vec3##tag pn) { \
        u.x -= pp.x, u.y -= pp.y, u.z -= pp.z; \
        return _glm_vec3_DOT(u, pn); \
    } \
    /* Linear interpolation */ \
    static inline glm_vec3##tag \
    glm_vec3##tag##_lerp(glm_vec3##tag u, glm_vec3##tag v, T t) { \
        T s = 1.0f - t; \
        u.x = s*u.x + t*v.x, u.y = s*u.y + t*v.y, u.z = s*u.z + t*v.z; \
        return u; \
    } \
    /* Swizzle */ \
    glm_vec3##tag##_xzy(glm_vec3##tag u) { \
        return (glm_vec3##tag) {u.x, u.z, u.y}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec3##tag##_yxz(glm_vec3##tag u) { \
        return (glm_vec3##tag) {u.y, u.x, u.z}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec3##tag##_yzx(glm_vec3##tag u) { \
        return (glm_vec3##tag) {u.y, u.z, u.x}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec3##tag##_zxy(glm_vec3##tag u) { \
        return (glm_vec3##tag) {u.z, u.x, u.y}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec3##tag##_zyx(glm_vec3##tag u) { \
        return (glm_vec3##tag) {u.z, u.y, u.x}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec3##tag##_swizzle(glm_vec3##tag u, const char* swz) { \
        T* a = glm_vec3_arr(u); \
        return (glm_vec3##tag) {a[swz[0] - 'x'], a[swz[1] - 'x'], a[swz[2] - 'x']}; \
    } \
    static inline bool \
    glm_vec3##tag##_rayPlaneIntersection(glm_vec3##tag* out, glm_vec3##tag u, glm_vec3##tag dir, \
                                                         glm_vec3##tag pp, glm_vec3##tag pn) { \
        T d = _glm_vec3_DOT(dir, pn); if (d == 0) return false; \
        T t = (_glm_vec3_DOT(pp, pn) - _glm_vec3_DOT(u, pn)) / d; \
        if (t < 0) return false; \
        u.x += dir.x*t, u.y += dir.y*t, u.z += dir.z*t; \
        *out = u; return true; \
    } \
    static inline bool \
    glm_vec3##tag##_equals(glm_vec3##tag u, glm_vec3##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        return u.z == v.z; \
    } \
    static inline int \
    glm_vec3##tag##_compare(glm_vec3##tag* u, glm_vec3##tag* v) { \
        if (u->x != v->x)  return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y)  return 1 - ((u->y < v->y)<<1); \
        return u->z == v->z ? 0 : 1 - ((u->z < v->z)<<1); \
    } \
    typedef T glm_vec3##tag##_value_t

#define _glm_vec3_DOT(u, v) (u.x*v.x + u.y*v.y + u.z*v.z)
#define _glm_vec3_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z
#define _glm_vec3_CROSS(u, v) u.y*v.z - v.y*u.z, u.z*v.x - v.z*u.x, u.x*v.y - u.y*v.x
    
declare_glm_vec3(d, double);
declare_glm_vec3(f, float);

static inline glm_vec3f glm_vec3d_tof(glm_vec3d v) {
    return (glm_vec3f) {(float) v.x, (float) v.y, (float) v.z};
}
static inline glm_vec3d glm_vec3f_tod(glm_vec3f v) {
    return (glm_vec3d) {v.x, v.y, v.z};
}

#endif
