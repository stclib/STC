/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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
/*
#include <stdio.h>
#include "cbitvec.h"

int main() {
    CBitVec vec = cbitvec_make(23, true);
    cbitvec_unset(&vec, 9);
    cbitvec_resize(&vec, 43, false);
    printf("%4zu: ", vec.size);for (int i=0; i<vec.size; ++i) printf("%d", cbitvec_value(&vec, i));puts("");
    cbitvec_set(&vec, 28);
    cbitvec_resize(&vec, 77, true);
    cbitvec_resize(&vec, 93, false);
    cbitvec_resize(&vec, 102, true);
    cbitvec_setValue(&vec, 99, false);
    printf("%4zu: ", vec.size);for (int i=0; i<vec.size; ++i) printf("%d", cbitvec_value(&vec, i));puts("");
    cbitvec_destroy(&vec);
}
*/
#ifndef CBITVEC__H__
#define CBITVEC__H__

#include <string.h>
#include <stdlib.h>
#include "cdefs.h"

typedef struct { uint32_t* _arr; size_t size; } CBitVec;

STC_INLINE void cbitvec_set(CBitVec *self, size_t i) {self->_arr[i >> 5] |= 1u << (i & 31);}
STC_INLINE void cbitvec_unset(CBitVec *self, size_t i) {self->_arr[i >> 5] &= ~(1u << (i & 31));}
STC_INLINE void cbitvec_setValue(CBitVec *self, size_t i, bool value) {value ? cbitvec_set(self, i) : cbitvec_unset(self, i);}
STC_INLINE void cbitvec_flip(CBitVec *self, size_t i) {self->_arr[i >> 5] ^= 1u << (i & 31);}
STC_INLINE bool cbitvec_value(CBitVec *self, size_t i) {return (self->_arr[i >> 5] & (1u << (i & 31))) != 0;}

STC_INLINE void cbitvec_resize(CBitVec* self, size_t size, bool value) {
    size_t newsz = (size + 31) >> 5, oldsz = (self->size + 31) >> 5;
    self->_arr = (uint32_t *) realloc(self->_arr, newsz * 4);
    memset(self->_arr + oldsz, value ? 0xff : 0x0, (newsz - oldsz) * 4);
    if (self->size & 31) {
		size_t idx = (self->size - 1) >> 5; uint32_t bits = (1u << (self->size & 31)) - 1;
		value ? (self->_arr[idx] |= ~bits) : (self->_arr[idx] &= bits);
	}
    self->size = size;
}

STC_INLINE void cbitvec_setAll(CBitVec *self, bool value) {
	memset(self->_arr, value ? 0xff : 0x0, ((self->size + 31) >> 5) * 4);
}

STC_INLINE void cbitvec_flipAll(CBitVec *self) {
    size_t n = (self->size + 31) >> 5;
    while (n--) self->_arr[n] ^= 0xffffffff;
}

STC_INLINE CBitVec cbitvec_make(size_t size, bool value) {
    CBitVec vec = {(uint32_t *) malloc(((size + 31) >> 5) * 4), size};
    cbitvec_setAll(&vec, value);
    return vec;
}

STC_INLINE void cbitvec_destroy(CBitVec* self) {
    free(self->_arr);
}

#endif
