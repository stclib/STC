/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvmap
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

// Unordered map - implemented with the robin-hood hashing scheme.
/*
#define T IMap, int, int
#include <stc/hashmap.h>
#include <stdio.h>

int main(void) {
    IMap map = c_make(IMap, {{12, 32}, {42, 54}});
    IMap_insert(&map, 5, 15);
    IMap_insert(&map, 8, 18);

    for (c_each_kv(k, v, IMap, map))
        printf(" %d -> %d\n", *k, *v);

    IMap_drop(&map);
}
*/

#define _i_prefix hmap_
#include "hmap.h"
