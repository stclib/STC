#ifndef STC_STRINGS_H_INCLUDED
#define STC_STRINGS_H_INCLUDED

#include "../stc/cstr.h"
#include "../stc/csview.h"
#include "../stc/czview.h"

// NB! C11 and later only!

#define cstr_make(...) c_MACRO_OVERLOAD(cstr_make, __VA_ARGS__)
#define cstr_make_1(a) _Generic((a), \
    char*: cstr_from, \
    const char*: cstr_from, \
    csview: cstr_from_sv, \
    czview: cstr_from_zv)(a)
#define cstr_make_2(str, n) \
    cstr_from_n(str, n)
#define cstr_make_3(s, pos, n) \
    cstr_from_pos(s, pos, n)

#define csview_make(...) c_MACRO_OVERLOAD(csview_make, __VA_ARGS__)
#define csview_make_1(a) _Generic((a), \
    cstr*: cstr_sv, \
    const cstr*: cstr_sv, \
    char*: csview_from, \
    const char*: csview_from, \
    czview: czview_sv)(a)
#define csview_make_2(str, n) \
    csview_from_n(str, n)
#define csview_make_3(a, pos, n) _Generic((a), \
    csview: csview_substr, \
    cstr*: cstr_substr, \
    const cstr*: cstr_substr)(a, pos, n)

#define czview_make(a) _Generic((a), \
    cstr*: cstr_zv, \
    const cstr*: cstr_zv, \
    char*: czview_from, \
    const char*: czview_from)(a)

#endif // STC_STRINGS_H_INCLUDED
