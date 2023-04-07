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
#include <stc/algo/coroutine.h>

struct iterate {
    int max_x, max_y;
    int x, y;
    int cco_state; // required member
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
        if (++n == 20) cco_stop(&it); // signal to stop at next
    }
    return 0;
}
*/
#include <stc/ccommon.h>

enum {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_suspended(ctx) ((ctx)->cco_state > 0)
#define cco_alive(ctx) ((ctx)->cco_state != cco_state_done)

#define cco_begin(ctx) \
    int *_state = &(ctx)->cco_state; \
    switch (*_state) { \
        case 0:

#define cco_end(retval) \
        *_state = cco_state_done; break; \
        case -99: goto _cco_final_; \
    } \
    return retval

#define cco_yield(...) c_MACRO_OVERLOAD(cco_yield, __VA_ARGS__)
#define cco_yield_1(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_yield_2(corocall2, ctx2) \
    cco_yield_3(corocall2, ctx2, )

#define cco_yield_3(corocall2, ctx2, retval) \
    do { \
        *_state = __LINE__; \
        do { \
            corocall2; if (cco_suspended(ctx2)) return retval; \
            case __LINE__:; \
        } while (cco_alive(ctx2)); \
    } while (0)

#define cco_final \
    case cco_state_final: \
    _cco_final_

#define cco_return \
    goto _cco_final_

#define cco_stop(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state > 0) *_state = cco_state_final; \
    } while (0)

#define cco_reset(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state == cco_state_done) *_state = 0; \
    } while (0)

#endif
