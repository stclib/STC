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
        STC_FLAG_CSERDE_SKIP = CSERDE_SKIP,
        STC_FLAG_CSERDE_PTR = CSERDE_PTR, STC_FLAG_ = 0 };
#define STC_DEREF_
#define STC_DEREF_CSERDE_PTR *
#define STC_DEREF_CSERDE_SKIP
#define STC_REF_ &
#define STC_REF_CSERDE_PTR
#define STC_REF_CSERDE_SKIP &

static inline void _to_json(const void* self, cserde_archive* ar)
    { fprintf(ar->fp, "<MISSING>"); (void)self; }


#define _fx_struct_field(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    OBJTYPE SPECIFIERS STC_DEREF_##__VA_ARGS__ NAME;


#define _fx_field_to_json(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    _rfl_field_to_json(S, NAME, OBJTYPE, STC_DEREF_##__VA_ARGS__, STC_REF_##__VA_ARGS__, STC_FLAG_##__VA_ARGS__);

#define _rfl_field_to_json(S, NAME, OBJTYPE, DEREF, REF, FLAG) do { \
    if (!cserde_save_pointer(#NAME, REF self->NAME, FLAG, ar)) \
        break; \
    const char* _fmt = c_generic_fmt(DEREF self->NAME); \
    if (_fmt[0] == 'B') \
        fprintf(ar->fp, "%s", _to_bool_from_opaque(REF self->NAME) ? "true" : "false"); \
    else if (_fmt[0] != 0) \
        fprintf(ar->fp, _fmt, DEREF self->NAME); /* includes char* */ \
    else \
        OBJTYPE##_to_json(REF self->NAME, ar); \
    if (FLAG == CSERDE_PTR) \
        { --ar->level; fprintf(ar->fp, "%s%*s}", _SRD_NL, _SRD_IND); } \
} while(0)


#define c_struct(S, ...) \
    typedef struct S S; \
    struct S { \
        c_reflect_each(S, _fx_struct_field, __VA_ARGS__) \
    }; \
 \
    void S##_to_json(S* self, cserde_archive* ar) { \
        ar->first = true; \
        if (!ar->fmt.comma) ar->fmt = cserde_normal; \
        ++ar->level; \
        fprintf(ar->fp, "{%s%*s", _SRD_NL, _SRD_IND); \
        c_reflect_each(S, _fx_field_to_json, __VA_ARGS__) \
        --ar->level; \
        fprintf(ar->fp, "%s%*s}", _SRD_NL, _SRD_IND); \
    } \
    struct _dummy

#endif // STC_SERIALIZE_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined STC_SERIALIZE_C_INCLUDED && defined STC_IMPLEMENT
#define STC_SERIALIZE_C_INCLUDED

#define T cserde_table, intptr_t, intptr_t, (c_declared)
#include "../hashmap.h"

bool cserde_save_pointer(const char* name, const void* field, int flag, cserde_archive* ar) {
    if (flag == CSERDE_SKIP)
        return false; /* don't serialize this field */
    if (!ar->first)
        fprintf(ar->fp, "%s%s%*s", _SRD_COMMA, _SRD_NL, _SRD_IND);
    fprintf(ar->fp, "\"%s\":%s", name, _SRD_SPACE);
    ar->first = false;
    if (flag == CSERDE_PTR) {
        int oid = 0, last = ar->next_oid;
        if (field != NULL) {
            cserde_table_result res = cserde_table_insert(&ar->table, (intptr_t)field, ar->next_oid + 1);
            oid = (int)(res.inserted ? ++ar->next_oid : res.ref->second);
        }
        ++ar->level;
        fprintf(ar->fp, "{%s%*s\"%s\":%s%d",
                _SRD_NL, _SRD_IND, (oid > last ? "oid__":"ref__"), _SRD_SPACE, oid);
        if (oid <= last) {
            --ar->level;
            fprintf(ar->fp, "%s%*s}", _SRD_NL, _SRD_IND);
            return false;
        } else
            fprintf(ar->fp, "%s%s%*s\"val__\":%s",
                    _SRD_COMMA, _SRD_NL, _SRD_IND, _SRD_SPACE);
    }
    return true;
}

void cserde_archive_reset(cserde_archive* ar, FILE* fp) {
    cserde_table_clear(&ar->table);
    if (ar->fp) fclose(ar->fp);
    ar->fp = fp;
    ar->first = true;
    ar->next_oid = 0;
}

void cserde_archive_drop(cserde_archive* ar) {
    cserde_table_drop(&ar->table);
    if (ar->fp) fclose(ar->fp);
}

#endif
#undef i_implement
