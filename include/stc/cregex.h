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
#ifndef CREGEX9_H_
#define CREGEX9_H_
/*
 * cregex9.h
 * 
 * This is a extended version of regexp9, supporting UTF8 input, common 
 * shorthand character classes, ++.
 */

#include <stdint.h>

typedef enum {
    creg_nomatch = -1,
    creg_matcherror = -2,
    creg_outofmemory = -3,
    creg_unmatchedleftparenthesis = -4,
    creg_unmatchedrightparenthesis = -5,
    creg_toomanysubexpressions = -6,
    creg_toomanycharacterclasses = -7,
    creg_malformedcharacterclass = -8,
    creg_missingoperand = -9,
    creg_unknownoperator = -10,
    creg_operandstackoverflow = -11,
    creg_operatorstackoverflow = -12,
    creg_operatorstackunderflow = -13,
} cregex_error_t;

enum {
    /* flags */
    creg_dotall = 1<<0,     /* compile */
    creg_caseless = 1<<1,   /* compile+match */
    creg_fullmatch = 1<<2,  /* match */
    creg_next = 1<<3,       /* match */
    creg_startend = 1<<4,   /* match */
    /* limits */
    creg_max_classes = 16,
    creg_max_captures = 32,
};

typedef struct {
    struct Reprog* prog;
} cregex;

typedef struct {
    const char* str;
    size_t len;
} cregmatch;

/* return number of capture groups on success, or (negative) error code on failure. */
int cregex_compile(cregex *rx, const char* pattern, int cflags);

static inline cregex cregex_init(void) {
    cregex rx = {NULL}; return rx;
}

static inline cregex cregex_new(const char* pattern, int cflags) {
    cregex rx;
    cregex_compile(&rx, pattern, cflags);
    return rx;
}
/* number of capture groups in the regex pattern */
int cregex_captures(cregex rx);

/* return number of capture groups on success, or (negative) error code on failure. */
int cregex_find(const cregex *rx, const char* string, 
                size_t nmatch, cregmatch match[], int mflags);

void cregex_replace(const char* src, char* dst, int dsize,
                    int nmatch, const cregmatch match[]);

void cregex_drop(cregex* preg);

#endif
