/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CSSET_H_INCLUDED
#define CSSET_H_INCLUDED

// Sorted set - implemented as an AA-tree (balanced binary tree).
/*
#include <stc/csset.h>
#include <stdio.h>

using_csset(i, int);        // sorted set of int

int main(void) {
    csset_i s = csset_i_init();
    csset_i_insert(&s, 5);
    csset_i_insert(&s, 8);
    csset_i_insert(&s, 3);
    csset_i_insert(&s, 5);

    c_foreach (k, csset_i, s)
        printf("set %d\n", *k.ref);
    csset_i_del(&s);
}
*/

#include "csmap.h"

#define forward_csset(X, Key) _c_aatree_types(csset_##X, csset_, Key, Key)

#define using_csset(...) c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
            using_csset_3(X, Key, c_default_compare)
#define using_csset_3(X, Key, keyCompare) \
            using_csset_5(X, Key, keyCompare, c_default_del, c_default_fromraw)
#define using_csset_4(X, Key, keyCompare, keyDel) \
            using_csset_5(X, Key, keyCompare, keyDel, c_no_clone)
#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
            using_csset_8(X, Key, keyCompare, keyDel, keyClone, c_default_toraw, Key, c_true)
#define using_csset_8(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey, defTypes) \
            _c_using_aatree(csset_##X, csset_, Key, Key, keyCompareRaw, \
                            @@, @@, @@, void, defTypes, keyDel, keyFromRaw, keyToRaw, RawKey)

#define using_csset_str() \
            _c_using_aatree_strkey(str, csset_, cstr, @@, @@, @@, void, c_true)

#define SET_ONLY_csset_(...) __VA_ARGS__
#define MAP_ONLY_csset_(...)
#define KEY_REF_csset_(vp)   (vp)

#endif
