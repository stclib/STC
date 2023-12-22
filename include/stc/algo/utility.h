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
#ifndef STC_UTILITY_H_INCLUDED
#define STC_UTILITY_H_INCLUDED

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
#define c_find_if_4(C, cnt, it_ptr, pred) \
    c_find_if_5(C, C##_begin(&cnt), C##_end(&cnt), it_ptr, pred)

#define c_find_if_5(C, start, end, it_ptr, pred) do { \
    intptr_t index = 0; \
    const C##_value *_endref = (end).ref, *value; \
    for (*(it_ptr) = start; (value = (it_ptr)->ref) != _endref; C##_next(it_ptr), ++index) \
        if (pred) goto c_JOIN(findif_, __LINE__); \
    (it_ptr)->ref = NULL; c_JOIN(findif_, __LINE__):; \
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

// --------------------------------
// c_copy_if
// --------------------------------

#define c_copy_if(C, cnt, outcnt_ptr, pred) do { \
    C _cnt = cnt, *_out = outcnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##_begin(&_cnt); (value = _it.ref); C##_next(&_it)) \
        if (pred) C##_push(_out, C##_value_clone(*_it.ref)); \
} while (0)

// --------------------------------
// c_all_of, c_any_of, c_none_of:
// --------------------------------

#define c_all_of(C, cnt, boolptr, pred) do { \
    C##_iter it; \
    c_find_if_4(C, cnt, &it, !(pred)); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#define c_any_of(C, cnt, boolptr, pred) do { \
    C##_iter it; \
    c_find_if_4(C, cnt, &it, pred); \
    *(boolptr) = it.ref != NULL; \
} while (0)

#define c_none_of(C, cnt, boolptr, pred) do { \
    C##_iter it; \
    c_find_if_4(C, cnt, &it, pred); \
    *(boolptr) = it.ref == NULL; \
} while (0)

#endif // STC_UTILITY_H_INCLUDED
