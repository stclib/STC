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

// Sorted set - implemented as an AA-tree (balanced binary tree).
/*
#include <stdio.h>

#define i_tag i
#define i_key int
#include <stc/csset.h> // sorted set of int

int main(void) {
    csset_i s = csset_i_init();
    csset_i_insert(&s, 5);
    csset_i_insert(&s, 8);
    csset_i_insert(&s, 3);
    csset_i_insert(&s, 5);

    c_foreach (k, csset_i, s)
        printf("set %d\n", *k.ref);
    csset_i_drop(&s);
}
*/

#ifndef _i_prefix
#define _i_prefix csset_
#endif
#define _i_isset
#include "csmap.h"
