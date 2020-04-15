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

#ifndef CQUAT__H__
#define CQUAT__H__

#include "cmat4.h"

#define cquat_arr(q)    (&(q).x)
#define cquatf_identity ((CQuatf) {0.f, 0.f, 0.f, 1.f})
#define cquatd_identity ((CQuatd) {0.0, 0.0, 0.0, 1.0})

#define declare_CQuat(tag, T) \
    typedef CVec4##tag CQuat##tag; \
 \
    static inline CQuat##tag \
    cquat##tag(T x, T y, T z, T w) { return (CQuat##tag) {x, y, z, w}; } \
 \
    static inline CQuat##tag \
    cquat##tag##_unit(CQuat##tag q) { \
        T dot = _cvec4_DOT(q, q); \
        if (dot <= 0.0f) return cquat##tag##_identity; \
        return cvec4##tag##_mult(q, 1.0f / c_sqrt_##tag(dot)); \
    } \
  \
    static inline CQuat##tag \
    cquat##tag##_fromAxis(T angle, CVec3##tag axis) { \
        T a = angle * 0.5f, c = c_cos_##tag(a), s = c_sin_##tag(a); \
        CVec3##tag u = cvec3##tag##_unit(axis); \
        return (CQuat##tag) {s * u.x, s * u.y, s * u.z, c}; \
    } \
 \
    static inline CQuat##tag \
    cquat##tag##_fromVectors(CVec3##tag u, CVec3##tag v) { \
		T norm_u_norm_v = c_sqrt_##tag(cvec3##tag##_dot(u, u) * cvec3##tag##_dot(v, v)); \
		T real_part = norm_u_norm_v + cvec3##tag##_dot(u, v); \
		CVec3##tag t; \
		if (real_part < 1.e-6f * norm_u_norm_v) { \
			/* If u and v are exactly opposite, rotate 180 degrees */ \
			/* around an arbitrary orthogonal axis. */ \
			real_part = 0; \
			t = fabs(u.x) > fabs(u.z) ? (CVec3##tag) {-u.y, u.x, 0} : (CVec3##tag) {0, -u.z, u.y}; \
		} else \
			t = cvec3##tag##_cross(u, v); /* Otherwise, build quaternion the standard way.*/ \
        CQuat##tag q = (CQuat##tag) {t.x, t.y, t.z, real_part}; \
        return cquat##tag##_unit(q); \
    } \
  \
    static inline CQuat##tag \
    cquat##tag##_fromMat4(CMat4##tag##ConstRaw m) { \
        T trace = m[0][0] + m[1][1] + m[2][2], r, rinv; \
        CQuat##tag dst; \
        if (trace >= 0.0f) { \
            r = c_sqrt_##tag(1.0f + trace); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[1][2] - m[2][1]); \
            dst.y = rinv * (m[2][0] - m[0][2]); \
            dst.z = rinv * (m[0][1] - m[1][0]); \
            dst.w = r * 0.5f; \
        } else if (m[0][0] >= m[1][1] && m[0][0] >= m[2][2]) { \
            r = c_sqrt_##tag(1.0f + m[0][0] - m[1][1] - m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = r * 0.5f; \
            dst.y = rinv * (m[0][1] + m[1][0]); \
            dst.z = rinv * (m[0][2] + m[2][0]); \
            dst.w = rinv * (m[1][2] - m[2][1]); \
        } else if (m[1][1] >= m[2][2]) { \
            r = c_sqrt_##tag(1.0f - m[0][0] + m[1][1] - m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[0][1] + m[1][0]); \
            dst.y = r * 0.5f; \
            dst.z = rinv * (m[1][2] + m[2][1]); \
            dst.w = rinv * (m[2][0] - m[0][2]); \
        } else { \
            r = c_sqrt_##tag(1.0f - m[0][0] - m[1][1] + m[2][2]); \
            rinv = 0.5f / r; \
            dst.x = rinv * (m[0][2] + m[2][0]); \
            dst.y = rinv * (m[1][2] + m[2][1]); \
            dst.z = r * 0.5f; \
            dst.w = rinv * (m[0][1] - m[1][0]); \
        } \
        return dst; \
    } \
 \
    static inline CMat4##tag \
    cquat##tag##_toMat4(CQuat##tag q) { \
        CMat4##tag dst; \
        T norm = cquat##tag##_length(q), \
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
     static inline CQuat##tag \
    cquat##tag##_conjugate(CQuat##tag q) { \
        q.x = -q.x, q.y = -q.y, q.z = -q.z; \
        return q; \
    } \
 \
    static inline CQuat##tag \
    cquat##tag##_cross(CQuat##tag p, CQuat##tag q) { \
        return (CQuat##tag) {p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y, \
                             p.w * q.y - p.x * q.z + p.y * q.w + p.z * q.x, \
                             p.w * q.z + p.x * q.y - p.y * q.x + p.z * q.w, \
                             p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z}; \
    } \
    static inline CQuat##tag \
    cquat##tag##_inverse(CQuat##tag q) { \
        q.x = -q.x, q.y = -q.y, q.z = -q.z; \
        return cvec4##tag##_mult(q, 1.0f / _cvec4_DOT(q, q)); \
    } \
    static inline T \
    cquat##tag##_angle(CQuat##tag q) { \
        /* sin(theta / 2) = length(x*x + y*y + z*z) */ \
        /* cos(theta / 2) = w */ \
        /* theta          = 2 * atan(sin(theta / 2) / cos(theta / 2)) */ \
        return 2.0f * c_atan2_##tag(c_sqrt_##tag(_cvec3_DOT(q, q)), q.w); \
    } \
    static inline T \
    cquat##tag##_imagLength(CQuat##tag q) { \
        return c_sqrt_##tag(_cvec3_DOT(q, q)); \
    } \
    typedef T cquat##tag##_value_t

#define cquatf_fromArray(arr) cvec4f_fromArray(arr)
#define cquatd_fromArray(arr) cvec4d_fromArray(arr)

#define cquatf_assign(q, x, y, z, w) cvec4f_assign(q, x, y, z, w)
#define cquatd_assign(q, x, y, z, w) cvec4d_assign(q, x, y, z, w)

#define cquatf_assignArray(q, arr) cvec4f_assignArray(q, arr)
#define cquatd_assignArray(q, arr) cvec4d_assignArray(q, arr)

#define cquatf_length(q) cvec4f_length(q)
#define cquatd_length(q) cvec4d_length(q)

#define cquatf_length2(q) cvec4f_length2(q)
#define cquatd_length2(q) cvec4d_length2(q)

#define cquatf_dot(p, q) cvec4f_dot(p, q)
#define cquatd_dot(p, q) cvec4d_dot(p, q)

#define cquatf_lerp(p, q, t) cvec4f_lerp(p, q, t)
#define cquatd_lerp(p, q, t) cvec4d_lerp(p, q, t)

#define cquatf_swizzle(q, swz) cvec4f_swizzle(q, swz)
#define cquatd_swizzle(q, swz) cvec4d_swizzle(q, swz)

#define cquatf_equals(p, q) cvec4f_equals(p, q)
#define cquatd_equals(p, q) cvec4d_equals(p, q)

#define cquatf_compare(p, q) cvec4f_compare(p, q)
#define cquatd_compare(p, q) cvec4d_compare(p, q)

#define cquatf_imag(q) cvec4f_to3(q)
#define cquatd_imag(q) cvec4d_to3(q)

#define cquatf_real(q) ((float) (q).w)
#define cquatd_real(q) ((double) (q).w)

declare_CQuat(f, float);
declare_CQuat(d, double);


static inline CQuatd cquatf_tod(CQuatf v) {
    return (CQuatd) {v.x, v.y, v.z, v.w};
}
static inline CQuatf cquatd_tof(CQuatd v) {
    return (CQuatf) {(float) v.x, (float) v.y, (float) v.z, (float) v.w};
}

#endif
