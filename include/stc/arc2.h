/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvmap
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

// Atomic Reference Counted pointer abstraction.
// May take a preallocated pointer in construction, so need to store 2 pointers.
// Unlike rc and arc, which only store 1 pointer.
/*
#define T IArc2, int
#include <stc/arc2.h>

#define T IArcVec, IArc2, (c_pro_key)
#include <stc/vec.h>

int main(void) {
    int* unmanaged = c_new(int, 42);
    IArc2 rc_a = IArc2_from_ptr(unmanaged); // no allocation, but extra pointer needed to store unmanaged
    int a = *rc_a.get;
    IArc2 rc_b = IArc2_make(77);            // allocates ref count and data in one.
    int b = *rc_b.get;

    IArcVec v = c_make(IArcVec, {1, 2, 3}); // like IArcVec_push(IArc2_make(1)), etc..
    IArcVec_push(&v, IArc_make(4));         // push takes an IArc as argument.
    IArcVec_emplace(&v, 5);                 // emplace: takes the IArc2's element value directly!
    IArcVec_push(&v, IArc2_clone(rc_a));    // share rc_a with the vector, ref count = 2.

    c_drop(IArc2, &rc_a, &rc_b);            // rc_a reduces ref count to 1, rc_b is destructed.
    IArcVec_drop(&v);                       // rc_a is now destructed
}
*/

#define _i_prefix arc2_
#define i_arc2
#include "arc.h"
