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
#ifndef STC_CCORO_INCLUDED
#define STC_CCORO_INCLUDED
/*
#include <stdio.h>
#include <stdbool.h>
#include <stc/algo/ccoro.h>

struct iterate {
    int max_x;
    int max_y;
    int ccoro_state; // required member
    int x;
    int y;
};

bool iterate(struct iterate* U) {
    ccoro_execute(U,
        for (U->x = 0; U->x < U->max_x; U->x++)
            for (U->y = 0; U->y < U->max_y; U->y++)
                ccoro_yield (true);

        ccoro_final: puts("final");
    );
    return false;
}

int main(void) {
    struct iterate it = {3, 3};
    int n = 0;
    while (iterate(&it))
    {
        printf("%d %d\n", it.x, it.y);
        if (++n == 20) { iterate(ccoro_stop(&it)); break; }
    }
    return 0;
}
*/
#include <stc/ccommon.h>

#define ccoro_execute(c, ...) \
     int* _state = &(c)->ccoro_state; \
     switch (*_state) { \
        case 0:; __VA_ARGS__ break; \
        default: assert(!"missing ccoro_finish: or illegal state"); \
     } \
     *_state = -2

#define ccoro_yield(ret) \
    do { \
        *_state = __LINE__; return ret; \
        case __LINE__:; \
    } while (0)

#define ccoro_yield_call(...)  c_MACRO_OVERLOAD(ccoro_yield_call, __VA_ARGS__)
#define ccoro_yield_call_2(c, subcoro) ccoro_yield_call_3(c, subcoro, )
#define ccoro_yield_call_3(c, subcoro, ret) \
    do { \
        *_state = __LINE__; \
        c_PASTE(co, __LINE__): \
        subcoro; return ret; \
        case __LINE__:; \
        if (ccoro_alive(c)) goto c_PASTE(co, __LINE__); \
    } while (0)

#define ccoro_final case -1
#define ccoro_stop(c) ((c)->ccoro_state = (c)->ccoro_state > 0 ? -1 : -2, c)
#define ccoro_alive(c) ((c)->ccoro_state >= 0)

#endif
