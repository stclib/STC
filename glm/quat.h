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

#ifndef GLM_QUAT__H__
#define GLM_QUAT__H__

#include "mat4.h"

#define glm_quat_arr(q)    (&(q).x)
#define glm_quatf_identity ((glm_quatf) {0.f, 0.f, 0.f, 1.f})
#define glm_quatd_identity ((glm_quatd) {0.0, 0.0, 0.0, 1.0})

#define declare_glm_quat(tag, T) \
    typedef glm_vec4##tag glm_quat##tag; \
 \
    static inline glm_quat##tag \
    glm_quat##tag##_init(T x, T y, T z, T w) { return (glm_quat##tag) {x, y, z, w}; } \
 \
    static inline glm_quat##tag \
    glm_quat##tag##_unit(glm_quat##tag q) { \
        T dot = _glm_vec4_DOT(q, q); \
        if (dot <= 0.0f) return glm_quat##tag##_identity; \
        return glm_vec4##tag##_mult(q, 1.0f / glm_sqrt_##tag(dot)); \
    } \
  \
    static inline glm_quat##tag \
    glm_quat##tag##_fromAxis(T angle, glm_vec3##tag axis) { \
        T a = angle * 0.5f, c = glm_cos_##tag(a), s = glm_sin_##tag(a); \
        glm_vec3##tag u = glm_vec3##tag##_unit(axis); \
        return (glm_quat##tag) {s * u.x, s * u.y, s * u.z, c}; \
    } \
 \
    static inline glm_quat##tag \
    glm_quat##tag##_fromVectors(glm_vec3##tag u, glm_vec3##tag v) { \
        T d = glm_sqrt_##tag(_glm_vec3_DOT(u, u) * _glm_vec3_DOT(v, v)); \
        T real_part = d + _glm_vec3_DOT(u, v); \
        glm_vec3##tag t; \
        if (real_part < 1.e-6f * d) { \
            /* If u and v are exactly opposite, rotate 180 degrees */ \
            /* around an arbitrary orthogonal axis. */ \
            real_part = 0; \
            t = fabs(u.x) > fabs(u.z) ? (glm_vec3##tag) {-u.y, u.x, 0} : (glm_vec3##tag) {0, -u.z, u.y}; \
        } else  /* Build quaternion the standard way.*/ \
            t = _glm_vec3_CROSS(u, v); \
        glm_quat##tag q = (glm_quat##tag) {t.x, t.y, t.z, real_part}; \
        return glm_quat##tag##_unit(q); \
    } \
  \
    static inline glm_quat##tag \
    glm_quat##tag##_fromMat4(glm_mat4##tag##m m) { \
        T trace = m[0][0] + m[1][1] + m[2][2], r, rinv; \
        glm_quat##tag dst; \
        if (trace >= 0.0f) { \
            r = glm_sqrt_##tag(1.0f + trace); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[1][2] - m[2][1]); \
            dst.y = rinv * (m[2][0] - m[0][2]); \
            dst.z = rinv * (m[0][1] - m[1][0]); \
            dst.w = r * 0.5f; \
        } else if (m[0][0] >= m[1][1] && m[0][0] >= m[2][2]) { \
            r = glm_sqrt_##tag(1.0f + m[0][0] - m[1][1] - m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = r * 0.5f; \
            dst.y = rinv * (m[0][1] + m[1][0]); \
            dst.z = rinv * (m[0][2] + m[2][0]); \
            dst.w = rinv * (m[1][2] - m[2][1]); \
        } else if (m[1][1] >= m[2][2]) { \
            r = glm_sqrt_##tag(1.0f - m[0][0] + m[1][1] - m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[0][1] + m[1][0]); \
            dst.y = r * 0.5f; \
            dst.z = rinv * (m[1][2] + m[2][1]); \
            dst.w = rinv * (m[2][0] - m[0][2]); \
        } else { \
            r = glm_sqrt_##tag(1.0f - m[0][0] - m[1][1] + m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[0][2] + m[2][0]); \
            dst.y = rinv * (m[1][2] + m[2][1]); \
            dst.z = r * 0.5f; \
            dst.w = rinv * (m[0][1] - m[1][0]); \
        } \
        return dst; \
    } \
 \
    static inline glm_mat4##tag \
    glm_quat##tag##_toMat4(glm_quat##tag q) { \
        glm_mat4##tag dst; \
        T norm = glm_quat##tag##_length(q), \
          s  = norm > 0.0f ? 2.0f / norm : 0.0f, \
          xx = s * q.x * q.x, xy = s * q.x * q.y, wx = s * q.w * q.x, \
          yy = s * q.y * q.y, yz = s * q.y * q.z, wy = s * q.w * q.y, \
          zz = s * q.z * q.z, xz = s * q.x * q.z, wz = s * q.w * q.z; \
 \
        dst.m[0][0] = 1.0f - yy - zz; \
        dst.m[1][1] = 1.0f - xx - zz; \
        dst.m[2][2] = 1.0f - xx - yy; \
        dst.m[0][1] = xy + wz; \
        dst.m[1][2] = yz + wx; \
        dst.m[2][0] = xz + wy; \
        dst.m[1][0] = xy - wz; \
        dst.m[2][1] = yz - wx; \
        dst.m[0][2] = xz - wy; \
        dst.m[0][3] = dst.m[1][3] = dst.m[2][3] = 0.0f; \
        dst.m[3][0] = dst.m[3][1] = dst.m[3][2] = 0.0f; \
        dst.m[3][3] = 1.0f; \
        return dst; \
    }  \
 \
     static inline glm_quat##tag \
    glm_quat##tag##_conjugate(glm_quat##tag q) { \
        q.x = -q.x, q.y = -q.y, q.z = -q.z; \
        return q; \
    } \
 \
    static inline glm_quat##tag \
    glm_quat##tag##_cross(glm_quat##tag p, glm_quat##tag q) { \
        return (glm_quat##tag) {p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y, \
                                p.w * q.y - p.x * q.z + p.y * q.w + p.z * q.x, \
                                p.w * q.z + p.x * q.y - p.y * q.x + p.z * q.w, \
                                p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z}; \
    } \
    static inline glm_quat##tag \
    glm_quat##tag##_inverse(glm_quat##tag q) { \
        q.x = -q.x, q.y = -q.y, q.z = -q.z; \
        return glm_vec4##tag##_mult(q, 1.0f / _glm_vec4_DOT(q, q)); \
    } \
    static inline T \
    glm_quat##tag##_angle(glm_quat##tag q) { \
        /* sin(theta / 2) = length(x*x + y*y + z*z) */ \
        /* cos(theta / 2) = w */ \
        /* theta          = 2 * atan(sin(theta / 2) / cos(theta / 2)) */ \
        return 2.0f * glm_atan2_##tag(glm_sqrt_##tag(_glm_vec3_DOT(q, q)), q.w); \
    } \
    static inline T \
    glm_quat##tag##_imagLength(glm_quat##tag q) { \
        return glm_sqrt_##tag(_glm_vec3_DOT(q, q)); \
    } \
    typedef T glm_quat##tag##_value_t

#define glm_quatf_fromArray(arr) glm_vec4f_fromArray(arr)
#define glm_quatd_fromArray(arr) glm_vec4d_fromArray(arr)

#define glm_quatf_assign(q, x, y, z, w) glm_vec4f_assign(q, x, y, z, w)
#define glm_quatd_assign(q, x, y, z, w) glm_vec4d_assign(q, x, y, z, w)

#define glm_quatf_assignArray(q, arr) glm_vec4f_assignArray(q, arr)
#define glm_quatd_assignArray(q, arr) glm_vec4d_assignArray(q, arr)

#define glm_quatf_length(q) glm_vec4f_length(q)
#define glm_quatd_length(q) glm_vec4d_length(q)

#define glm_quatf_length2(q) glm_vec4f_length2(q)
#define glm_quatd_length2(q) glm_vec4d_length2(q)

#define glm_quatf_dot(p, q) glm_vec4f_dot(p, q)
#define glm_quatd_dot(p, q) glm_vec4d_dot(p, q)

#define glm_quatf_lerp(p, q, t) glm_vec4f_lerp(p, q, t)
#define glm_quatd_lerp(p, q, t) glm_vec4d_lerp(p, q, t)

#define glm_quatf_swizzle(q, swz) glm_vec4f_swizzle(q, swz)
#define glm_quatd_swizzle(q, swz) glm_vec4d_swizzle(q, swz)

#define glm_quatf_equals(p, q) glm_vec4f_equals(p, q)
#define glm_quatd_equals(p, q) glm_vec4d_equals(p, q)

#define glm_quatf_compare(p, q) glm_vec4f_compare(p, q)
#define glm_quatd_compare(p, q) glm_vec4d_compare(p, q)

#define glm_quatf_imag(q) glm_vec4f_to3(q)
#define glm_quatd_imag(q) glm_vec4d_to3(q)

#define glm_quatf_real(q) ((float) (q).w)
#define glm_quatd_real(q) ((double) (q).w)

declare_glm_quat(f, float);
declare_glm_quat(d, double);


static inline glm_quatd glm_quatf_tod(glm_quatf v) {
    return (glm_quatd) {v.x, v.y, v.z, v.w};
}
static inline glm_quatf glm_quatd_tof(glm_quatd v) {
    return (glm_quatf) {(float) v.x, (float) v.y, (float) v.z, (float) v.w};
}

#endif
