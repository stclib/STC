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

// -------- cserde_define_struct(S) --------

#define cserde_define_struct(S) \
    _cserde_define_struct(S, serde_struct_##S)

#define _cserde_define_struct(S, ...) \
    typedef struct S S; \
    struct S { \
        c_reflect_each(S, _fx_struct_field, __VA_ARGS__) \
    }; \
    void S##_to_json(S* self, cserde_archive* ar); \
    void S##_from_json(S* self, cserde_archive* ar)

#define _fx_struct_field(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    OBJTYPE SPECIFIERS SRD_DEREF_##__VA_ARGS__ NAME;

static inline void _to_json(const void* self, cserde_archive* ar)
    { cstream_printf(&ar->stream, "<MISSING>"); (void)self; }


// -------- cserde_implement_save(S): to_json() --------

#define cserde_implement_save(S) \
    _cserde_implement_save(S, serde_struct_##S)

#define _cserde_implement_save(S, ...) \
    void S##_to_json(S* self, cserde_archive* ar) { \
        c_assert(ar->fmt.comma); \
        ar->initial = true; \
        ++ar->level; \
        cstream_printf(&ar->stream, "{%s%*s", SRD_NL, SRD_IND); \
        cstr _tmp = {0}; \
        c_reflect_each(S, _fx_save_field, __VA_ARGS__) \
        cstr_drop(&_tmp); \
        --ar->level; \
        cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND); \
    } \
    struct _dummy

static inline _Bool _to_bool_from_opaque(const void* ref)
    { return *(const _Bool*)ref; }

#define _fx_save_field(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    _rfl_save_field(S, NAME, OBJTYPE, SRD_DEREF_##__VA_ARGS__, SRD_REF_##__VA_ARGS__, SRD_FLAG_##__VA_ARGS__);

#define _rfl_save_field(S, NAME, OBJTYPE, DEREF, REF, FLAG) do { \
    if (!_cserde_save_field(#NAME, (void**)&self->NAME, FLAG, ar)) \
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


// -------- cserde_implement_load(S): from_json() --------

#define cserde_implement_load(S) \
    _cserde_implement_load(S, serde_struct_##S)

#define _cserde_implement_load(S, ...) \
    void S##_from_json(S* self, cserde_archive* ar) { \
        c_reflect_each(S, _fx_load_field, __VA_ARGS__) \
    } \
    struct _dummy

static inline void _from_json(const void* self, cserde_archive* ar)
    { (void)self; (void)ar; }

#define _fx_load_field(S, NAME, OBJTYPE, SPECIFIERS, ...) \
    _rfl_load_field(S, NAME, OBJTYPE, SRD_DEREF_##__VA_ARGS__, SRD_REF_##__VA_ARGS__, SRD_FLAG_##__VA_ARGS__);

#define _rfl_load_field(S, NAME, OBJTYPE, DEREF, REF, FLAG) do { \
    if (!cserde_load_field(#NAME, (void**)&self->NAME, FLAG, ar)) \
        break; \
    const char* _fmt = c_generic_fmt(DEREF self->NAME); \
    if (_fmt[0] == 0) \
        OBJTYPE##_from_json(REF self->NAME, ar); \
    else if (_fmt[0] == 'B') \
        cstream_printf(&ar->stream, "%s", _to_bool_from_opaque(REF self->NAME) ? "true" : "false"); \
    else if (_fmt[0] == '"') \
        cstream_printf(&ar->stream, _fmt, cjson_escape_str(csview_from(*(char**)&DEREF self->NAME), &_tmp).str); /* includes char* */ \
    else \
        cstream_printf(&ar->stream, _fmt, DEREF self->NAME); /* includes char* */ \
    if (FLAG & CSERDE_PTR) \
        { --ar->level; cstream_printf(&ar->stream, "%s%*s}", SRD_NL, SRD_IND); } \
} while(0)


// ---- UTILS ----

csviewz  cjson_escape_str(csview in, cstr* out);
csviewz  cjson_unescape_str(csview in, cstr* out);
//char*    cbase64_encode(const uint8_t* in, isize_t in_len);
//uint8_t* cbase64_decode(const char* in, isize_t in_len, uint8_t* out, isize_t* out_len);

#endif // STC_SERIALIZE_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined STC_IMPLEMENT_CSERDE

#define T cserde_table, intptr_t, intptr_t, (c_declared)
#include "../hashmap.h"

int _cserde_register_object(void* field, cserde_archive* ar) {
    ++ar->next_tokpos;
    if (!field) return 0;
    cserde_table_result res = cserde_table_insert(&ar->table, (intptr_t)field, ar->next_tokpos);
    return (int)(res.inserted ? ar->next_tokpos : res.ref->second);
}

bool _cserde_save_field(const char* name, void** field, int flag, cserde_archive* ar) {
    if (flag == CSERDE_SKIP)
        return false; /* don't serialize this field */
    const char *nl_or_space = SRD_NL[0] ? "\n" : SRD_SPACE;
    if (!ar->initial)
        cstream_printf(&ar->stream, ",%s%*s", nl_or_space, SRD_IND);
    cstream_printf(&ar->stream, "\"%s\":%s", name, SRD_SPACE);
    ++ar->next_tokpos;
    ar->initial = false;
    if (flag & CSERDE_PTR) { // field is address of the pointer
        int tokpos = _cserde_register_object(*field, ar);
        PRINT_OBJ_ID();
        ++ar->level;
        if (tokpos < ar->next_tokpos) {
            ar->next_tokpos += 2;
            cstream_printf(&ar->stream, "{%s%*s\"<REF>\":%s", SRD_NL, SRD_IND, SRD_SPACE);
            PRINT_OBJ_ID();
            cstream_printf(&ar->stream, "%d%s%*s}", tokpos, SRD_NL,
                           ar->fmt.indent*(ar->level - 1), "");
            --ar->level;
            return false;
        } else {
            cstream_printf(&ar->stream, "{%s%*s\"<PTR>\":%s",
                           nl_or_space, SRD_IND, SRD_SPACE);
            ar->next_tokpos += 2;
        }
    } else { // field is actually a ref to an element, not to a pointer: register it.
        cserde_table_insert(&ar->table, (intptr_t)field, ++ar->next_tokpos);
        PRINT_OBJ_ID();
    }
    return true;
}

void cserde_archive_drop(cserde_archive* ar) {
    cserde_table_drop(&ar->table);
    cstream_drop(&ar->stream);
}

// ---- STRING ESCAPE CONVERSION

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

/*
// ---- BASE64 CONVERSION

static const char b64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* cbase64_encode(const uint8_t* in, isize_t in_len) {
    isize_t out_cap = 4*(in_len + 2)/3 + 1;
    char* out = (char *)c_malloc(out_cap);
    if (!out) return NULL;

    isize_t i = 0, j = 0;
    for (; i + 2 < in_len; i += 3) {
        uint32_t n = ((uint32_t)in[i] << 16) |
                     ((uint32_t)in[i + 1] << 8) |
                      (uint32_t)in[i + 2];
        out[j++] = b64_chars[(n >> 18) & 0x3F];
        out[j++] = b64_chars[(n >> 12) & 0x3F];
        out[j++] = b64_chars[(n >> 6)  & 0x3F];
        out[j++] = b64_chars[ n        & 0x3F];
    }

    // Handle remaining bytes with padding
    if (i < in_len) {
        uint32_t m = ((uint32_t)in[i] << 16);
        char ch = '=';
        if (in_len - i == 2) {
            m |= (uint32_t)in[i + 1] << 8;
            ch = b64_chars[(m >> 6) & 0x3F];
        }
        out[j++] = b64_chars[(m >> 18) & 0x3F];
        out[j++] = b64_chars[(m >> 12) & 0x3F];
        out[j++] = ch;
        out[j++] = '=';
    }
    out[j] = '\0';
    return out;
}

uint8_t* cbase64_decode(const char* in, isize_t in_len, uint8_t* out, isize_t* out_len) {
    isize_t j = 0;
    uint32_t buf = 0;
    int k = 0;

    while (in_len--) {
        char ch = *in++;
        if (ch == '=') break;

        // Find index in alphabet
        char *idx = strchr(b64_chars, ch);
        if (!idx) continue; // Skip whitespace or invalid chars

        buf = (buf << 6) | (uint32_t)(idx - b64_chars);
        if (++k == 4) {
            out[j++] = (buf >> 16) & 255;
            out[j++] = (buf >> 8) & 255;
            out[j++] = buf & 255;
            buf = 0;
            k = 0;
        }
    }
    if (k == 3) {
        out[j++] = (buf >> 10) & 255;
        out[j++] = (buf >> 2) & 255;
    } else if (k == 2) {
        out[j++] = (buf >> 4) & 255;
    }
    out[ *out_len = j ] = '\0';
    return out;
}
*/
#endif // STC_IMPLEMENT_CSERDE
