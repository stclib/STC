/* 
 * Copyright (c) 2020 Fabian van Rissenbeck
 *                    https://github.com/deinernstjetzt/mregexp
 *               2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 *                    http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
 *               2022 Tyge Løvset
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef CREGEX_INCLUDED
#define CREGEX_INCLUDED

#include "csview.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

typedef struct {
    size_t start;
    size_t end;
} cregex_match;

typedef struct {
    union cregex_node *nodes;
    const char* input;
    cregex_match match;
} cregex;

typedef enum {
    cregex_OK = 0,
    cregex_FAILED_ALLOC,
    cregex_INVALID_UTF8,
    cregex_INVALID_PARAMS,
    cregex_EARLY_QUANTIFIER,
    cregex_INVALID_COMPLEX_QUANT,
    cregex_UNEXPECTED_EOL,
    cregex_INVALID_COMPLEX_CLASS,
    cregex_UNCLOSED_SUBEXPRESSION,
} cregex_error_t;

/* create an empty expression */
STC_INLINE cregex cregex_init(void)
    { cregex rx = {NULL}; return rx; }

STC_INLINE bool cregex_valid(cregex rx)
    { return rx.nodes != NULL; }

/* create and compile a regular expression */
STC_API cregex cregex_new(const char *re);

/* compile regular expression: reuse previous regex only */
STC_API bool cregex_compile(cregex* rx, const char *re);

/* get error type if a function failed */
STC_API cregex_error_t cregex_error(void);

/* check if input s matches re */
STC_API bool cregex_matches(cregex *rx, const char *s);

/* find the next matching substring in s */
STC_API bool cregex_find_next(cregex *rx, const char *s, cregex_match *m);

/* find the next matching substring in s, return as csview */
STC_API bool cregex_find_next_sv(cregex *rx, const char *s, csview *sv);

/* find the first matching substring in s */
STC_INLINE bool cregex_find(cregex *rx, const char *s, cregex_match *m) {
    m->start = m->end = 0;
    return cregex_find_next(rx, s, m);
}

STC_INLINE bool cregex_find_sv(cregex *rx, const char *s, csview *sv) {
    sv->str = s, sv->size = 0;
    return cregex_find_next_sv(rx, s, sv);
}

/* get captured slice from capture group number index */
STC_API bool cregex_capture(cregex *rx, size_t index, cregex_match *m);

/* get captured slice from capture group number index as a csview */
STC_INLINE bool cregex_capture_sv(cregex *rx, size_t index, csview *sv) {
    cregex_match m;
    bool ret = cregex_capture(rx, index, &m);
    *sv = c_make(csview){rx->input + m.start, m.end - m.start};
    return ret;
}

/* get amount of capture groups inside of
 * the regular expression */
STC_API size_t cregex_capture_size(cregex rx);

/* free regular expression */
STC_API void cregex_drop(cregex *rx);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)


/* function pointer type used to evaluate if a regex node
 * matched a given string */
typedef bool (*_rx_MatchFunc)(union cregex_node *node, const char *orig,
                              const char *cur, const char **next);

typedef struct _rx_GenericNode {
    union cregex_node *prev;
    union cregex_node *next;
    _rx_MatchFunc match;
} _rx_GenericNode;

typedef struct {
    _rx_GenericNode generic;
    uint32_t chr;
} _rx_CharNode;

typedef struct {
    _rx_GenericNode generic;
    union cregex_node *subexp;
    size_t min, max;
} _rx_QuantNode;

typedef struct {
    _rx_GenericNode generic;
    uint32_t first, last;
} _rx_RangeNode;

typedef struct {
    _rx_GenericNode generic;
    _rx_RangeNode *ranges;
    bool negate;
} _rx_ClassNode;

typedef struct {
    _rx_GenericNode generic;
    union cregex_node *subexp;
    cregex_match cap;
} _rx_CapNode;

typedef struct {
    _rx_GenericNode generic;
    union cregex_node *left;
    union cregex_node *right;
} _rx_OrNode;

typedef union cregex_node {
    _rx_GenericNode generic;
    _rx_CharNode chr;
    _rx_QuantNode quant;
    _rx_ClassNode cls;
    _rx_RangeNode range;
    _rx_CapNode cap;
    _rx_OrNode ior;
} cregex_node;

static bool _rx_is_match(cregex_node *node, const char *orig,
                         const char *cur, const char **next)
{
    if (node == NULL) {
        *next = cur;
        return true;
    } else {
        return ((node->generic.match)(node, orig, cur, next)) &&
               _rx_is_match(node->generic.next, orig, *next, next);
    }
}

static bool _rx_char_is_match(cregex_node *node, const char *orig,
                              const char *cur, const char **next)
{
    if (*cur == 0) {
        return false;
    }

    *next = utf8_next(cur);
    return node->chr.chr == utf8_peek(cur);
}

static bool _rx_start_is_match(cregex_node *node, const char *orig,
                               const char *cur, const char **next)
{
    *next = cur;
    return true;
}

static bool _rx_anchor_begin_is_match(cregex_node *node, const char *orig,
                                      const char *cur, const char **next)
{
    *next = cur;
    return orig == cur;
}

static bool _rx_anchor_end_is_match(cregex_node *node, const char *orig,
                                    const char *cur, const char **next)
{
    *next = cur;
    return *cur == 0;
}

static bool _rx_any_is_match(cregex_node *node, const char *orig,
                             const char *cur, const char **next)
{
    if (*cur) {
        *next = utf8_next(cur);
        return true;
    }

    return false;
}



static bool _rx_quant_is_match(cregex_node *node, const char *orig,
                               const char *cur, const char **next)
{
    _rx_QuantNode *quant = (_rx_QuantNode *)node;
    size_t matches = 0;

    while (_rx_is_match(quant->subexp, orig, cur, next)) {
        matches++;
        cur = *next;

        if (matches >= quant->max)
            break;
    }

    *next = cur;
    return matches >= quant->min;
}

static bool _rx_class_is_match(cregex_node *node, const char *orig,
                               const char *cur, const char **next)
{
    _rx_ClassNode *cls = (_rx_ClassNode *)node;

    if (*cur == 0)
        return false;

    const uint32_t chr = utf8_peek(cur);
    *next = utf8_next(cur);

    bool found = false;
    for (_rx_RangeNode *range = cls->ranges; range != NULL;
         range = (_rx_RangeNode *)range->generic.next) {
        if (chr >= range->first && chr <= range->last) {
            found = true;
            break;
        }
    }

    if (cls->negate)
        found = !found;

    return found;
}
#include <stdio.h>
static bool _rx_cap_is_match(cregex_node *node, const char *orig,
                             const char *cur, const char **next)
{
    _rx_CapNode *cap = (_rx_CapNode *)node;

    if (_rx_is_match(cap->subexp, orig, cur, next)) {
        cap->cap.start = cur - orig;
        cap->cap.end = (*next) - orig;
        //printf("Cap: %.*s\n", cap->cap.end - cap->cap.start, orig + cap->cap.start);        
        return true;
    }

    return false;
}

static bool _rx_or_is_match(cregex_node *node, const char *orig,
                            const char *cur, const char **next)
{
    _rx_OrNode *ior = (_rx_OrNode *)node;

    if (ior->generic.next != NULL) {
        ior->right = ior->generic.next;
        ior->generic.next = NULL;
    }

    if (_rx_is_match(ior->left, orig, cur, next) && ior->left != NULL) {
        return true;
    }

    return _rx_is_match(ior->right, orig, cur, next) && ior->right != NULL;
}

/* Global error value with callback address */
static struct {
    cregex_error_t err;
    const char *s;
    jmp_buf buf;
} _rx_CompileException;

/* set global error value to the default value */
static inline void _rx_clear_compile_exception(void)
{
    _rx_CompileException.err = cregex_OK;
    _rx_CompileException.s = NULL;
}

/* set global error value and jump back to the exception handler */
static void _rx_throw_compile_exception(cregex_error_t err, const char *s)
{
    _rx_CompileException.err = err;
    _rx_CompileException.s = s;
    longjmp(_rx_CompileException.buf, 1);
}

static size_t _rx_calc_compiled_escaped_len(const char *s, const char **leftover)
{
    if (*s == 0)
        _rx_throw_compile_exception(cregex_UNEXPECTED_EOL, s);

    const uint32_t chr = utf8_peek(s);
    *leftover = utf8_next(s);

    switch (chr) {
    case 's':
        return 5;

    case 'S':
        return 5;

    case 'd':
        return 2;

    case 'D':
        return 2;

    case 'w':
        return 5;

    case 'W':
        return 5;

    default:
        return 1;
    }
}

static const size_t _rx_calc_compiled_class_len(const char *s,
                                                const char **leftover)
{
    if (*s == '^')
        s++;

    size_t ret = 1;

    while (*s && *s != ']') {
        uint32_t chr = utf8_peek(s);
        s = utf8_next(s);
        if (chr == '\\') {
            s = utf8_next(s);
        }

        if (*s == '-' && s[1] != ']') {
            s++;
            chr = utf8_peek(s);
            s = utf8_next(s);

            if (chr == '\\')
                s = utf8_next(s);
        }

        ret++;
    }

    if (*s == ']') {
        s++;
        *leftover = s;
    } else {
        _rx_throw_compile_exception(cregex_INVALID_COMPLEX_CLASS, s);
    }

    return ret;
}

/* get required amount of memory in amount of nodes
 * to _rx_compile regular expressions */
static const size_t _rx_calc_compiled_len(const char *s)
{
    if (*s == 0) {
        return 1;
    } else {
        const uint32_t chr = utf8_peek(s);
        size_t ret = 0;
        s = utf8_next(s);

        switch (chr) {
        case '{': {
            const char *end = strstr(s, "}");

            if (end == NULL)
                _rx_throw_compile_exception(
                    cregex_INVALID_COMPLEX_QUANT, s);

            s = end + 1;
            ret = 1;
            break;
        }

        case '\\':
            ret = _rx_calc_compiled_escaped_len(s, &s);
            break;

        case '[':
            ret = _rx_calc_compiled_class_len(s, &s);
            break;

        default:
            ret = 1;
            break;
        }

        return ret + _rx_calc_compiled_len(s);
    }
}

static void _rx_append_quant(cregex_node **prev, cregex_node *cur, size_t min,
                             size_t max, const char *re)
{
    cur->generic.match = _rx_quant_is_match;
    cur->generic.next = NULL;
    cur->generic.prev = NULL;

    cur->quant.max = max;
    cur->quant.min = min;
    cur->quant.subexp = *prev;

    *prev = (*prev)->generic.prev;
    if (*prev == NULL)
        _rx_throw_compile_exception(cregex_EARLY_QUANTIFIER, re);

    cur->quant.subexp->generic.next = NULL;
    cur->quant.subexp->generic.prev = NULL;
}

static inline bool _rx_is_digit(uint32_t c)
{
    return c >= '0' && c <= '9';
}

static size_t _rx_parse_digit(const char *s, const char **leftover)
{
    size_t ret = 0;

    while (*s) {
        uint32_t chr = utf8_peek(s);

        if (_rx_is_digit(chr)) {
            ret *= 10;
            ret += chr - '0';
            s = utf8_next(s);
        } else {
            break;
        }
    }

    *leftover = s;
    return ret;
}

/* parse complex quantifier of format {m,n} 
 * valid formats: {,} {m,} {,n} {m} {m,n} */
static void _rx_parse_complex_quant(const char *re, const char **leftover,
                                    size_t *min_p, size_t *max_p)
{
    if (*re == 0)
        _rx_throw_compile_exception(cregex_INVALID_COMPLEX_QUANT, re);

    uint32_t tmp = utf8_peek(re);
    size_t min = 0, max = SIZE_MAX;

    if (_rx_is_digit(tmp)) {
        min = _rx_parse_digit(re, &re);
    } else if (tmp != ',') {
        _rx_throw_compile_exception(cregex_INVALID_COMPLEX_QUANT, re);
    }

    tmp = utf8_peek(re);

    if (tmp == ',') {
        re = utf8_next(re);
        if (_rx_is_digit(utf8_peek(re)))
            max = _rx_parse_digit(re, &re);
        else
            max = SIZE_MAX;
    } else {
        max = min;
    }

    tmp = utf8_peek(re);
    if (tmp == '}') {
        *leftover = re + 1;
        *min_p = min;
        *max_p = max;
    } else {
        _rx_throw_compile_exception(cregex_INVALID_COMPLEX_QUANT, re);
    }
}

/* append character class to linked list of nodes with
 * ranges given as optional arguments. Returns pointer
 * to next */
static cregex_node *_rx_append_class(cregex_node *cur, bool negate, size_t n, ...)
{
    cur->cls.negate = negate;
    cur->cls.ranges = (_rx_RangeNode *)(n ? cur + 1 : NULL);
    cur->generic.match = _rx_class_is_match;
    cur->generic.next = NULL;
    cur->generic.prev = NULL;

    va_list ap;
    va_start(ap, n);
    cregex_node *prev = NULL;
    cur = cur + 1;

    for (size_t i = 0; i < n; ++i) {
        const uint32_t first = va_arg(ap, uint32_t);
        const uint32_t last = va_arg(ap, uint32_t);

        cur->generic.next = NULL;
        cur->generic.prev = prev;

        if (prev)
            prev->generic.next = cur;

        cur->range.first = first;
        cur->range.last = last;

        prev = cur;
        cur = cur + 1;
    }

    va_end(ap);

    return cur;
}

/** _rx_compile escaped characters. return pointer to the next free node. */
static cregex_node *_rx_compile_next_escaped(const char *re, const char **leftover,
                                             cregex_node *cur)
{
    if (*re == 0)
        _rx_throw_compile_exception(cregex_UNEXPECTED_EOL, re);

    const uint32_t chr = utf8_peek(re);
    *leftover = utf8_next(re);
    cregex_node *ret = cur + 1;

    switch (chr) {
    case 'n':
        cur->chr.chr = '\n';
        cur->generic.match = _rx_char_is_match;
        break;

    case 't':
        cur->chr.chr = '\t';
        cur->generic.match = _rx_char_is_match;
        break;

    case 'r':
        cur->chr.chr = '\r';
        cur->generic.match = _rx_char_is_match;
        break;

    case 's':
        ret = _rx_append_class(cur, false, 4, ' ', ' ', '\t', '\t', '\r',
                   '\r', '\n', '\n');
        break;

    case 'S':
        ret = _rx_append_class(cur, true, 4, ' ', ' ', '\t', '\t', '\r',
                   '\r', '\n', '\n');
        break;

    case 'w':
        ret = _rx_append_class(cur, false, 4, 'a', 'z', 'A', 'Z', '0', '9',
                   '_', '_');
        break;

    case 'W':
        ret = _rx_append_class(cur, true, 4, 'a', 'z', 'A', 'Z', '0', '9',
                   '_', '_');
        break;

    case 'd':
        ret = _rx_append_class(cur, false, 1, '0', '9');
        break;

    case 'D':
        ret = _rx_append_class(cur, true, 1, '0', '9');
        break;

    default:
        cur->chr.chr = chr;
        cur->generic.match = _rx_char_is_match;
        break;
    }

    return ret;
}

static cregex_node *_rx_compile_next_complex_class(const char *re,
                                                   const char **leftover,
                                                   cregex_node *cur)
{
    cur->generic.match = _rx_class_is_match;
    cur->generic.next = NULL;
    cur->generic.prev = NULL;

    if (*re == '^') {
        re++;
        cur->cls.negate = true;
    } else {
        cur->cls.negate = false;
    }

    cur->cls.ranges = NULL;

    cur = cur + 1;
    cregex_node *prev = NULL;

    while (*re && *re != ']') {
        uint32_t first = 0, last = 0;

        first = utf8_peek(re);
        re = utf8_next(re);
        if (first == '\\') {
            if (*re == 0)
                _rx_throw_compile_exception(
                    cregex_INVALID_COMPLEX_CLASS, re);

            first = utf8_peek(re);
            re = utf8_next(re);
        }

        if (*re == '-' && re[1] != ']' && re[1]) {
            re++;
            last = utf8_peek(re);
            re = utf8_next(re);

            if (last == '\\') {
                if (*re == 0)
                    _rx_throw_compile_exception(
                        cregex_INVALID_COMPLEX_CLASS,
                        re);

                last = utf8_peek(re);
                re = utf8_next(re);
            }
        } else {
            last = first;
        }

        cur->range.first = first;
        cur->range.last = last;
        cur->generic.prev = prev;
        cur->generic.next = NULL;

        if (prev == NULL) {
            (cur - 1)->cls.ranges = (_rx_RangeNode *)cur;
        } else {
            prev->generic.next = cur;
        }

        prev = cur;
        cur++;
    }

    if (*re == ']') {
        *leftover = re + 1;
        return cur;
    } else {
        _rx_throw_compile_exception(cregex_INVALID_COMPLEX_CLASS, re);
        return NULL; // Unreachable
    }
}

static const char *_rx_find_closing_par(const char *s)
{
    size_t level = 1;

    for (; *s && level != 0; ++s) {
        if (*s == '\\')
            s++;
        else if (*s == '(')
            level++;
        else if (*s == ')')
            level--;
    }

    if (level == 0)
        return s;
    else
        return NULL;
}

static cregex_node *_rx_compile(const char *re, const char *end, cregex_node *nodes);

static cregex_node *_rx_compile_next_cap(const char *re, const char **leftover,
                                         cregex_node *cur)
{
    cur->cap.cap.start = 0;
    cur->cap.cap.end = 0;
    cur->cap.subexp = cur + 1;
    cur->generic.next = NULL;
    cur->generic.prev = NULL;
    cur->generic.match = _rx_cap_is_match;

    const char *end = _rx_find_closing_par(re);

    if (end == NULL)
        _rx_throw_compile_exception(cregex_UNCLOSED_SUBEXPRESSION, re);

    *leftover = end;
    return _rx_compile(re, end - 1, cur + 1);
}

static cregex_node *insert_or(cregex_node *cur, cregex_node **prev) {
    cur->generic.match = _rx_or_is_match;
    cur->generic.next = NULL;
    cur->generic.prev = NULL;

    // Find last start node
    cregex_node *begin = *prev;

    while (begin->generic.match != _rx_start_is_match) {
        begin = begin->generic.prev;
    }

    cur->ior.left = begin->generic.next;
    *prev = begin;

    return cur + 1;
}

/* _rx_compile next node. returns address of next available node.
 * returns NULL if re is empty */
static cregex_node *_rx_compile_next(const char *re, const char **leftover,
                                     cregex_node *prev, cregex_node *cur)
{
    if (*re == 0)
        return NULL;

    const uint32_t chr = utf8_peek(re);
    re = utf8_next(re);
    cregex_node *next = cur + 1;

    switch (chr) {
    case '^':
        cur->generic.match = _rx_anchor_begin_is_match;
        break;

    case '$':
        cur->generic.match = _rx_anchor_end_is_match;
        break;

    case '.':
        cur->generic.match = _rx_any_is_match;
        break;

    case '*':
        _rx_append_quant(&prev, cur, 0, SIZE_MAX, re);
        break;

    case '+':
        _rx_append_quant(&prev, cur, 1, SIZE_MAX, re);
        break;

    case '?':
        _rx_append_quant(&prev, cur, 0, 1, re);
        break;

    case '{': {
        size_t min = 0, max = SIZE_MAX;
        const char *leftover = NULL;
        _rx_parse_complex_quant(re, &leftover, &min, &max);

        _rx_append_quant(&prev, cur, min, max, re);
        re = leftover;
        break;
    }

    case '[':
        next = _rx_compile_next_complex_class(re, &re, cur);
        break;

    case '(':
        next = _rx_compile_next_cap(re, &re, cur);
        break;

    case '\\':
        next = _rx_compile_next_escaped(re, &re, cur);
        break;

    case '|':
        next = insert_or(cur, &prev);
        break;

    default:
        cur->chr.chr = chr;
        cur->generic.match = _rx_char_is_match;
        break;
    }

    cur->generic.next = NULL;
    cur->generic.prev = prev;
    prev->generic.next = cur;
    *leftover = re;

    return next;
}

/* _rx_compile raw regular expression into a linked list of nodes. return leftover nodes */
static cregex_node *_rx_compile(const char *re, const char *end, cregex_node *nodes)
{
    cregex_node *prev = nodes;
    cregex_node *cur = nodes + 1;

    prev->generic.next = NULL;
    prev->generic.prev = NULL;
    prev->generic.match = _rx_start_is_match;

    while (cur != NULL && re != NULL && re < end) {
        const char *next = NULL;
        cregex_node *next_node = _rx_compile_next(re, &next, prev, cur);

        prev = cur;
        cur = next_node;
        re = next;
    }

    return cur;
}

STC_DEF cregex cregex_new(const char *re)
{
    cregex rx = {NULL};
    cregex_compile(&rx, re);
    return rx;
}

STC_DEF bool cregex_compile(cregex* rx, const char *re)
{
    _rx_clear_compile_exception();

    if (!utf8_valid(re)) {
        _rx_CompileException.err = cregex_INVALID_UTF8;
        _rx_CompileException.s = NULL;
        return false;
    }

    if (setjmp(_rx_CompileException.buf)) {
        // Error callback
        c_free(rx->nodes);
        rx->nodes = NULL;
        return false;
    }

    const size_t compile_len = _rx_calc_compiled_len(re);
    rx->nodes = (cregex_node *)c_realloc(rx->nodes, compile_len * sizeof(cregex_node));
    _rx_compile(re, re + strlen(re), rx->nodes);
    return true;
}

STC_DEF cregex_error_t cregex_error(void)
{
    return _rx_CompileException.err;
}

STC_DEF bool cregex_matches(cregex* rx, const char *s)
{
    const char* next;
    bool res = _rx_is_match(rx->nodes, s, s, &next);
    if (res && *next == 0) { 
        rx->match.start = 0;
        rx->match.end = next - s;
        rx->input = s;
        return true;
    }
    rx->input = NULL;
    return false;
}

STC_DEF bool cregex_find_next(cregex *rx, const char *s, cregex_match *m)
{
    const char *it = s + m->end, *next;

    for (; *it; it = utf8_next(it)) {
        if (_rx_is_match(rx->nodes, s, it, &next)) {
            m->start = it - s;
            m->end = next - s;
            rx->match = *m;
            rx->input = s;
            return true;
        }
    }
    rx->input = NULL;
    return false;
}

STC_API bool cregex_find_next_sv(cregex *rx, const char *s, csview *sv)
{
    if (!sv->str) sv->str = s;
    cregex_match m = {(size_t)(sv->str - s), m.start + sv->size};
    
    bool res = cregex_find_next(rx, s, &m);
    if (res) *sv = c_make(csview){s + m.start, m.end - m.start};
    return res;
}


STC_DEF void cregex_drop(cregex *rx)
{
    c_free(rx->nodes);
}

/* calculate amount of capture groups
 * inside a regular expression */
static size_t _rx_cap_node_count(cregex_node *nodes)
{
    if (nodes == NULL) {
        return 0;
    } else if (nodes->generic.match == _rx_quant_is_match) {
        return _rx_cap_node_count(nodes->quant.subexp) +
               _rx_cap_node_count(nodes->generic.next);
    } else if (nodes->generic.match == _rx_cap_is_match) {
        return _rx_cap_node_count(nodes->quant.subexp) +
               _rx_cap_node_count(nodes->generic.next) + 1;
    } else {
        return _rx_cap_node_count(nodes->generic.next);
    }
}

STC_DEF size_t cregex_capture_size(cregex rx)
{
    return _rx_cap_node_count(rx.nodes) + (rx.input != NULL);
}

static cregex_node *_rx_find_capture_node(cregex_node *node, size_t index)
{
    if (node == NULL) {
        return NULL;
    } else if (node->generic.match == _rx_cap_is_match) {
        if (index == 0) {
            return node;
        } else {
            const size_t subexp_len =
                _rx_cap_node_count(node->cap.subexp);
            if (index <= subexp_len) {
                return _rx_find_capture_node(node->cap.subexp,
                             index - subexp_len);
            } else {
                return _rx_find_capture_node(node->generic.next,
                             index - subexp_len - 1);
            }
        }
    } else if (node->generic.match == _rx_quant_is_match) {
        const size_t subexp_len = _rx_cap_node_count(node->quant.subexp);
        if (index < subexp_len) {
            return _rx_find_capture_node(node->quant.subexp, index);
        } else {
            return _rx_find_capture_node(node->generic.next, index - 1); // FIX by Tyge, added: - 1
        }
    } else {
        return _rx_find_capture_node(node->generic.next, index);
    }
}

STC_DEF bool cregex_capture(cregex *rx, size_t index, cregex_match *m)
{
    if (index == 0) { *m = rx->match; return m->end != 0; }

    _rx_CapNode *cap = (_rx_CapNode *)_rx_find_capture_node(rx->nodes, index - 1);
    if (cap) *m = cap->cap;
    return cap != NULL;
}

#endif
#endif
#undef i_opt
