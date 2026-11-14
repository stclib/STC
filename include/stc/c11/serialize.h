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
#include "../cstr.h"
#include "../csview.h"

enum {  CSERDE_SKIP = 1<<0, CSERDE_PTR = 1<<1,
        SRD_FLAG_ = 0,
        SRD_FLAG_CSERDE_SKIP = CSERDE_SKIP,
        SRD_FLAG_CSERDE_PTR = CSERDE_PTR };
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
    if (!cserde_save_field(#NAME, (void**)&self->NAME, FLAG, ar)) \
        break; \
    const char* _fmt = c_generic_fmt(DEREF self->NAME); \
    if (_fmt[0] == 0) \
        OBJTYPE##_to_json(REF self->NAME, ar); \
    else if (_fmt[0] == 'B') \
        cstream_printf(&ar->stream, "%s", _to_bool_from_opaque(REF self->NAME) ? "true" : "false"); \
    else if (_fmt[0] == '"') \
        cstream_printf(&ar->stream, _fmt, cjson_escape_str(csview_from(*(char**)&DEREF self->NAME), &_tmp).str); /* includes char* */ \
    else \
        cstream_printf(&ar->stream, _fmt, DEREF self->NAME); /* includes char* */ \
    if (FLAG & CSERDE_PTR) \
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
        ar->initial = true; \
        ++ar->level; \
        cstream_printf(&ar->stream, "{%s%*s", SRD_NL, SRD_IND); \
        cstr _tmp = {0}; \
        c_reflect_each(S, _fx_field_to_json, __VA_ARGS__) \
        cstr_drop(&_tmp); \
        --ar->level; \
        cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND); \
    } \
    struct _dummy

static inline _Bool _to_bool_from_opaque(const void* ref)
    { return *(const _Bool*)ref; }

csviewz cjson_escape_str(csview in, cstr* out);
csviewz cjson_unescape_str(csview in, cstr* out);

#endif // STC_SERIALIZE_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined STC_IMPLEMENT_CSERDE

#define T cserde_table, intptr_t, intptr_t, (c_declared)
#include "../hashmap.h"

int cserde_register_object(void* field, cserde_archive* ar) {
    ++ar->next_oid;
    if (!field) return 0;
    cserde_table_result res = cserde_table_insert(&ar->table, (intptr_t)field, ar->next_oid);
    return (int)(res.inserted ? ar->next_oid : res.ref->second);
}

bool cserde_save_field(const char* name, void** field, int flag, cserde_archive* ar) {
    if (flag == CSERDE_SKIP)
        return false; /* don't serialize this field */
    const char *nl_or_space = SRD_NL[0] ? "\n" : SRD_SPACE;
    if (!ar->initial)
        cstream_printf(&ar->stream, ",%s%*s", nl_or_space, SRD_IND);
    cstream_printf(&ar->stream, "\"%s\":%s", name, SRD_SPACE);
    ++ar->next_oid;
    ar->initial = false;
    if (flag & CSERDE_PTR) { // field is address of the pointer
        int oid = cserde_register_object(*field, ar);
        PRINT_OBJ_ID();
        ++ar->level;
        if (oid < ar->next_oid) {
            ar->next_oid += 2;
            cstream_printf(&ar->stream, "{%s%*s\"<REF>\":%s", SRD_NL, SRD_IND, SRD_SPACE);
            PRINT_OBJ_ID();
            cstream_printf(&ar->stream, "%d%s%*s}", oid, SRD_NL,
                           ar->fmt.indent*(ar->level - 1), "");
/*
            cstream_printf(&ar->stream, "{%s%*s\"<REF>\":%s%d%s%*s}",
                           SRD_NL, SRD_IND, SRD_SPACE, oid, SRD_NL,
                           ar->fmt.indent*(ar->level - 1), "");
*/
            --ar->level;
            return false;
        } else {
            cstream_printf(&ar->stream, "{%s%*s\"<PTR>\":%s",
                           nl_or_space, SRD_IND, SRD_SPACE);
            ar->next_oid += 2;
        }
    } else { // field is actually a ref to an element, not to a pointer: register it.
        cserde_table_insert(&ar->table, (intptr_t)field, ++ar->next_oid);
        PRINT_OBJ_ID();
    }
    return true;
}

void cserde_archive_drop(cserde_archive* ar) {
    cserde_table_drop(&ar->table);
    cstream_drop(&ar->stream);
}

// ---- un/escape str ----

csviewz cjson_escape_str(const csview in, cstr* out) {
    // First pass: count output length.
    isize_t out_len = 0, n = in.size;
    for (const char *p = in.buf; n--; ++p) {
        out_len += 1 + (strchr("\"\\\t\n\r\b\f", *p) != NULL);
    }
    char *w = cstr_reserve(out, out_len);
    n = in.size;
    for (const uint8_t *p = (const uint8_t *)in.buf; n--; ++p) {
        switch (*p) {
            case '"':
            case '\\': *w++ = '\\'; *w++ = (char)*p; break;
            case '\t': *w++ = '\\'; *w++ = 't'; break;
            case '\n': *w++ = '\\'; *w++ = 'n'; break;
            case '\r': *w++ = '\\'; *w++ = 'r'; break;
            case '\b': *w++ = '\\'; *w++ = 'b'; break;
            case '\f': *w++ = '\\'; *w++ = 'f'; break;
            // allow any chars > 0x1f assuming they are unicode:
            default: if (*p > 0x1f) *w++ = (char)*p;
        }
    }
    cstr_set_size_unchecked(out, out_len);
    return cstr_zv(out);
}

csviewz cjson_unescape_str(const csview in, cstr* out) {
    char *w = cstr_reserve(out, in.size), *b = w;
    isize_t n = in.size;
    for (const char *p = in.buf; n--; ++p) {
        if (*p != '\\') { *w++ = *p; continue; }
        if (n-- == 0) break;
        p += 1;
        switch (*p) {
            case '\0':
            case '\\': *w++ = '\\'; break;
            case '"':  *w++ = '"';  break;
            case 'n':  *w++ = '\n'; break;
            case 't':  *w++ = '\t'; break;
            case 'r':  *w++ = '\r'; break;
            case 'b':  *w++ = '\b'; break;
            case 'f':  *w++ = '\f'; break;
            //case 'u': TODO; maybe
            default: *w++ = '\\'; *w++ = *p;
        }
    }
    cstr_set_size_unchecked(out, w - b);
    return cstr_zv(out);
}
#endif
#undef i_implement
