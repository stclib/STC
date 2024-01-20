/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
// c_erase_if
// --------------------------------

// Use with: list, hmap, hset, smap, sset:
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

// Use with: stack, vec, deq, queue:
#define c_eraseremove_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    intptr_t _n = 0; \
    const C##_value* value; \
    C##_iter _i, _it = C##_begin(_cnt); \
    while ((value = _it.ref) && !(pred)) \
        C##_next(&_it); \
    for (_i = _it; (value = _it.ref); C##_next(&_it)) { \
        if (pred) C##_value_drop(_it.ref), ++_n; \
        else *_i.ref = *_it.ref, C##_next(&_i); \
    } \
    C##_adjust_end_(_cnt, -_n); \
} while (0)

// ------------------------------------
// c_copy, c_copy_if, c_copy_reverse_if
// ------------------------------------

#define c_copy(...) c_MACRO_OVERLOAD(c_copy, __VA_ARGS__)
#define c_copy_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, true)

#define c_copy_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, true)

#define c_copy_reverse(...) c_MACRO_OVERLOAD(c_copy_reverse, __VA_ARGS__)
#define c_copy_reverse_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, true)

#define c_copy_reverse_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, true)


#define c_copy_if(...) c_MACRO_OVERLOAD(c_copy_if, __VA_ARGS__)
#define c_copy_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, pred)

#define c_copy_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, pred)

#define c_copy_reverse_if(...) c_MACRO_OVERLOAD(c_copy_reverse_if, __VA_ARGS__)
#define c_copy_reverse_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, pred)

#define c_copy_reverse_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, pred)

// private
#define _c_copy_if(C, cnt, rev, C_out, outcnt_ptr, pred) do { \
    C _cnt = cnt; \
    C_out *_out = outcnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##rev##begin(&_cnt); (value = _it.ref); C##rev##next(&_it)) \
        if (pred) C_out##_push(_out, C##_value_clone(*_it.ref)); \
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

#endif // STC_UTILITY_H_INCLUDED
