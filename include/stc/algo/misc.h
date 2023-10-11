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
// c_find_if
// --------------------------------

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(it, C, cnt, pred) do { \
    intptr_t _index = 0; \
    for (it = C##_begin(&cnt); it.ref && !(pred); C##_next(&it)) \
        ++_index; \
} while (0)

#define c_find_if_5(it, C, start, end, pred) do { \
    intptr_t _index = 0; \
    const C##_value* _endref = (end).ref; \
    for (it = start; it.ref != _endref && !(pred); C##_next(&it)) \
        ++_index; \
    if (it.ref == _endref) it.ref = NULL; \
} while (0)

// --------------------------------
// c_erase_if
// --------------------------------

// Use with: clist, cmap, cset, csmap, csset:
#define c_erase_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    for (C##_iter it = C##_begin(_cnt); it.ref; ) { \
        if (pred) it = C##_erase_at(_cnt, it); \
        else C##_next(&it); \
    } \
} while (0)

// --------------------------------
// c_eraseremove_if
// --------------------------------

// Use with: cstack, cvec, cdeq, cqueue:
#define c_eraseremove_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    intptr_t _n = 0; \
    C##_iter it = C##_begin(_cnt), _i; \
    while (it.ref && !(pred)) \
        C##_next(&it); \
    for (_i = it; it.ref; C##_next(&it)) \
        if (pred) C##_value_drop(it.ref), ++_n; \
        else *_i.ref = *it.ref, C##_next(&_i); \
    C##_adjust_end_(_cnt, -_n); \
} while (0)


// --------------------------------
// c_all_of, c_any_of, c_none_of:
// --------------------------------

#define c_all_of(boolptr, it, C, cnt, pred) do { \
    C##_iter it; \
    c_find_if_4(it, C, cnt, !(pred)); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#define c_any_of(boolptr, it, C, cnt, pred) do { \
    C##_iter it; \
    c_find_if_4(it, C, cnt, pred); \
    *(boolptr) = it.ref != NULL; \
} while (0)

#define c_none_of(boolptr, it, C, cnt, pred) do { \
    C##_iter it; \
    c_find_if_4(it, C, cnt, pred); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#endif
