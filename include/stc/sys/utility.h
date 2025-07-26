/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_UTILITY_H_INCLUDED
#define STC_UTILITY_H_INCLUDED

// --------------------------------
// c_find_if, c_find_reverse_if
// --------------------------------

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_begin(&cnt), NULL, _, outit_ptr, pred)

#define c_find_if_5(C, start, finish, outit_ptr, pred) \
    _c_find(C, start, (finish).ref, _, outit_ptr, pred)

#define c_find_reverse_if(...) c_MACRO_OVERLOAD(c_find_reverse_if, __VA_ARGS__)
#define c_find_reverse_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_rbegin(&cnt), NULL, _r, outit_ptr, pred)

#define c_find_reverse_if_5(C, rstart, rfinish, outit_ptr, pred) \
    _c_find(C, rstart, (rfinish).ref, _r, outit_ptr, pred)

// private
#define _c_find(C, start, endref, rev, outit_ptr, pred) do { \
    C##_iter* _out = outit_ptr; \
    const C##_value *value, *_endref = endref; \
    for (*_out = start; (value = _out->ref) != _endref; C##rev##next(_out)) \
        if (pred) goto c_JOIN(findif_, __LINE__); \
    _out->ref = NULL; c_JOIN(findif_, __LINE__):; \
} while (0)

// --------------------------------
// c_reverse
// --------------------------------

#define c_reverse_array(array, n) do { \
    typedef struct { char d[sizeof 0[array]]; } _etype; \
    _etype* _arr = (_etype *)(array); \
    for (isize _i = 0, _j = (n) - 1; _i < _j; ++_i, --_j) \
        c_swap(_arr + _i, _arr + _j); \
} while (0)

// Compiles with vec, stack, and deque, and cspan container types:
#define c_reverse(CntType, self) do { \
    CntType* _self = self; \
    for (isize _i = 0, _j = CntType##_size(_self) - 1; _i < _j; ++_i, --_j) \
        c_swap(CntType##_at_mut(_self, _i), CntType##_at_mut(_self, _j)); \
} while (0)

// --------------------------------
// c_erase_if
// --------------------------------

// Use with: list, hashmap, hashset, sortedmap, sortedset:
#define c_erase_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##_begin(_cnt); (value = _it.ref); ) { \
        if (pred) _it = C##_erase_at(_cnt, _it); \
        else C##_next(&_it); \
    } \
} while (0)

// --------------------------------
// c_eraseremove_if
// --------------------------------

// Use with: stack, vec, deque, queue:
#define c_eraseremove_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    isize _n = 0; \
    const C##_value* value; \
    C##_iter _i, _it = C##_begin(_cnt); \
    while ((value = _it.ref) && !(pred)) \
        C##_next(&_it); \
    for (_i = _it; (value = _it.ref); C##_next(&_it)) { \
        if (pred) C##_value_drop(_cnt, _it.ref), ++_n; \
        else *_i.ref = *_it.ref, C##_next(&_i); \
    } \
    C##_adjust_end_(_cnt, -_n); \
} while (0)

// --------------------------------
// c_copy_to, c_copy_if
// --------------------------------

#define c_copy_to(...) c_MACRO_OVERLOAD(c_copy_to, __VA_ARGS__)
#define c_copy_to_3(C, outcnt_ptr, cnt) \
    _c_copy_if(C, outcnt_ptr, _, C, cnt, true)

#define c_copy_to_4(C_out, outcnt_ptr, C, cnt) \
    _c_copy_if(C_out, outcnt_ptr, _, C, cnt, true)

#define c_copy_if(...) c_MACRO_OVERLOAD(c_copy_if, __VA_ARGS__)
#define c_copy_if_4(C, outcnt_ptr, cnt, pred) \
    _c_copy_if(C, outcnt_ptr, _, C, cnt, pred)

#define c_copy_if_5(C_out, outcnt_ptr, C, cnt, pred) \
    _c_copy_if(C_out, outcnt_ptr, _, C, cnt, pred)

// private
#define _c_copy_if(C_out, outcnt_ptr, rev, C, cnt, pred) do { \
    C_out *_out = outcnt_ptr; \
    C _cnt = cnt; \
    const C##_value* value; \
    for (C##_iter _it = C##rev##begin(&_cnt); (value = _it.ref); C##rev##next(&_it)) \
        if (pred) C_out##_push(_out, C_out##_value_clone(_out, *_it.ref)); \
} while (0)

// --------------------------------
// c_all_of, c_any_of, c_none_of
// --------------------------------

#define c_all_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, !(pred)); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

#define c_any_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref != NULL; \
} while (0)

#define c_none_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

// --------------------------------
// c_min, c_max, c_min_n, c_max_n
// --------------------------------
#define _c_minmax_call(fn, T, ...) \
   fn(c_make_array(T, {__VA_ARGS__}), c_sizeof((T[]){__VA_ARGS__})/c_sizeof(T))

#define c_min(...) _c_minmax_call(c_min_n, isize, __VA_ARGS__)
#define c_umin(...) _c_minmax_call(c_umin_n, size_t, __VA_ARGS__)
#define c_min32(...) _c_minmax_call(c_min32_n, int32_t, __VA_ARGS__)
#define c_fmin(...) _c_minmax_call(c_fmin_n, float, __VA_ARGS__)
#define c_dmin(...) _c_minmax_call(c_dmin_n, double, __VA_ARGS__)
#define c_max(...) _c_minmax_call(c_max_n, isize, __VA_ARGS__)
#define c_umax(...) _c_minmax_call(c_umax_n, size_t, __VA_ARGS__)
#define c_max32(...) _c_minmax_call(c_max32_n, int32_t, __VA_ARGS__)
#define c_fmax(...) _c_minmax_call(c_fmax_n, float, __VA_ARGS__)
#define c_dmax(...) _c_minmax_call(c_dmax_n, double, __VA_ARGS__)

#define _c_minmax_def(fn, T, opr) \
    static inline T fn(const T a[], isize n) { \
        T x = a[0]; \
        for (isize i = 1; i < n; ++i) if (a[i] opr x) x = a[i]; \
        return x; \
    }
_c_minmax_def(c_min32_n, int32_t, <)
_c_minmax_def(c_min_n, isize, <)
_c_minmax_def(c_umin_n, size_t, <)
_c_minmax_def(c_fmin_n, float, <)
_c_minmax_def(c_dmin_n, double, <)
_c_minmax_def(c_max32_n, int32_t, >)
_c_minmax_def(c_max_n, isize, >)
_c_minmax_def(c_umax_n, size_t, >)
_c_minmax_def(c_fmax_n, float, >)
_c_minmax_def(c_dmax_n, double, >)

#endif // STC_UTILITY_H_INCLUDED
