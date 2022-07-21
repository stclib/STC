/*
This is a Unix port of the Plan 9 regular expression library, by Rob Pike.
Please send comments about the packaging to Russ Cox <rsc@swtch.com>.

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
#ifndef CREGEX_H_
#define CREGEX_H_
/*
 * cregex.h
 * 
 * This is a extended version of regexp9, supporting UTF8 input, common 
 * shorthand character classes, ++.
 */
#include "forward.h" // csview 

typedef enum {
    cre_success = 1,
    cre_nomatch = 0,
    cre_matcherror = -1,
    cre_outofmemory = -2,
    cre_unmatchedleftparenthesis = -3,
    cre_unmatchedrightparenthesis = -4,
    cre_toomanysubexpressions = -5,
    cre_toomanycharacterclasses = -6,
    cre_malformedcharacterclass = -7,
    cre_missingoperand = -8,
    cre_unknownoperator = -9,
    cre_operandstackoverflow = -10,
    cre_operatorstackoverflow = -11,
    cre_operatorstackunderflow = -12,
} cregex_error_t;

enum {
    /* compile-flags */
    cre_DOTALL = 1<<0,
    cre_CASELESS = 1<<1,
    /* match-flags */
    cre_FULLMATCH = 1<<2,
    cre_NEXT = 1<<3,
    cre_STARTEND = 1<<4,
    /* limits */
    cre_MAXCLASSES = 16,
    cre_MAXCAPTURES = 32,
};

typedef struct {
    struct Reprog* prog;
} cregex;

typedef csview cregmatch;

static inline
cregex cregex_init(void) {
    cregex rx = {0};
    return rx;
}

/* return number of capture groups on success, or (negative) error code on failure. */
int cregex_compile(cregex *self, const char* pattern, int cflags);

static inline
cregex cregex_from(const char* pattern, int cflags, int* res) {
    cregex rx = {0};
    int ret = cregex_compile(&rx, pattern, cflags);
    if (res) *res = ret;
    return rx;
}

/* number of capture groups in a regex pattern */
int cregex_captures(const cregex* self);

/* return 1 on match, 0 on nomatch, and -1 on failure. */
int cregex_match(const char* input, const cregex* re,
                 csview match[], int mflags);

int cregex_match_pat(const char* input, const char* pattern,
                     csview match[], int cmflags);

/* replace regular expression */ 
cstr cregex_replace(const char* input, const cregex* re, const char* replace,
                    cstr (*mfun)(int i, csview match), unsigned count);

cstr cregex_replace_patx(const char* input, const char* pattern, 
                         const char* replace, cstr (*mfun)(int i, csview match),
                         unsigned count, int cflags);
static inline
cstr cregex_replace_pat(const char* input, const char* pattern, const char* replace)
    { return cregex_replace_patx(input, pattern, replace, NULL, 0, 0); }

/* destroy regex */
void cregex_drop(cregex* self);

#endif
