/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// This implements serialization in C.
// ref: https://www.boost.org/doc/libs/latest/libs/serialization/doc/tutorial.html
//      https://github.com/pantonante/boost-serialization-examples/tree/master/simple
#ifndef STC_SERIALIZE_H_INCLUDED
#define STC_SERIALIZE_H_INCLUDED

#include <stdio.h>
#include <stc/sys/reflect.h>

#define _rfl_fmt(x) _Generic ((x), \
    _Bool: "%d", \
    unsigned char: "%hhu", \
    signed char: "%hhd", \
    char: "%c", \
    short: "%hd", \
    unsigned short: "%hu", \
    int: "%d", \
    unsigned: "%u", \
    long: "%ld", \
    unsigned long: "%lu", \
    long long: "%lld", \
    unsigned long long: "%llu", \
    float: "%g", \
    double: "%g", \
    long double: "%Lg", \
    char*: "\"%s\"", \
    wchar_t*: "\"%ls\"", \
    void*: "%p", \
    const char*: "\"%s\"", \
    const wchar_t*: "\"%ls\"", \
    const void*: "%p", \
    default: 0)

#define _fx_typedef(S, name, flag, objtype, ...) \
    typedef objtype __VA_ARGS__ S##_##name##_t;

#define _fx_struct_field(S, name, ...) \
    S##_##name##_t name;

#define _fx_field_to_json(S, name, flag, objtype, ...) \
    _rfl_field_to_json(S, name, (flag+0), self->name, objtype##_to_json, __VA_ARGS__);

static inline void _to_json(const void* self, FILE* fp)
    {}

#define _rfl_field_to_json(S, name, flag, field, tojson, ...) do { \
    if (flag == 1) break; /* don't serialize this field */ \
    char _buf[128]; \
    const char* _fmt = _rfl_fmt(field); \
    if (_fmt == NULL) { \
        fprintf(fp, "%s\"" #name "\": ", (first ? "" : ", ")); \
        tojson(&field, fp); \
    } else { \
        sprintf(_buf, "%s\"" #name "\": %s", (first ? "  " : ", "), _fmt); \
        fprintf(fp, _buf, field); \
    } first = false; \
} while(0)

#define c_struct(S, ...) \
    typedef struct S S; \
    c_reflect_each(S, _fx_typedef, __VA_ARGS__) \
 \
    struct S { \
        c_reflect_each(S, _fx_struct_field, __VA_ARGS__) \
    }; \
 \
    void S##_to_json(const S* self, FILE* fp) { \
        fprintf(fp, "{\"" #S "\": {"); \
        bool first = true; \
        c_reflect_each(S, _fx_field_to_json, __VA_ARGS__) \
        fprintf(fp, "}}\n"); \
    } \
    struct _dummy

#endif // STC_SERIALIZE_H_INCLUDED
