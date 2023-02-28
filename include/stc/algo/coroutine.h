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
#ifndef STC_COROUTINE_INCLUDED
#define STC_COROUTINE_INCLUDED
/*
#include <stdio.h>
#include <stdbool.h>
#include <stc/algo/coroutine.h>

struct iterate {
    int max_x;
    int max_y;
    int cco_state; // required member
    int x;
    int y;
};

bool iterate(struct iterate* I) {
    cco_begin(I);
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield(true);

        cco_final:
            puts("final");
    cco_end(false);
}

int main(void) {
    struct iterate it = {.max_x=3, .max_y=3};
    int n = 0;
    while (iterate(&it))
    {
        printf("%d %d\n", it.x, it.y);
        // example of early stop:
        if (++n == 20) (void)cco_stop(&it); // signal to stop at next
    }
    return 0;
}
*/
#include <stc/ccommon.h>

enum {
    cco_state_final = -1,
    cco_state_expired = -2,
    cco_state_illegal = -3,
};

#define cco_begin(ctx) \
    int *_state = &(ctx)->cco_state; \
    switch (*_state) { \
        case cco_state_expired: \
        case 0:; \

#define cco_end(retval) \
        *_state = cco_state_expired; break; \
        default: assert(!"missing cco_final: or illegal state"); \
                 goto cco_finish; /* avoid unused warning */ \
    } \
    return retval

#define cco_yield(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_return \
    do { \
        *_state = cco_state_final; goto cco_finish; \
    } while (0)

#define cco_coroutine(corocall, ctx, retval) \
    do { \
        *_state = __LINE__; \
        c_PASTE(cco, __LINE__): corocall; if (cco_alive(ctx)) return retval; \
        case __LINE__: if (cco_alive(ctx)) goto c_PASTE(cco, __LINE__); \
    } while (0)

#define cco_final case cco_state_final: cco_finish
#define cco_stop(ctx) (((ctx)->cco_state = cco_alive(ctx) ? \
                       cco_state_final : cco_state_illegal), ctx)
#define cco_alive(ctx) ((ctx)->cco_state > 0)

#endif
