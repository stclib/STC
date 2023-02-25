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

bool iterate(struct iterate* U) {
    cco_begin(U);
        for (U->x = 0; U->x < U->max_x; U->x++)
            for (U->y = 0; U->y < U->max_y; U->y++)
                cco_yield (true);

        cco_final:
            puts("final");
    cco_end(U);
    return false;
}

int main(void) {
    struct iterate it = {3, 3};
    int n = 0;
    while (iterate(&it))
    {
        printf("%d %d\n", it.x, it.y);
        if (++n == 20) { iterate(cco_stop(&it)); break; }
    }
    return 0;
}
*/
#include <stc/ccommon.h>

#define cco_begin(c) \
     int* _state = &(c)->cco_state; \
     switch (*_state) { \
        case 0:

#define cco_end() \
        *_state = 0; break; \
        default: assert(!"missing cco_final: or illegal state"); \
    } *_state *= -1

#define cco_yield(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_yield_coroutine(c, corocall, retval) \
    do { \
        *_state = __LINE__; \
        c_PASTE(cco, __LINE__): corocall; return retval; \
        case __LINE__:; if (cco_alive(c)) goto c_PASTE(cco, __LINE__); \
    } while (0)

#define cco_final case -1
#define cco_alive(c) ((c)->cco_state > 0)
#define cco_stop(c) ((c)->cco_state = cco_alive(c) ? -1 : -2, c)

#endif
