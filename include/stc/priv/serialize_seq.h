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
// This is part of implementing serialization in C.
// To be included by non-associative containers..
#include "../sys/archive.h"
#include "../sys/reflect.h"

STC_INLINE void _c_MEMB(_to_json)(Self* self, cserde_archive* ar) {
    fprintf(ar->fp, "[");
    ++ar->level;
    int first = 1;
#ifndef i_serde_key
    const char* _fmt = c_generic_fmt((i_key){0});
#endif

    for (_m_iter it = _c_MEMB(_begin)(self); it.ref; _c_MEMB(_next)(&it)) {
        const char* comma = first ? "" : (_SRD_SPACE[0] ? ", " : ",");
        fprintf(ar->fp, "%s%s%*s", comma, _SRD_NL, _SRD_IND);
#ifndef i_serde_key
        if (_fmt[0] == 'B') \
            fprintf(ar->fp, "%s", _to_bool_from_opaque(it.ref) ? "true" : "false");
        else if (_fmt[0] != 0)
            fprintf(ar->fp, _fmt, *it.ref);
#else
        c_JOIN(i_serde_key, _to_json)(it.ref, ar);
#endif
        first = 0;
    }
    --ar->level;
    fprintf(ar->fp, "%s%*s]", _SRD_NL, _SRD_IND);
}
