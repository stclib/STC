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
// This implements serialization in C.
#ifndef STC_SERIALIZE_H_INCLUDED
#define STC_SERIALIZE_H_INCLUDED

#include <stdio.h>
#include "../sys/archive.h"
#include "../sys/reflect.h"

enum {  CSERDE_SKIP = 1<<0, CSERDE_PTR = 1<<1,
        SRD_FLAG_CSERDE_SKIP = CSERDE_SKIP,
        SRD_FLAG_CSERDE_PTR = CSERDE_PTR, SRD_FLAG_ = 0 };
#define SRD_DEREF_
#define SRD_DEREF_CSERDE_PTR *
#define SRD_DEREF_CSERDE_SKIP
#define SRD_REF_ &
#define SRD_REF_CSERDE_PTR
#define SRD_REF_CSERDE_SKIP &

static inline void _to_json(const void* self, cserde_archive* ar)
    { cstream_printf(&ar->stream, "<MISSING>"); (void)self; }


#define _fx_struct_field(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    OBJTYPE SPECIFIERS SRD_DEREF_##__VA_ARGS__ NAME;


#define _fx_field_to_json(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    _rfl_field_to_json(S, NAME, OBJTYPE, SRD_DEREF_##__VA_ARGS__, SRD_REF_##__VA_ARGS__, SRD_FLAG_##__VA_ARGS__);

#define _rfl_field_to_json(S, NAME, OBJTYPE, DEREF, REF, FLAG) do { \
    if (!cserde_save_pointer(#NAME, REF self->NAME, FLAG, ar)) \
        break; \
    const char* _fmt = c_generic_fmt(DEREF self->NAME); \
    if (_fmt[0] == 'B') \
        cstream_printf(&ar->stream, "%s", _to_bool_from_opaque(REF self->NAME) ? "true" : "false"); \
    else if (_fmt[0] != 0) \
        cstream_printf(&ar->stream, _fmt, DEREF self->NAME); /* includes char* */ \
    else \
        OBJTYPE##_to_json(REF self->NAME, ar); \
    if (FLAG == CSERDE_PTR) \
        { --ar->level; cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND); } \
} while(0)

#define cserde_define_struct(S) \
    _cserde_define_struct(S, serde_struct_##S)

#define cserde_implement_struct(S) \
    _cserde_implement_struct(S, serde_struct_##S)

#define _cserde_define_struct(S, ...) \
    typedef struct S S; \
    struct S { \
        c_reflect_each(S, _fx_struct_field, __VA_ARGS__) \
    }; \
    void S##_to_json(S* self, cserde_archive* ar)

#define _cserde_implement_struct(S, ...) \
    void S##_to_json(S* self, cserde_archive* ar) { \
        c_assert(ar->fmt.comma); \
        ar->first = true; \
        ++ar->level; \
        cstream_printf(&ar->stream, "{%s%*s", SRD_NL, SRD_IND); \
        c_reflect_each(S, _fx_field_to_json, __VA_ARGS__) \
        --ar->level; \
        cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND); \
    } \
    struct _dummy

#endif // STC_SERIALIZE_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined STC_IMPLEMENT_CSERDE

#define T cserde_table, intptr_t, intptr_t, (c_declared)
#include "../hashmap.h"

bool cserde_save_pointer(const char* name, const void* field, int flag, cserde_archive* ar) {
    if (flag == CSERDE_SKIP)
        return false; /* don't serialize this field */
    const char *nl_or_space = SRD_NL[0] ? "\n" : SRD_SPACE;
    if (!ar->first)
        cstream_printf(&ar->stream, ",%s%*s", nl_or_space, SRD_IND);
    cstream_printf(&ar->stream, "\"%s\":%s", name, SRD_SPACE);
    ar->first = false;
    if (flag == CSERDE_PTR) {
        int oid = 0, last = ar->next_oid;
        if (field != NULL) {
            cserde_table_result res = cserde_table_insert(&ar->table, (intptr_t)field, ar->next_oid + 1);
            oid = (int)(res.inserted ? ++ar->next_oid : res.ref->second);
        }
        ++ar->level;
        cstream_printf(&ar->stream, "{%s%*s\"%s\":%s%d",
                       SRD_NL, SRD_IND, (oid > last ? "oid__":"ref__"), SRD_SPACE, oid);
        if (oid <= last) {
            --ar->level;
            cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND);
            return false;
        } else
            cstream_printf(&ar->stream, ",%s%*s\"val__\":%s",
                           nl_or_space, SRD_IND, SRD_SPACE);
    }
    return true;
}

void cserde_archive_drop(cserde_archive* ar) {
    cserde_table_drop(&ar->table);
    cstream_drop(&ar->stream);
}

#endif
#undef i_implement
