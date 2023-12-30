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

/* A string type with short string optimization in C99 with good small-string
 * optimization (22 characters with 24 bytes string).
 */
#define i_header // external linkage by default. override with i_static.
#include "priv/linkage.h"

#ifndef STC_CSTR_H_INCLUDED
#define STC_CSTR_H_INCLUDED

#include "common.h"
#include "types.h"
#include "priv/utf8_prv.h"
#include "priv/cstr_prv.h"

#endif // STC_CSTR_H_INCLUDED

#if defined i_implement || defined i_static
  #include "priv/cstr_prv.c"
#endif // i_implement

#if defined i_import
  #include "priv/utf8_prv.c"
#endif

#include "priv/linkage2.h"
