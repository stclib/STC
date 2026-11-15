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
// To be included by arc and box.
#include "../sys/archive.h"
#include "../sys/reflect.h"

STC_API void _c_MEMB(_to_json)(Self* self, cserde_archive* ar);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF void _c_MEMB(_to_json)(Self* self, cserde_archive* ar) {
    ++ar->level;
    ++ar->next_tokpos;
    int tokpos = _cserde_register_object(self->get, ar);
    if (tokpos < ar->next_tokpos) { // for box, only if a raw pointer referred to it before this point.
        cstream_printf(&ar->stream, "{%s%*s\"<REF>\":%s", SRD_NL, SRD_IND, SRD_SPACE);
        PRINT_OBJ_ID();
        cstream_printf(&ar->stream, "%d", tokpos);
    } else {
        cstream_printf(&ar->stream, "{%s%*s\"<PTR>\":%s", SRD_NL, SRD_IND, SRD_SPACE);
        PRINT_OBJ_ID();
#ifdef i_serde_key
        c_JOIN(i_serde_key, _to_json)(self->get, ar);
#else
        const char* _fmt = c_generic_fmt((i_key){0});
        if (_fmt[0] == 'B') \
            cstream_printf(&ar->stream, "%s", *self->get ? "true" : "false");
        else if (_fmt[0] != 0)
            cstream_printf(&ar->stream, _fmt, *self->get);
#endif
    }
    --ar->level;
    cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND);
}
#endif // i_implement
