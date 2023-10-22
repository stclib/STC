/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#ifndef STC_MISC_INCLUDED
#define STC_MISC_INCLUDED

// ----------------------------------
// c_auto init+drop containers (RAII) 
// ----------------------------------

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(C, a) \
    c_with_2(C a = C##_init(), C##_drop(&a))
#define c_auto_3(C, a, b) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init()), \
            (C##_drop(&b), C##_drop(&a)))
#define c_auto_4(C, a, b, c) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init()), \
            (C##_drop(&c), C##_drop(&b), C##_drop(&a)))

// --------------------------------
// stc_find_if
// --------------------------------

#define stc_find_if(...) c_MACRO_OVERLOAD(stc_find_if, __VA_ARGS__)
#define stc_find_if_4(it_p, C, cnt, pred) \
    stc_find_if_5(it_p, C, C##_begin(&cnt), C##_end(&cnt), pred)

#define stc_find_if_5(it_p, C, start, end, pred) do { \
    intptr_t index = 0; \
    const C##_value *_endref = (end).ref, *value; \
    for (*(it_p) = start; (value = (it_p)->ref) != _endref; C##_next(it_p), ++index) \
        if (pred) goto c_JOIN(findif_, __LINE__); \
    (it_p)->ref = NULL; c_JOIN(findif_, __LINE__):; \
} while (0)

// --------------------------------
// stc_erase_if
// --------------------------------

// Use with: clist, cmap, cset, csmap, csset:
#define stc_erase_if(C, cnt_p, pred) do { \
    C* _cnt = cnt_p; \
    const C##_value* value; \
    for (C##_iter _it = C##_begin(_cnt); (value = _it.ref); ) { \
        if (pred) _it = C##_erase_at(_cnt, _it); \
        else C##_next(&_it); \
    } \
} while (0)

// --------------------------------
// stc_eraseremove_if
// --------------------------------

// Use with: cstack, cvec, cdeq, cqueue:
#define stc_eraseremove_if(C, cnt_p, pred) do { \
    C* _cnt = cnt_p; \
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


// --------------------------------
// stc_all_of, stc_any_of, stc_none_of:
// --------------------------------

#define stc_all_of(boolptr, C, cnt, pred) do { \
    C##_iter it; \
    stc_find_if_4(&it, C, cnt, !(pred)); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#define stc_any_of(boolptr, C, cnt, pred) do { \
    C##_iter it; \
    stc_find_if_4(&it, C, cnt, pred); \
    *(boolptr) = it.ref != NULL; \
} while (0)

#define stc_none_of(boolptr, C, cnt, pred) do { \
    C##_iter it; \
    stc_find_if_4(&it, C, cnt, pred); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#endif
