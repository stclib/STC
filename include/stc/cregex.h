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
    CREG_MULTILINE = 1<<2, /* multi-line input, ^=bol $=eol */

    /* match-flags */
    CREG_FULLMATCH = 1<<3, /* like start-, end-of-line anchors were in pattern: "^ ... $" */
    CREG_NEXT = 1<<4,      /* use end of previous match[0] as start of input */

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

// [deprecated]:
#define c_match(...) c_each_match(__VA_ARGS__)

#define c_each_match(it, re, str) \
    cregex_iter it = {.regex=re, .input={str}, .match={{0}}}; \
    cregex_match(it.regex, it.input.buf, it.match, CREG_NEXT) == CREG_OK;

#define c_each_match_sv(it, re, strview) \
    cregex_iter it = {.regex=re, .input=strview, .match={{0}}}; \
    cregex_match_sv(it.regex, it.input, it.match, CREG_NEXT) == CREG_OK;


/* compile a regex from a pattern. return CREG_OK, or negative error code on failure. */
STC_EXTERN int cregex_compile_pro(cregex *re, const char* pattern, int cflags);

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
STC_EXTERN void cregex_drop(cregex* re);

/* number of capture groups in a regex pattern, excluding the full match capture (0) */
STC_EXTERN int cregex_captures(const cregex* re);

/* cregex_match*(): return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. aio = all-in-one, i.e. compile, match (, replace), drop */
#define cregex_match(...) _cregex_match(__VA_ARGS__,)
#define cregex_match_sv(...) _cregex_match_sv(__VA_ARGS__,)
#define cregex_match_aio(...) _cregex_match_aio(__VA_ARGS__,)
#define cregex_match_aio_sv(...) _cregex_match_aio_sv(__VA_ARGS__,)
#define cregex_is_match(re, str) (_cregex_match(re, str, 0) == CREG_OK)

/* cregex_replace*(): return a cstr where matches are replaced with a substitute string expression */
#define cregex_replace(...) _cregex_replace(__VA_ARGS__,)
#define cregex_replace_sv(...) _cregex_replace_sv(__VA_ARGS__,)
#define cregex_replace_aio(...) _cregex_replace_aio(__VA_ARGS__,)
#define cregex_replace_aio_sv(...) _cregex_replace_aio_sv(__VA_ARGS__,)

/* ----- Private ----- */

typedef struct { csview* match; int flags; } cregex_match_opt_s;
typedef struct { int count; bool(*xform)(int group, csview match, cstr* out); int flags; } cregex_replace_opt_s;

#define _cregex_match(re, str, ...) cregex_match_opt(re, str, NULL, (cregex_match_opt_s){__VA_ARGS__})
#define _cregex_match_sv(re, sv, ...) cregex_match_sv_opt(re, sv, (cregex_match_opt_s){__VA_ARGS__})
#define _cregex_match_aio(pattern, str, ...) cregex_match_aio_opt(pattern, str, NULL, (cregex_match_opt_s){__VA_ARGS__})
#define _cregex_match_aio_sv(pattern, sv, ...) cregex_match_aio_sv_opt(pattern, sv, (cregex_match_opt_s){__VA_ARGS__})
#define _cregex_replace(re, str, repl, ...) cregex_replace_opt(re, str, NULL, repl, (cregex_replace_opt_s){__VA_ARGS__})
#define _cregex_replace_sv(re, sv, repl, ...) cregex_replace_sv_opt(re, sv, repl, (cregex_replace_opt_s){__VA_ARGS__})
#define _cregex_replace_aio(pattern, str, repl, ...) cregex_replace_aio_opt(pattern, str, NULL, repl, (cregex_replace_opt_s){__VA_ARGS__})
#define _cregex_replace_aio_sv(pattern, sv, repl, ...) cregex_replace_aio_sv_opt(pattern, sv, repl, (cregex_replace_opt_s){__VA_ARGS__})

STC_EXTERN int cregex_match_opt(const cregex* re, const char* input, const char* input_end, cregex_match_opt_s opt);
STC_EXTERN int cregex_match_aio_opt(const char* pattern, const char* input, const char* input_end, cregex_match_opt_s opt);
STC_EXTERN cstr cregex_replace_opt(const cregex* re, const char* input, const char* input_end, const char* repl, cregex_replace_opt_s opt);
STC_EXTERN cstr cregex_replace_aio_opt(const char* pattern, const char* input, const char* input_end, const char* repl, cregex_replace_opt_s opt);

static inline int cregex_match_sv_opt(const cregex* re, csview sv, cregex_match_opt_s opt)
    { return cregex_match_opt(re, sv.buf, sv.buf+sv.size, opt); }
static inline int cregex_match_aio_sv_opt(const char* pattern, csview sv, cregex_match_opt_s opt)
    { return cregex_match_aio_opt(pattern, sv.buf, sv.buf+sv.size, opt); }
static inline cstr cregex_replace_sv_opt(const cregex* re, csview sv, const char* repl, cregex_replace_opt_s opt)
    { return cregex_replace_opt(re, sv.buf, sv.buf+sv.size, repl, opt); }
static inline cstr cregex_replace_aio_sv_opt(const char* pattern, csview sv, const char* repl, cregex_replace_opt_s opt)
    { return cregex_replace_aio_opt(pattern, sv.buf, sv.buf+sv.size, repl, opt); }
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
