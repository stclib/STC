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
#include "common.h"
#include "types.h" // csview, cstr types
#include <string.h>

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

    /* replace-count */
    CREG_REPLACEALL = INT32_MAX,

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
    const cregex* regex;
    const char* input;
    csview match[CREG_MAX_CAPTURES];
} cregex_iter;

#define c_formatch(it, re, string) \
    for (cregex_iter it = {.regex=re, .input=string, .match={{0}}}; \
         cregex_find_with(it.regex, it.input, it.match, CREG_NEXT) == CREG_OK && it.match[0].size > 0; )


/* compile a regex from a pattern. return CREG_OK, or negative error code on failure. */
int cregex_compile_with(cregex *re, const char* pattern, int cflags);

STC_INLINE int cregex_compile(cregex* re, const char* pattern)
    { return cregex_compile_with(re, pattern, CREG_DEFAULT); }

/* construct and return a regex from a pattern. return CREG_OK, or negative error code on failure. */
STC_INLINE cregex cregex_with(const char* pattern, int cflags) {
    cregex re = {0};
    cregex_compile_with(&re, pattern, cflags);
    return re;
}
STC_INLINE cregex cregex_from(const char* pattern)
    { return cregex_with(pattern, CREG_DEFAULT); }


/* number of capture groups in a regex pattern, excluding the full match capture (0) */
int cregex_captures(const cregex* re);


/* return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. */
int cregex_find_with(const cregex* re, const char* input, csview match[], int mflags);

STC_INLINE int cregex_find_sv(const cregex* re, csview input, csview match[]) {
    if (match) match[0] = input;
    return cregex_find_with(re, input.buf, match, CREG_STARTEND);
}
int cregex_find(const cregex* re, const char* input, csview match[])
    { return cregex_find_with(re, input, match, CREG_DEFAULT); }


STC_INLINE bool cregex_is_match_with(const cregex* re, const char* input, int mflags)
    { return cregex_find_with(re, input, NULL, mflags) == CREG_OK; }

STC_INLINE bool cregex_is_match(const cregex* re, const char* input)
    { return cregex_is_match_with(re, input, CREG_DEFAULT); }


/* match + compile RE pattern */
int cregex_find_pattern_with(const char* pattern, const char* input,
                             csview match[], int cmflags);

int cregex_find_pattern(const char* pattern, const char* input, csview match[])
    { return cregex_find_pattern_with(pattern, input, match, CREG_DEFAULT); }


/* replace csview input with replace using regular expression pattern */
cstr cregex_replace_sv_with(const cregex* re, csview input, const char* replace, int count,
                         bool (*transform)(int group, csview match, cstr* result), int rflags);

/* replace input with replace using regular expression */
STC_INLINE cstr cregex_replace(const cregex* re, const char* input, const char* replace) {
    csview sv = {input, c_strlen(input)};
    return cregex_replace_sv_with(re, sv, replace, INT32_MAX, NULL, CREG_DEFAULT);
}

/* replace + compile RE pattern, and extra arguments */
cstr cregex_replace_pattern_with(const char* pattern, const char* input, const char* replace, int count,
                                 bool (*transform)(int group, csview match, cstr* result), int crflags);

STC_INLINE cstr cregex_replace_pattern(const char* pattern, const char* input, const char* replace)
    { return cregex_replace_pattern_with(pattern, input, replace, INT32_MAX, NULL, CREG_DEFAULT); }

/* destroy regex */
void cregex_drop(cregex* re);

#endif // STC_CREGEX_H_INCLUDED

#if defined STC_IMPLEMENT || defined i_implement || defined i_import
  #include "priv/linkage.h"
  #include "priv/cregex_prv.c"
  #if defined i_import
     #include "priv/utf8_prv.c"
     #include "priv/cstr_prv.c"
  #endif
  #include "priv/linkage2.h"
#endif
