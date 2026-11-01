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
#ifndef STC_SERIALIZE_H_INCLUDED
#define STC_SERIALIZE_H_INCLUDED

#include <stdio.h>
#include <stc/sys/reflect.h>

#define _rfl_fmt(x) _Generic ((x), \
    _Bool: "B", \
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
    char const*: "\"%s\"", \
    default: "")

enum {  CSER_EXCLUDE = 1<<0,
        CSER_POINTER = 1<<1,
        STC_SER_ = 0, 
        STC_SER_CSER_EXCLUDE = CSER_EXCLUDE,
        STC_SER_CSER_POINTER = CSER_POINTER };
#define STC_EMIT_
#define STC_EMIT_CSER_EXCLUDE
#define STC_EMIT_CSER_POINTER *

static inline void _to_json(const void* self, FILE* fp)
    { fprintf(fp, "<MISSING>"); (void)self; }

#define _fx_struct_field(S, name, baretype, specifiers, ...) \
    baretype specifiers STC_EMIT_##__VA_ARGS__ name;

#define _fx_field_to_json(S, name, baretype, specifiers, ...) \
    _rfl_field_to_json(S, name, baretype, STC_EMIT_##__VA_ARGS__ self->name, STC_SER_##__VA_ARGS__);

#define _rfl_field_to_json(S, name, baretype, field, flag) do { \
    if (flag == CSER_EXCLUDE) break; /* don't serialize this field */ \
    fprintf(fp, "%s\"" #name "\": ", (first ? "" : ", ")); \
    const char* _fmt = _rfl_fmt(field); \
    if (_fmt[0] == 'B') \
        fprintf(fp, "%s", (*(_Bool*)(&field) ? "true" : "false")); \
    else if (_fmt[0] != 0) \
        fprintf(fp, _fmt, field); /* includes char* */ \
    else if (flag != CSER_POINTER) \
        baretype##_to_json(&field, fp); \
    else { /* if pointer */ } \
    first = false; \
} while(0)

#define c_struct(S, ...) \
    typedef struct S S; \
 \
    struct S { \
        c_reflect_each(S, _fx_struct_field, __VA_ARGS__) \
    }; \
 \
    void S##_to_json(const S* self, FILE* fp) { \
        bool first = true; \
        fprintf(fp, "{\"" #S "\": {"); \
        c_reflect_each(S, _fx_field_to_json, __VA_ARGS__) \
        fprintf(fp, "}}\n"); \
    } \
    struct _dummy

#endif // STC_SERIALIZE_H_INCLUDED
