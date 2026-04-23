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
#ifndef STC_UTF8_DECODE_H_INCLUDED
#define STC_UTF8_DECODE_H_INCLUDED

enum { cutf8_ACCEPT=0, cutf8_REJECT=12 };

extern int      cutf8_decode_codepoint(cutf8_decode_t* d, const char* s, const char* end); // s < end
extern uint32_t cutf8_peek(const char* s);

static inline uint32_t cutf8_decode(cutf8_decode_t* d, const uint32_t byte) {
    /* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
    extern const uint8_t cutf8_dtab[]; /* utf8_decode.c */

    const uint32_t type = cutf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xffU >> type) & byte;
    return d->state = cutf8_dtab[256 + d->state + type];
}

#endif // STC_UTF8_DECODE_H_INCLUDED
