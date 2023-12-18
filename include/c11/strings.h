#ifndef STC_STRINGS_H_INCLUDED
#define STC_STRINGS_H_INCLUDED

#include "../stc/cstr.h"
#include "../stc/csview.h"
#include "../stc/czview.h"

// NB! C11 and later only!

#define cstr(...) c_MACRO_OVERLOAD(cstr, __VA_ARGS__)
#define cstr_1(a) _Generic((a), \
    cstr: cstr_clone, \
    char*: cstr_from, \
    const char*: cstr_from, \
    csview: cstr_from_sv, \
    czview: cstr_from_zv)(a)
#define cstr_2(str, n) cstr_from_n(str, n)
#define cstr_3(sp, pos, n) cstr_from_pos(sp, pos, n)


#define csview(...) c_MACRO_OVERLOAD(csview, __VA_ARGS__)
#define csview_1(a) _Generic((a), \
    cstr*: cstr_sv, \
    const cstr*: cstr_sv, \
    char*: csview_from, \
    const char*: csview_from, \
    czview: czview_sv)(a)
#define csview_2(str, n) csview_from_n(str, n)
#define csview_3(sv, pos, n) csview_substr(sv, pos, n)


#define czview(a) _Generic((a), \
    cstr*: cstr_zv, \
    const cstr*: cstr_zv, \
    char*: czview_from, \
    const char*: czview_from)(a)

#endif // STC_STRINGS_H_INCLUDED
