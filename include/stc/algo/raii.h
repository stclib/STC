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
#ifndef STC_RAII_INCLUDED
#define STC_RAII_INCLUDED

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(declvar, drop) \
    for (declvar, *_i, **_ip = &_i; _ip; _ip = 0, drop)
#define c_with_3(declvar, pred, drop) \
    for (declvar, *_i, **_ip = &_i; _ip && (pred); _ip = 0, drop)

#define c_scope(...) c_MACRO_OVERLOAD(c_scope, __VA_ARGS__)
#define c_scope_2(init, drop) \
    for (int _i = (init, 1); _i; _i = 0, drop)
#define c_scope_3(init, pred, drop) \
    for (int _i = (init, 1); _i && (pred); _i = 0, drop)

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(C, a) \
    c_with_2(C a = C##_init(), C##_drop(&a))
#define c_auto_3(C, a, b) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init()), \
            (C##_drop(&b), C##_drop(&a)))
#define c_auto_4(C, a, b, c) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init()), \
            (C##_drop(&c), C##_drop(&b), C##_drop(&a)))
#define c_auto_5(C, a, b, c, d) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
            (C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a)))

#endif
