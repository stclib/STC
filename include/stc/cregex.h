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

enum {
    CREG_DEFAULT = 0,

    /* compile-flags */
    CREG_DOTALL = 1<<0,    /* dot matches newline too */
    CREG_ICASE = 1<<1,     /* ignore case */

    /* match-flags */
    CREG_FULLMATCH = 1<<2, /* like start-, end-of-line anchors were in pattern: "^ ... $" */
    CREG_NEXT = 1<<3,      /* use end of previous match[0] as start of input */

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
    const cregex* regex;
    csview input;
    csview match[CREG_MAX_CAPTURES];
} cregex_iter;

#define c_match(it, re, str) \
    cregex_iter it = {.regex=re, .input={str}, .match={{0}}}; \
    cregex_match(it.regex, it.input.buf, it.match, CREG_NEXT) == CREG_OK && it.match[0].size;

#define c_match_sv(it, re, strview) \
    cregex_iter it = {.regex=re, .input=strview, .match={{0}}}; \
    cregex_match_sv(it.regex, it.input, it.match, CREG_NEXT) == CREG_OK && it.match[0].size;

/* compile a regex from a pattern. return CREG_OK, or negative error code on failure. */
extern int cregex_compile_pro(cregex *re, const char* pattern, int cflags);

#define cregex_compile(...) \
    c_ARG_4(__VA_ARGS__, cregex_compile_pro(__VA_ARGS__), cregex_compile_pro(__VA_ARGS__, CREG_DEFAULT), _too_few_args_)

/* construct and return a regex from a pattern. return CREG_OK, or negative error code on failure. */
STC_INLINE cregex cregex_make(const char* pattern, int cflags) {
    cregex re = {0};
    cregex_compile_pro(&re, pattern, cflags);
    return re;
}
STC_INLINE cregex cregex_from(const char* pattern)
    { return cregex_make(pattern, CREG_DEFAULT); }

/* destroy regex */
extern void cregex_drop(cregex* re);

/* number of capture groups in a regex pattern, excluding the full match capture (0) */
extern int cregex_captures(const cregex* re);

/* ----- Private ----- */

struct cregex_match_opt { csview* match; int flags; int _dummy; };
struct cregex_replace_opt { int count; bool(*xform)(int group, csview match, cstr* out); int flags; int _dummy; };

extern int cregex_match_opt(const cregex* re, const char* input, const char* input_end, struct cregex_match_opt opt);
extern int cregex_match_aio_opt(const char* pattern, const char* input, const char* input_end, struct cregex_match_opt opt);
extern cstr cregex_replace_opt(const cregex* re, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt);
extern cstr cregex_replace_aio_opt(const char* pattern, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt);

static inline int cregex_match_sv_opt(const cregex* re, csview sv, struct cregex_match_opt opt)
    { return cregex_match_opt(re, sv.buf, sv.buf+sv.size, opt); }
static inline int cregex_match_aio_sv_opt(const char* pattern, csview sv, struct cregex_match_opt opt)
    { return cregex_match_aio_opt(pattern, sv.buf, sv.buf+sv.size, opt); }
static inline cstr cregex_replace_sv_opt(const cregex* re, csview sv, const char* replace, struct cregex_replace_opt opt)
    { return cregex_replace_opt(re, sv.buf, sv.buf+sv.size, replace, opt); }
static inline cstr cregex_replace_aio_sv_opt(const char* pattern, csview sv, const char* replace, struct cregex_replace_opt opt)
    { return cregex_replace_aio_opt(pattern, sv.buf, sv.buf+sv.size, replace, opt); }

/* match: return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. */
#define _cregex_match(re, str, ...) cregex_match_opt(re, str, NULL, (struct cregex_match_opt){__VA_ARGS__})
#define _cregex_match_sv(re, sv, ...) cregex_match_sv_opt(re, sv, (struct cregex_match_opt){__VA_ARGS__})
/* all-in-one: compile RE pattern + match + free */
#define _cregex_match_aio(pattern, str, ...) cregex_match_aio_opt(pattern, str, NULL, (struct cregex_match_opt){__VA_ARGS__})
#define _cregex_match_aio_sv(pattern, sv, ...) cregex_match_aio_sv_opt(pattern, sv, (struct cregex_match_opt){__VA_ARGS__})

/* replace input with a string using regular expression */
#define _cregex_replace(re, str, replace, ...) cregex_replace_opt(re, str, NULL, replace, (struct cregex_replace_opt){__VA_ARGS__})
#define _cregex_replace_sv(re, sv, replace, ...) cregex_replace_sv_opt(re, sv, replace, (struct cregex_replace_opt){__VA_ARGS__})
/* all-in-one: compile RE string pattern + match + replace + free */
#define _cregex_replace_aio(pattern, str, replace, ...) cregex_replace_aio_opt(pattern, str, NULL, replace, (struct cregex_replace_opt){__VA_ARGS__})
#define _cregex_replace_aio_sv(pattern, sv, replace, ...) cregex_replace_aio_sv_opt(pattern, sv, replace, (struct cregex_replace_opt){__VA_ARGS__})

/* ----- API functions ---- */

#define cregex_match(...) _cregex_match(__VA_ARGS__, ._dummy=0)
#define cregex_match_sv(...) _cregex_match_sv(__VA_ARGS__, ._dummy=0)
#define cregex_match_aio(...) _cregex_match_aio(__VA_ARGS__, ._dummy=0)
#define cregex_match_aio_sv(...) _cregex_match_aio_sv(__VA_ARGS__, ._dummy=0)
#define cregex_is_match(re, str) (_cregex_match(re, str, 0) == CREG_OK)

#define cregex_replace(...) _cregex_replace(__VA_ARGS__, ._dummy=0)
#define cregex_replace_sv(...) _cregex_replace_sv(__VA_ARGS__, ._dummy=0)
#define cregex_replace_aio(...) _cregex_replace_aio(__VA_ARGS__, ._dummy=0)
#define cregex_replace_aio_sv(...) _cregex_replace_aio_sv(__VA_ARGS__, ._dummy=0)

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
