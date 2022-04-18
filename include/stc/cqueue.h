/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
// STC queue
/*
#include <stc/crandom.h>
#include <stdio.h>

#define i_val int
#include <stc/cqueue.h>

int main() {
    int n = 10000000;
    stc64_t rng = stc64_new(1234);
    stc64_uniform_t dist = stc64_uniform_new(0, n);

    c_auto (cqueue_int, Q)
    {
        // Push ten million random numbers onto the queue.
        for (int i=0; i<n; ++i)
            cqueue_int_push(&Q, stc64_uniform(&rng, &dist));

        // Push or pop on the queue ten million times
        printf("before: size, capacity: %d, %d\n", n, cqueue_int_size(Q), cqueue_int_capacity(Q));
        for (int i=n; i>0; --i) {
            int r = stc64_uniform(&rng, &dist);
            if (r & 1)
                ++n, cqueue_int_push(&Q, r);
            else
                --n, cqueue_int_pop(&Q);
        }
        printf("after: size, capacity: %d, %d\n", n, cqueue_int_size(Q), cqueue_int_capacity(Q));
    }
}
*/

#ifndef _i_prefix
#define _i_prefix cqueue_
#endif
#define _i_queue
#define _push_back _push
#define _emplace_back _emplace
#define _pop_front _pop

#include "cdeq.h"

#undef _push_back
#undef _emplace_back
#undef _pop_front
#undef _i_queue
