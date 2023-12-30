/*
This is a Unix port of the Plan 9 regular expression library, by Rob Pike.

Copyright © 2021 Plan 9 Foundation
Copyright © 2022 Tyge Løvset, for additions made in 2022.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef STC_CREGEX_H_INCLUDED
#define STC_CREGEX_H_INCLUDED
/*
 * cregex.h
 *
 * This is a extended version of regexp9, supporting UTF8 input, common
 * shorthand character classes, ++.
 */
#include <stdbool.h>
#include <string.h>
#include "types.h" // csview, cstr types
#include "common.h"

enum {
    CREG_DEFAULT = 0,

    /* compile-flags */
    CREG_DOTALL = 1<<0,    /* dot matches newline too */
    CREG_ICASE = 1<<1,     /* ignore case */

    /* match-flags */
    CREG_FULLMATCH = 1<<2, /* like start-, end-of-line anchors were in pattern: "^ ... $" */
    CREG_NEXT = 1<<3,      /* use end of previous match[0] as start of input */
    CREG_STARTEND = 1<<4,  /* use match[0] as start+end of input */

    /* replace-flags */
    CREG_STRIP = 1<<5,     /* only keep the matched strings, strip rest */

    /* limits */
    CREG_MAX_CLASSES = 16,
    CREG_MAX_CAPTURES = 32,
};

typedef enum {
    CREG_OK = 0,
    CREG_NOMATCH = -1,
    CREG_MATCHERROR = -2,
    CREG_OUTOFMEMORY = -3,
    CREG_UNMATCHEDLEFTPARENTHESIS = -4,
    CREG_UNMATCHEDRIGHTPARENTHESIS = -5,
    CREG_TOOMANYSUBEXPRESSIONS = -6,
    CREG_TOOMANYCHARACTERCLASSES = -7,
    CREG_MALFORMEDCHARACTERCLASS = -8,
    CREG_MISSINGOPERAND = -9,
    CREG_UNKNOWNOPERATOR = -10,
    CREG_OPERANDSTACKOVERFLOW = -11,
    CREG_OPERATORSTACKOVERFLOW = -12,
    CREG_OPERATORSTACKUNDERFLOW = -13,
} cregex_result;

typedef struct {
    struct _Reprog* prog;
    int error;
} cregex;

typedef struct {
    const cregex* re;
    const char* input;
    csview match[CREG_MAX_CAPTURES];
} cregex_iter;

#define c_formatch(it, Re, Input) \
    for (cregex_iter it = {Re, Input, {{0}}}; \
         cregex_find_4(it.re, it.input, it.match, CREG_NEXT) == CREG_OK; )

STC_INLINE cregex cregex_init(void) {
    cregex re = {0};
    return re;
}

/* return CREG_OK, or negative error code on failure. */
#define cregex_compile(...) c_MACRO_OVERLOAD(cregex_compile, __VA_ARGS__)
#define cregex_compile_2(re, pattern) cregex_compile_3(re, pattern, CREG_DEFAULT)
int cregex_compile_3(cregex *re, const char* pattern, int cflags);

/* construct and return a regex from a pattern. */
#define cregex_from(...) c_MACRO_OVERLOAD(cregex_from, __VA_ARGS__)
#define cregex_from_1(pattern) cregex_from_2(pattern, CREG_DEFAULT)

STC_INLINE cregex cregex_from_2(const char* pattern, int cflags) {
    cregex re = {0};
    cregex_compile_3(&re, pattern, cflags);
    return re;
}

/* number of capture groups in a regex pattern, excluding the full match capture (0) */
int cregex_captures(const cregex* re);

/* return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. */
#define cregex_find(...) c_MACRO_OVERLOAD(cregex_find, __VA_ARGS__)
#define cregex_find_3(re, input, match) cregex_find_4(re, input, match, CREG_DEFAULT)
int cregex_find_4(const cregex* re, const char* input, csview match[], int mflags);

/* find with csview as input. */
STC_INLINE int cregex_find_sv(const cregex* re, csview input, csview match[]) {
    csview *mp = NULL;
    if (match) { match[0] = input; mp = match; }
    return cregex_find(re, input.buf, mp, CREG_STARTEND);
}

/* match + compile RE pattern */
#define cregex_find_pattern(...) c_MACRO_OVERLOAD(cregex_find_pattern, __VA_ARGS__)
#define cregex_find_pattern_3(pattern, input, match) \
    cregex_find_pattern_4(pattern, input, match, CREG_DEFAULT)
int cregex_find_pattern_4(const char* pattern, const char* input,
                          csview match[], int cmflags);

STC_INLINE bool cregex_is_match(const cregex* re, const char* input)
    { return cregex_find_4(re, input, NULL, CREG_DEFAULT) == CREG_OK; }

/* replace csview input with replace using regular expression pattern */
#define cregex_replace_sv(...) c_MACRO_OVERLOAD(cregex_replace_sv, __VA_ARGS__)
#define cregex_replace_sv_3(pattern, input, replace) \
    cregex_replace_sv_4(pattern, input, replace, INT32_MAX)
#define cregex_replace_sv_4(pattern, input, replace, count) \
    cregex_replace_sv_6(pattern, input, replace, count, NULL, CREG_DEFAULT)
cstr cregex_replace_sv_6(const cregex* re, csview input, const char* replace, int count,
                         bool (*transform)(int group, csview match, cstr* result), int rflags);

/* replace input with replace using regular expression */
#define cregex_replace(...) c_MACRO_OVERLOAD(cregex_replace, __VA_ARGS__)
#define cregex_replace_3(re, input, replace) cregex_replace_4(re, input, replace, INT32_MAX)

STC_INLINE cstr cregex_replace_4(const cregex* re, const char* input, const char* replace, int count) {
    csview sv = {input, c_strlen(input)};
    return cregex_replace_sv_4(re, sv, replace, count);
}

/* replace + compile RE pattern, and extra arguments */
#define cregex_replace_pattern(...) c_MACRO_OVERLOAD(cregex_replace_pattern, __VA_ARGS__)
#define cregex_replace_pattern_3(pattern, input, replace) \
    cregex_replace_pattern_4(pattern, input, replace, INT32_MAX)
#define cregex_replace_pattern_4(pattern, input, replace, count) \
    cregex_replace_pattern_6(pattern, input, replace, count, NULL, CREG_DEFAULT)
cstr cregex_replace_pattern_6(const char* pattern, const char* input, const char* replace, int count,
                              bool (*transform)(int group, csview match, cstr* result), int crflags);

/* destroy regex */
void cregex_drop(cregex* re);

#endif // STC_CREGEX_H_INCLUDED

#if defined i_implement || defined i_import
  #include "priv/linkage.h"
  #include "priv/cregex_prv.c"
  #if defined i_import
     #include "priv/utf8_prv.c"
     #include "priv/cstr_prv.c"
  #endif
  #include "priv/linkage2.h"
#endif
