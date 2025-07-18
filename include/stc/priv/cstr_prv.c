/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
// ------------------- STC_CSTR_CORE --------------------
#if !defined STC_CSTR_CORE_C_INCLUDED && \
    (defined i_implement || defined STC_CSTR_CORE)
#define STC_CSTR_CORE_C_INCLUDED

void cstr_drop(const cstr* self) {
    if (cstr_is_long(self))
        cstr_l_drop(self);
}

cstr* cstr_take(cstr* self, const cstr s) {
    if (cstr_is_long(self) && self->lon.data != s.lon.data)
        cstr_l_drop(self);
    *self = s;
    return self;
}

size_t cstr_hash(const cstr *self) {
    csview sv = cstr_sv(self);
    return c_hash_str(sv.buf);
}

isize cstr_find_sv(const cstr* self, csview search) {
    csview sv = cstr_sv(self);
    char* res = c_strnstrn(sv.buf, sv.size, search.buf, search.size);
    return res ? (res - sv.buf) : c_NPOS;
}

char* _cstr_internal_move(cstr* self, const isize pos1, const isize pos2) {
    cstr_buf b = cstr_getbuf(self);
    if (pos1 != pos2) {
        const isize newlen = (b.size + pos2 - pos1);
        if (newlen > b.cap)
            b.data = cstr_reserve(self, b.size*3/2 + pos2 - pos1);
        c_memmove(&b.data[pos2], &b.data[pos1], b.size - pos1);
        _cstr_set_size(self, newlen);
    }
    return b.data;
}

char* _cstr_init(cstr* self, const isize len, const isize cap) {
    if (cap > cstr_s_cap) {
        self->lon.data = (char *)c_malloc(cap + 1);
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    cstr_s_set_size(self, len);
    return self->sml.data;
}

char* cstr_reserve(cstr* self, const isize cap) {
    if (cstr_is_long(self)) {
        if (cap > cstr_l_cap(self)) {
            self->lon.data = (char *)c_realloc(self->lon.data, cstr_l_cap(self) + 1, cap + 1);
            cstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > cstr_s_cap) {
        char* data = (char *)c_malloc(cap + 1);
        const isize len = cstr_s_size(self);
        /* copy full short buffer to emulate realloc() */
        c_memcpy(data, self->sml.data, c_sizeof self->sml);
        self->lon.data = data;
        self->lon.size = (size_t)len;
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sml.data;
}

char* cstr_resize(cstr* self, const isize size, const char value) {
    cstr_buf b = cstr_getbuf(self);
    if (size > b.size) {
        if (size > b.cap && (b.data = cstr_reserve(self, size)) == NULL)
            return NULL;
        c_memset(b.data + b.size, value, size - b.size);
    }
    _cstr_set_size(self, size);
    return b.data;
}

isize cstr_find_at(const cstr* self, const isize pos, const char* search) {
    csview sv = cstr_sv(self);
    if (pos > sv.size) return c_NPOS;
    const char* res = strstr((char*)sv.buf + pos, search);
    return res ? (res - sv.buf) : c_NPOS;
}

char* cstr_assign_n(cstr* self, const char* str, const isize len) {
    char* d = cstr_reserve(self, len);
    if (d) { _cstr_set_size(self, len); c_memmove(d, str, len); }
    return d;
}

char* cstr_append_n(cstr* self, const char* str, const isize len) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size + len > b.cap) {
        const size_t off = (size_t)(str - b.data);
        b.data = cstr_reserve(self, b.size*3/2 + len);
        if (b.data == NULL) return NULL;
        if (off <= (size_t)b.size) str = b.data + off; /* handle self append */
    }
    c_memcpy(b.data + b.size, str, len);
    _cstr_set_size(self, b.size + len);
    return b.data;
}

cstr cstr_from_replace(csview in, csview search, csview repl, int32_t count) {
    cstr out = cstr_init();
    isize from = 0; char* res;
    if (count == 0) count = INT32_MAX;
    if (search.size)
        while (count-- && (res = c_strnstrn(in.buf + from, in.size - from, search.buf, search.size))) {
            const isize pos = (res - in.buf);
            cstr_append_n(&out, in.buf + from, pos - from);
            cstr_append_n(&out, repl.buf, repl.size);
            from = pos + search.size;
        }
    cstr_append_n(&out, in.buf + from, in.size - from);
    return out;
}

void cstr_erase(cstr* self, const isize pos, isize len) {
    cstr_buf b = cstr_getbuf(self);
    if (len > b.size - pos) len = b.size - pos;
    c_memmove(&b.data[pos], &b.data[pos + len], b.size - (pos + len));
    _cstr_set_size(self, b.size - len);
}

void cstr_shrink_to_fit(cstr* self) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size == b.cap)
        return;
    if (b.size > cstr_s_cap) {
        self->lon.data = (char *)c_realloc(self->lon.data, cstr_l_cap(self) + 1, b.size + 1);
        cstr_l_set_cap(self, b.size);
    } else if (b.cap > cstr_s_cap) {
        c_memcpy(self->sml.data, b.data, b.size + 1);
        cstr_s_set_size(self, b.size);
        c_free(b.data, b.cap + 1);
    }
}
#endif // STC_CSTR_CORE_C_INCLUDED

// ------------------- STC_CSTR_IO --------------------
#if !defined STC_CSTR_IO_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_IO)
#define STC_CSTR_IO_C_INCLUDED

char* cstr_append_uninit(cstr *self, isize len) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size + len > b.cap && (b.data = cstr_reserve(self, b.size*3/2 + len)) == NULL)
        return NULL;
    _cstr_set_size(self, b.size + len);
    return b.data + b.size;
}

bool cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    isize pos = 0;
    cstr_buf b = cstr_getbuf(self);
    for (;;) {
        if (c == delim || c == EOF) {
            _cstr_set_size(self, pos);
            return true;
        }
        if (pos == b.cap) {
            _cstr_set_size(self, pos);
            char* data = cstr_reserve(self, (b.cap = b.cap*3/2 + 16));
            b.data = data;
        }
        b.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

isize cstr_vfmt(cstr* self, isize start, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    const int n = vsnprintf(NULL, 0ULL, fmt, args);
    vsnprintf(cstr_reserve(self, start + n) + start, (size_t)n+1, fmt, args2);
    va_end(args2);
    _cstr_set_size(self, start + n);
    return n;
}

cstr cstr_from_fmt(const char* fmt, ...) {
    cstr s = cstr_init();
    va_list args;
    va_start(args, fmt);
    cstr_vfmt(&s, 0, fmt, args);
    va_end(args);
    return s;
}

isize cstr_append_fmt(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize n = cstr_vfmt(self, cstr_size(self), fmt, args);
    va_end(args);
    return n;
}

/* NB! self-data in args is UB */
isize cstr_printf(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize n = cstr_vfmt(self, 0, fmt, args);
    va_end(args);
    return n;
}
#endif // STC_CSTR_IO_C_INCLUDED

// ------------------- STC_CSTR_UTF8 --------------------
#if !defined STC_CSTR_UTF8_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_UTF8 || defined STC_UTF8_PRV_C_INCLUDED)
#define STC_CSTR_UTF8_C_INCLUDED

#include <ctype.h>

void cstr_u8_erase(cstr* self, const isize u8pos, const isize u8len) {
    csview b = cstr_sv(self);
    csview span = utf8_subview(b.buf, u8pos, u8len);
    c_memmove((void *)&span.buf[0], &span.buf[span.size], b.size - span.size - (span.buf - b.buf));
    _cstr_set_size(self, b.size - span.size);
}

bool cstr_u8_valid(const cstr* self)
    { return utf8_valid(cstr_str(self)); }

static int toLower(int c)
    { return c >= 'A' && c <= 'Z' ? c + 32 : c; }
static int toUpper(int c)
    { return c >= 'a' && c <= 'z' ? c - 32 : c; }
static struct {
    int      (*conv_asc)(int);
    uint32_t (*conv_utf)(uint32_t);
}
fn_tocase[] = {{toLower, utf8_casefold},
               {toLower, utf8_tolower},
               {toUpper, utf8_toupper}};

cstr cstr_tocase_sv(csview sv, int k) {
    cstr out = {0};
    char *buf = cstr_reserve(&out, sv.size*3/2);
    isize sz = 0;
    utf8_decode_t d = {.state=0};
    const char* end = sv.buf + sv.size;

    while (sv.buf < end) {
        sv.buf += utf8_decode_codepoint(&d, sv.buf, end);

        if (d.codep < 0x80)
            buf[sz++] = (char)fn_tocase[k].conv_asc((int)d.codep);
        else {
            uint32_t cp = fn_tocase[k].conv_utf(d.codep);
            sz += utf8_encode(buf + sz, cp);
        }
    }
    _cstr_set_size(&out, sz);
    cstr_shrink_to_fit(&out);
    return out;
}
#endif // i_import STC_CSTR_UTF8_C_INCLUDED
