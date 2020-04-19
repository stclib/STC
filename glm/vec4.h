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

#ifndef GLM_VEC4__H__
#define GLM_VEC4__H__

#include "vec3.h"

#ifdef GLM_NO_ANONYMOUS_STRUCT
#define _glm_VEC4(tag, T) struct glm_vec4##tag { T x, y, z, w; }
#else
#define _glm_VEC4(tag, T) union glm_vec4##tag { struct { T x, y, z, w; }; T arr[4]; }
#endif

#define glm_vec4_arr(v) (&(v).x)
#define glm_vec4f_zero ((glm_vec4f) {0.f, 0.f, 0.f, 0.f})
#define glm_vec4d_zero ((glm_vec4d) {0.0, 0.0, 0.0, 0.0})

#define declare_glm_vec4(tag, T) \
    typedef _glm_VEC4(tag, T) glm_vec4##tag; \
    static glm_vec4##tag glm_vec4##tag##_axis[4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}; \
 \
    static inline glm_vec4##tag \
    glm_vec4##tag##_init(T x, T y, T z, T w) { return (glm_vec4##tag) {x, y, z, w}; } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_fromArray(const T* a) { return (glm_vec4##tag) {a[0], a[1], a[2], a[3]}; } \
 \
    static inline glm_vec4##tag* \
    glm_vec4##tag##_assign(glm_vec4##tag* self, T x, T y, T z, T w) { \
         self->x = x, self->y = y, self->z = z, self->z = z, self->w = w; return self; \
    } \
    static inline glm_vec4##tag* \
    glm_vec4##tag##_assignArray(glm_vec4##tag* self, const T* a) { \
         self->x = a[0], self->y = a[1], self->z = a[2], self->w = a[3]; return self; \
    } \
    static inline glm_vec4##tag* \
    glm_vec4##tag##_add(glm_vec4##tag* self, glm_vec4##tag v) { \
         self->x += v.x, self->y += v.y, self->z += v.z, self->w += v.w; return self; \
    } \
    static inline glm_vec4##tag* \
    glm_vec4##tag##_sub(glm_vec4##tag* self, glm_vec4##tag v) { \
         self->x -= v.x, self->y -= v.y, self->z -= v.z, self->w -= v.w; return self; \
    } \
    static inline glm_vec4##tag* \
    glm_vec4##tag##_scale(glm_vec4##tag* self, T s) { \
         self->x *= s, self->y *= s, self->z *= s, self->w *= s; return self; \
    } \
    static inline T \
    glm_vec4##tag##_length(glm_vec4##tag v) { \
        return glm_sqrt_##tag(_glm_vec4_DOT(v, v)); \
    } \
    static inline T \
    glm_vec4##tag##_length2(glm_vec4##tag v) { \
        return _glm_vec4_DOT(v, v); \
    } \
    static inline T \
    glm_vec4##tag##_distance(glm_vec4##tag u, glm_vec4##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z, u.w -= v.w; \
        return glm_sqrt_##tag(_glm_vec4_DOT(u, u)); \
    } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_plus(glm_vec4##tag u, glm_vec4##tag v) { \
        u.x += v.x, u.y += v.y, u.z += v.z, u.w += v.w; return u; \
    } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_minus(glm_vec4##tag u, glm_vec4##tag v) { \
        u.x -= v.x, u.y -= v.y, u.z -= v.z, u.w -= v.w; return u; \
    } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_mult(glm_vec4##tag v, T s) { \
        v.x *= s, v.y *= s, v.z *= s, v.w *= s; return v; \
    } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_neg(glm_vec4##tag v) { \
        v.x = -v.x, v.y = -v.y, v.z = -v.z, v.w = -v.w; return v; \
    } \
    static inline glm_vec4##tag \
    glm_vec4##tag##_normalize(glm_vec4##tag v) { \
        T s = glm_sqrt_##tag(_glm_vec4_DOT(v, v)); \
        if (s < 1e-8) return glm_vec4##tag##_zero; \
        s = 1.0f / s; v.x *= s, v.y *= s, v.z *= s, v.w *= s; \
        return v; \
    } \
    static inline T \
    glm_vec4##tag##_dot(glm_vec4##tag u, glm_vec4##tag v) { \
        return _glm_vec4_DOT(u, v); \
    } \
    static inline glm_vec4##tag \
    glm_vec3##tag##_xyzw(glm_vec3##tag u, T w) { \
        return (glm_vec4##tag) {u.x, u.y, u.z, w}; \
    } \
    /* Swizzle */ \
    static inline glm_vec3##tag \
    glm_vec4##tag##_xyz(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.x, u.y, u.z}; \
    }; \
    glm_vec4##tag##_xzy(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.x, u.z, u.y}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec4##tag##_yxz(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.y, u.x, u.z}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec4##tag##_yzx(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.y, u.z, u.x}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec4##tag##_zxy(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.z, u.x, u.y}; \
    }; \
    static inline glm_vec3##tag \
    glm_vec4##tag##_zyx(glm_vec4##tag u) { \
        return (glm_vec3##tag) {u.z, u.y, u.x}; \
    }; \
 \
    static inline glm_vec4##tag \
    glm_vec4##tag##_swizzle(glm_vec4##tag u, const char* swz) { \
        const T* a = glm_vec4_arr(u); \
        return (glm_vec4##tag) {a[(swz[0] - 'x') & 3], \
                                a[(swz[1] - 'x') & 3], \
                                a[(swz[2] - 'x') & 3], \
                                a[(swz[3] - 'x') & 3]}; \
    } \
    static inline glm_vec3##tag \
    glm_vec4##tag##_swizzle3(glm_vec4##tag u, const char* swz) { \
        const T* a = glm_vec4_arr(u); \
        return (glm_vec3##tag) {a[(swz[0] - 'x') & 3], \
                                a[(swz[1] - 'x') & 3], \
                                a[(swz[2] - 'x') & 3]}; \
    } \
    /* Linear interpolation */ \
    static inline glm_vec4##tag \
    glm_vec4##tag##_lerp(glm_vec4##tag u, glm_vec4##tag v, T t) { \
        T s = 1.0f - t; \
        u.x = s*u.x + t*v.x, u.y = s*u.y + t*v.y, \
        u.z = s*u.z + t*v.z, u.w = s*u.w + t*v.w; \
        return u; \
    } \
    static inline bool \
    glm_vec4##tag##_equals(glm_vec4##tag u, glm_vec4##tag v) { \
        if (u.x != v.x) return false; \
        if (u.y != v.y) return false; \
        if (u.z != v.z) return false; \
        return u.w == v.w; \
    } \
    static inline int \
    glm_vec4##tag##_compare(glm_vec4##tag* u, glm_vec4##tag* v) { \
        if (u->x != v->x)  return 1 - ((u->x < v->x)<<1); \
        if (u->y != v->y)  return 1 - ((u->y < v->y)<<1); \
        if (u->z != v->z)  return 1 - ((u->z < v->z)<<1); \
        return u->w == v->w ? 0 : 1 - ((u->w < v->w)<<1); \
    } \
    typedef T glm_vec4##tag##_value_t

#define _glm_vec4_DOT(u, v) (u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w)
#define _glm_vec4_SUB(u, v) u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w
    
declare_glm_vec4(d, double);
declare_glm_vec4(f, float);

static inline glm_vec4d
glm_vec4f_tod(glm_vec4f v) {
    return (glm_vec4d) {v.x, v.y, v.z, v.w};
}
static inline glm_vec4f
glm_vec4d_tof(glm_vec4d v) {
    return (glm_vec4f) {(float) v.x, (float) v.y, (float) v.z, (float) v.w};
}

#endif
