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

// (Non-Atomic) Reference Counted pointer abstraction.
// May take a preallocated pointer in construction, so need to store 2 pointers.
// Unlike rc and arc, which only store 1 pointer.
/*
#define T IRc2, int
#include <stc/rc2.h>

#define T IRcVec, IRc2, (c_pro_key)
#include <stc/vec.h>

int main(void) {
    int* unmanaged = c_new(int, 42);
    IRc2 rc_a = IRc2_from_ptr(unmanaged); // no allocation, but an extra pointer needed to store unmanaged
    int a = *rc_a.get;
    IRc2 rc_b = IRc2_make(77);            // allocates ref count and data in one.
    int b = *rc_b.get;

    IRcVec v = c_make(IRcVec, {1, 2, 3}); // like IRcVec_push(IRc2_make(1)), etc..
    IRcVec_push(&v, IRc2_make(4));        // push takes an IRc2 as argument.
    IRcVec_emplace(&v, 5);                // emplace: takes the IRc2's element value directly!
    IRcVec_push(&v, IRc2_clone(rc_a));    // share rc_a with the vector, ref count = 2.

    c_drop(IRc2, &rc_a, &rc_b);           // rc_a reduces ref count to 1, rc_b is destructed.
    IRcVec_drop(&v);                      // rc_a is now destructed
}
*/

#define _i_prefix rc2_
#define i_arc2
#define i_no_atomic
#include "arc.h"
