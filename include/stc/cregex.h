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
#include "ccommon.h"

#ifndef CREGEX_INCLUDED
#define CREGEX_INCLUDED

#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

typedef struct {
	union cregex_node *nodes;
} cregex;

typedef struct {
	size_t match_begin;
	size_t match_end;
} cregex_match_t;

#define i_type cregex_res
#define i_val cregex_match_t
#include "cstack.h"

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

/* check if a given string is valid utf8 */
STC_API bool cregex_valid_utf8(const char *s);

/* create an empty expression */
STC_INLINE cregex cregex_init(void)
    { cregex re = {NULL}; return re; }

/* compile regular expression */
STC_API cregex cregex_compile(const char *re);

/* get error type if a function failed */
STC_API cregex_error_t cregex_error(void);

/* find the first matching substring in s */
STC_API bool cregex_match(cregex re, const char *s, cregex_match_t *m);

/* get all non-overlapping matches in string s. returns NULL
 * if no matches are found. returned value must be freed */
STC_API cregex_res cregex_match_all(cregex re, const char *s);

/* get amount of capture groups inside of
 * a regular expression */
STC_API size_t cregex_captures_len(cregex re);

/* get captured slice from capture group number index */
STC_API const cregex_match_t *cregex_capture(cregex re, size_t index);

/* free regular expression */
STC_API void cregex_drop(cregex *re);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

enum {
    _rx_UTF8_ACCEPT = 0,
    _rx_UTF8_REJECT = 1
};

static const uint8_t _rx_utf8d[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
	8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
	0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
	0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
	0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
	1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
	1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
	1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

static inline uint32_t _rx_utf8_decode(uint32_t *state, uint32_t *codep,
				   const uint32_t byte)
{
	uint32_t type = _rx_utf8d[byte];
	*codep = (*state != _rx_UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6)
					 : (0xff >> type) & (byte);

	*state = _rx_utf8d[256 + (*state << 4) + type];
	return *state;
}

static bool _rx_utf8_count_codepoints(size_t *count, const uint8_t *s)
{
	uint32_t state = _rx_UTF8_ACCEPT, codepoint;
	for (*count = 0; *s; ++s)
		*count += !_rx_utf8_decode(&state, &codepoint, *s);
	return state == _rx_UTF8_ACCEPT; // NB! valid == true
}

STC_DEF bool cregex_valid_utf8(const char *s)
{
	size_t count;
	bool valid = _rx_utf8_count_codepoints(&count, (const uint8_t *)s);
	return valid;
}

static inline uint32_t _rx_utf8_peek(const char *s)
{
	uint32_t state = _rx_UTF8_ACCEPT, codepoint;
	_rx_utf8_decode(&state, &codepoint, (uint8_t)s[0]);
	return codepoint;
}

static inline uint32_t _rx_utf8_char_width(uint8_t c)
{
    uint32_t ret = ((c & 0xF0) == 0xE0);
    ret += (ret << 1);                // 3
    ret |= (c < 0x80);                // 1
    ret |= ((c & 0xE0) == 0xC0) << 1; // 2
    ret |= ((c & 0xF8) == 0xF0) << 2; // 4
    return ret;
}

static inline const char *_rx_utf8_next(const char *s)
{
	if (*s == 0)
		return NULL;

	return s + _rx_utf8_char_width((uint8_t)s[0]);
}

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
	cregex_match_t cap;
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

static bool _rx_is_match(cregex_node *node, const char *orig, const char *cur,
		     const char **next)
{
	if (node == NULL) {
		*next = cur;
		return true;
	} else {
		return ((node->generic.match)(node, orig, cur, next)) &&
		       _rx_is_match(node->generic.next, orig, *next, next);
	}
}

static bool _rx_char_is_match(cregex_node *node, const char *orig, const char *cur,
			  const char **next)
{
	if (*cur == 0) {
		return false;
	}

	*next = _rx_utf8_next(cur);
	return node->chr.chr == _rx_utf8_peek(cur);
}

static bool _rx_start_is_match(cregex_node *node, const char *orig, const char *cur,
			   const char **next)
{
	*next = cur;
	return true;
}

static bool _rx_anchor_begin_is_match(cregex_node *node, const char *orig,
				  const char *cur, const char **next)
{
	*next = cur;
	return strlen(orig) == strlen(cur);
}

static bool _rx_anchor_end_is_match(cregex_node *node, const char *orig,
				const char *cur, const char **next)
{
	*next = cur;
	return strlen(cur) == 0;
}

static bool _rx_any_is_match(cregex_node *node, const char *orig, const char *cur,
			 const char **next)
{
	if (*cur) {
		*next = _rx_utf8_next(cur);
		return true;
	}

	return false;
}

static bool _rx_quant_is_match(cregex_node *node, const char *orig, const char *cur,
			   const char **next)
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

static bool _rx_class_is_match(cregex_node *node, const char *orig, const char *cur,
			   const char **next)
{
	_rx_ClassNode *cls = (_rx_ClassNode *)node;

	if (*cur == 0)
		return false;

	const uint32_t chr = _rx_utf8_peek(cur);
	*next = _rx_utf8_next(cur);

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

static bool _rx_cap_is_match(cregex_node *node, const char *orig, const char *cur,
			 const char **next)
{
	_rx_CapNode *cap = (_rx_CapNode *)node;

	if (_rx_is_match(cap->subexp, orig, cur, next)) {
		cap->cap.match_begin = cur - orig;
		cap->cap.match_end = (*next) - orig;
		return true;
	}

	return false;
}

static bool _rx_or_is_match(cregex_node *node, const char *orig, const char *cur,
	const char **next)
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
struct {
	cregex_error_t err;
	const char *s;
	jmp_buf buf;
} _rx_CompileException;

/* set global error value to the default value */
static inline void clear_compile_exception(void)
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

	const uint32_t chr = _rx_utf8_peek(s);
	*leftover = _rx_utf8_next(s);

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
		uint32_t chr = _rx_utf8_peek(s);
		s = _rx_utf8_next(s);
		if (chr == '\\') {
			s = _rx_utf8_next(s);
		}

		if (*s == '-' && s[1] != ']') {
			s++;
			chr = _rx_utf8_peek(s);
			s = _rx_utf8_next(s);

			if (chr == '\\')
				s = _rx_utf8_next(s);
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
		const uint32_t chr = _rx_utf8_peek(s);
		size_t ret = 0;
		s = _rx_utf8_next(s);

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
		uint32_t chr = _rx_utf8_peek(s);

		if (_rx_is_digit(chr)) {
			ret *= 10;
			ret += chr - '0';
			s = _rx_utf8_next(s);
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

	uint32_t tmp = _rx_utf8_peek(re);
	size_t min = 0, max = SIZE_MAX;

	if (_rx_is_digit(tmp)) {
		min = _rx_parse_digit(re, &re);
	} else if (tmp != ',') {
		_rx_throw_compile_exception(cregex_INVALID_COMPLEX_QUANT, re);
	}

	tmp = _rx_utf8_peek(re);

	if (tmp == ',') {
		re = _rx_utf8_next(re);
		if (_rx_is_digit(_rx_utf8_peek(re)))
			max = _rx_parse_digit(re, &re);
		else
			max = SIZE_MAX;
	} else {
		max = min;
	}

	tmp = _rx_utf8_peek(re);
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

	const uint32_t chr = _rx_utf8_peek(re);
	*leftover = _rx_utf8_next(re);
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

		first = _rx_utf8_peek(re);
		re = _rx_utf8_next(re);
		if (first == '\\') {
			if (*re == 0)
				_rx_throw_compile_exception(
					cregex_INVALID_COMPLEX_CLASS, re);

			first = _rx_utf8_peek(re);
			re = _rx_utf8_next(re);
		}

		if (*re == '-' && re[1] != ']' && re[1]) {
			re++;
			last = _rx_utf8_peek(re);
			re = _rx_utf8_next(re);

			if (last == '\\') {
				if (*re == 0)
					_rx_throw_compile_exception(
						cregex_INVALID_COMPLEX_CLASS,
						re);

				last = _rx_utf8_peek(re);
				re = _rx_utf8_next(re);
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
	cur->cap.cap.match_begin = 0;
	cur->cap.cap.match_end = 0;
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

	const uint32_t chr = _rx_utf8_peek(re);
	re = _rx_utf8_next(re);
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

STC_DEF cregex cregex_compile(const char *re)
{
    cregex ret = {NULL};

	clear_compile_exception();
	if (re == NULL) {
		_rx_CompileException.err = cregex_INVALID_PARAMS;
		return ret;
	}

	if (!cregex_valid_utf8(re)) {
		_rx_CompileException.err = cregex_INVALID_UTF8;
		_rx_CompileException.s = NULL;
		return ret;
	}

	cregex_node *nodes = NULL;

	if (setjmp(_rx_CompileException.buf)) {
		// Error callback
		free(nodes);
		return ret;
	}

	const size_t compile_len = _rx_calc_compiled_len(re);
	nodes = (cregex_node *)calloc(compile_len, sizeof(cregex_node));
	_rx_compile(re, re + strlen(re), nodes);
	ret.nodes = nodes;

	return ret;
}

STC_DEF cregex_error_t cregex_error(void)
{
	return _rx_CompileException.err;
}

STC_DEF bool cregex_match(cregex re, const char *s, cregex_match_t *m)
{
	clear_compile_exception();

	if (re.nodes == NULL || s == NULL || m == NULL) {
		_rx_CompileException.err = cregex_INVALID_PARAMS;
		return false;
	}

	m->match_begin = SIZE_MAX;
	m->match_end = SIZE_MAX;

	for (const char *tmp_s = s; *tmp_s; tmp_s = _rx_utf8_next(tmp_s)) {
		const char *next = NULL;
		if (_rx_is_match(re.nodes, s, tmp_s, &next)) {
			m->match_begin = tmp_s - s;
			m->match_end = next - s;
			return true;
		}
	}

	return false;
}

STC_DEF void cregex_drop(cregex *re)
{
	free(re->nodes);
}

STC_DEF cregex_res cregex_match_all(cregex re, const char *s)
{
	cregex_res matches = cregex_res_init();
	size_t offset = 0;

	const char *s_end = s + strlen(s);
	while (s < s_end) {
		cregex_match_t tmp;
		if (cregex_match(re, s, &tmp)) {
			size_t end = tmp.match_end;
			s += end;
			tmp.match_begin += offset;
			tmp.match_end += offset;

			offset += end;
			cregex_res_push(&matches, tmp);
		} else {
			break;
		}
	}

	return matches;
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

STC_DEF size_t cregex_captures_len(cregex re)
{
	return _rx_cap_node_count(re.nodes);
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
							 index - 1 -
								 subexp_len);
			}
		}
	} else if (node->generic.match == _rx_quant_is_match) {
		const size_t subexp_len = _rx_cap_node_count(node->quant.subexp);
		if (index < subexp_len) {
			return _rx_find_capture_node(node->quant.subexp, index);
		} else {
			return _rx_find_capture_node(node->generic.next, index);
		}
	} else {
		return _rx_find_capture_node(node->generic.next, index);
	}
}

STC_DEF const cregex_match_t *cregex_capture(cregex re, size_t index)
{
	_rx_CapNode *cap = (_rx_CapNode *)_rx_find_capture_node(re.nodes, index);

	if (cap == NULL) {
		return NULL;
	}

	return &cap->cap;
}

#endif
#endif
#undef i_opt
