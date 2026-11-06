/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
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
#ifndef CSTREAM_H_INCLUDED
#define CSTREAM_H_INCLUDED
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"

typedef struct {
    FILE* fp; char* data;
    isize_t size, capacity;
} cstream;

void    cstream_printf(cstream* self, const char* fmt, ...);
void    cstream_drop(cstream* self);
#define cstream_close(self) cstream_drop(self) // alias
#endif // CSTREAM_H_INCLUDED

#ifdef STC_IMPLEMENT_CSTREAM // ----------------------------------

void cstream_printf(cstream* self, const char* fmt, ...) {
    va_list args, args2;
    va_start(args, fmt);

    if (self->fp)
        vfprintf(self->fp, fmt, args);
    else {
        va_copy(args2, args);
        const int n = vsnprintf(NULL, 0U, fmt, args);
        if (n < 0) goto done2;
        const isize_t pos = self->size;
        self->size += n;
        if (self->size > self->capacity) {
            const isize_t cap = self->capacity;
            self->capacity = self->size + self->capacity/2;
            self->data = (char *)c_realloc(self->data, cap, self->capacity + 1);
        }
        vsnprintf(self->data + pos, (size_t)n + 1, fmt, args2);
        done2: va_end(args2);
    }
    va_end(args);
}

void cstream_drop(cstream* self) {
    if (self->fp) fclose(self->fp);
    else free(self->data);
}
#undef i_implement
#endif // i_implement
