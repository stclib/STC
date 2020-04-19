#include "mat4.h"

#define declare_mat4_transform(tag, T) \
    /* right handed, opengl spec functions */ \
 \
    static inline glm_mat4##tag glm_mat4##tag_##ortho(T left, T right, T bottom, T top, T zNear, T zFar) \
    { \
        glm_mat4##tag dst = glm_mat4##tag##_identity; \
        dst.m[0][0] = 2.0f / (right - left); \
        dst.m[1][1] = 2.0f / (top - bottom); \
        dst.m[2][2] = - 2.0f / (zFar - zNear); \
        dst.m[3][0] = - (right + left) / (right - left); \
        dst.m[3][1] = - (top + bottom) / (top - bottom); \
        dst.m[3][2] = - (zFar + zNear) / (zFar - zNear); \
        return dst; \
    } \
 \
    static inline glm_mat4##tag glm_mat4##tag_##frustum(T left, T right, T bottom, T top, T nearVal, T farVal) \
    { \
        glm_mat4##tag dst = glm_mat4##tag##_zero; \
        dst.m[0][0] = (2.0f * nearVal) / (right - left); \
        dst.m[1][1] = (2.0f * nearVal) / (top - bottom); \
        dst.m[2][0] = (right + left) / (right - left); \
        dst.m[2][1] = (top + bottom) / (top - bottom); \
        dst.m[2][2] = - (farVal + nearVal) / (farVal - nearVal); \
        dst.m[2][3] = - 1.0f; \
        dst.m[3][2] = - (2.0f * farVal * nearVal) / (farVal - nearVal); \
        return dst; \
    } \
     \
    static inline glm_mat4##tag glm_mat4##tag##_perspective(T fovy, T aspect, T zNear, T zFar) \
    { \
        const T tanHalfFovy = glm_tan_##tag(fovy / 2.0f); \
 \
        glm_mat4##tag dst = glm_mat4##tag##_zero; \
        dst.m[0][0] = 1.0f / (aspect * tanHalfFovy); \
        dst.m[1][1] = 1.0f / (tanHalfFovy); \
        dst.m[2][2] = - (zFar + zNear) / (zFar - zNear); \
        dst.m[2][3] = - 1.0f; \
        dst.m[3][2] = - (2.0f * zFar * zNear) / (zFar - zNear); \
        return dst; \
    }     \
     \
    static inline glm_mat4##tag glm_mat4##tag_##perspectiveFov(T fov, T width, T height, T zNear, T zFar) \
    { \
        const T rad = fov; \
        const T h = glm_cos_##tag(0.5f * rad) / glm_sin_##tag(0.5f * rad); \
        const T w = h * height / width; /* todo max(width , Height) / min(width , Height)? */ \
 \
        glm_mat4##tag dst = glm_mat4##tag##_zero; \
        dst.m[0][0] = w; \
        dst.m[1][1] = h; \
        dst.m[2][2] = - (zFar + zNear) / (zFar - zNear); \
        dst.m[2][3] = - 1.0f; \
        dst.m[3][2] = - (2.0f * zFar * zNear) / (zFar - zNear); \
        return dst; \
    }     \
     \
    static inline glm_mat4##tag glm_mat4##tag_##infinitePerspective(T fovy, T aspect, T zNear) \
    { \
        const T range = glm_tan_##tag(fovy / 2.0f) * zNear; \
        const T left = -range * aspect; \
        const T right = range * aspect; \
        const T bottom = -range; \
        const T top = range; \
 \
        glm_mat4##tag dst = glm_mat4##tag##_zero; \
        dst.m[0][0] = (2.0f * zNear) / (right - left); \
        dst.m[1][1] = (2.0f * zNear) / (top - bottom); \
        dst.m[2][2] = - 1.0f; \
        dst.m[2][3] = - 1.0f; \
        dst.m[3][2] = - 2.0f * zNear; \
        return dst; \
    } \
 \
    static inline glm_mat4##tag translate(const glm_mat4##tag##m m, glm_vec3##tag v) \
    { \
        glm_mat4##tag dst = *(const glm_mat4##tag *) m; \
        glm_vec4##tag##_add(&dst.v[3], glm_vec4##tag##_mult(dst.v[0], v.x) \
                                     + glm_vec4##tag##_mult(dst.v[1], v.y) \
                                     + glm_vec4##tag##_mult(dst.v[2], v.z); \
        return dst; \
    } \
 \
 \
    static inline glm_mat4##tag rotate(glm_mat4##tag##m m, T angle, glm_vec3##tag v) \
    { \
        const T a = angle; \
        const T c = glm_cos_##tag(a); \
        const T s = glm_sin_##tag(a); \
 \
        glm_vec3##tag axis = glm_vec3##tag##_normalize(v); \
        glm_vec3##tag temp = glm_vec3##tag##_mult(axis, 1.0f - c); \
 \
        glm_mat4##tag rot; \
        rot.m[0][0] = c + temp.x * axis.x; \
        rot.m[0][1] = temp.x * axis.y + s * axis.z; \
        rot.m[0][2] = temp.x * axis.z - s * axis.y; \
 \
        rot.m[1][0] = temp.y * axis.x - s * axis.z; \
        rot.m[1][1] = c + temp.y * axis.y; \
        rot.m[1][2] = temp.y * axis.z + s * axis.x; \
 \
        rot.m[2][0] = temp.z * axis.x + s * axis.y; \
        rot.m[2][1] = temp.z * axis.y - s * axis.x; \
        rot.m[2][2] = c + temp.z * axis.z; \
 \
        glm_mat4##tag dst; \
        const glm_vec4##tag* mv = (const glm_vec4##tag *) m; \
         dst.m[0] = glm_vec4##tag##_mult(mv[0], rot.m[0][0]) \
                 + glm_vec4##tag##_mult(mv[1], rot.m[0][1]) \
                 + glm_vec4##tag##_mult(mv[2], rot.m[0][2]); \
        dst.m[1] = glm_vec4##tag##_mult(mv[0], rot.m[1][0]) \
                 + glm_vec4##tag##_mult(mv[1], rot.m[1][1]) \
                 + glm_vec4##tag##_mult(mv[2], rot.m[1][2]); \
        dst.m[2] = glm_vec4##tag##_mult(mv[0], rot.m[2][0]) \
                 + glm_vec4##tag##_mult(mv[1], rot.m[2][1]) \
                 + glm_vec4##tag##_mult(mv[2], rot.m[2][2]); \
        dst.m[3] = mv[3]; \
        return dst; \
    } \
 \
 \
    static inline glm_mat4##tag scale(glm_mat4##tag##m m, glm_vec3##tag v) \
    { \
        glm_mat4##tag dst; \
        dst.m[0] = m[0] * v[0]; \
        dst.m[1] = m[1] * v[1]; \
        dst.m[2] = m[2] * v[2]; \
        dst.m[3] = m[3]; \
        return dst; \
    } \
 \
    static inline glm_mat4##tag lookAtRH(glm_vec3##tag eye, glm_vec3##tag center, glm_vec3##tag up) \
    { \
        const glm_vec3##tag f(normalize(center - eye)); \
        const glm_vec3##tag s(normalize(cross(f, up))); \
        const glm_vec3##tag u(cross(s, f)); \
 \
        glm_mat4##tag dst(1); \
        dst.m[0][0] = s.x; \
        dst.m[1][0] = s.y; \
        dst.m[2][0] = s.z; \
        dst.m[0][1] = u.x; \
        dst.m[1][1] = u.y; \
        dst.m[2][1] = u.z; \
        dst.m[0][2] =-f.x; \
        dst.m[1][2] =-f.y; \
        dst.m[2][2] =-f.z; \
        dst.m[3][0] =-dot(s, eye); \
        dst.m[3][1] =-dot(u, eye); \
        dst.m[3][2] = dot(f, eye); \
        return dst; \
    }
