#ifndef FMT_H_INCLUDED
#define FMT_H_INCLUDED
/*
void        fmt_print(dst, fmt, ...);
void        fmt_drop_buffer(fmt_buffer* buf);

  dst - destination, one of:
    int fd          1=stdout, 2=stderr
    FILE* fp        Write to a file
    char* strbuf    Write to a pre-allocated string buffer
    fmt_buffer* buf Auto realloc the needed memory (safe).
                    Set buf->stream=1 for stream-mode.
                    Call fmt_drop_buffer(buf) after usage.

  fmt - format string
    {}              Auto-detected format. If :MOD is not specified,
                    float will use ".8g" format, and double ".16g".
    {:MOD}          Format modifiers: < left align (replaces -), default for char*, char.
                                      > right align, default for numbers.
                    Other than that MOD can be normal printf format modifiers.
    {{, }}          Print chars {, and }. (note: a single % prints %).

* C11 or higher required.
* MAX 255 chars fmt string by default. MAX 12 arguments after fmt string.
* Static linking by default, shared symbols by defining FMT_HEADER / FMT_IMPLEMENT.
* (c) operamint, 2022, MIT License.
-----------------------------------------------------------------------------------
#include "fmt.h"

int main() {
    const double pi = 3.141592653589793;
    const size_t x = 1234567890;
    const char* string = "Hello world";
    const wchar_t* wstr = L"The whole";
    const char z = 'z';
    _Bool flag = 1;
    unsigned char r = 123, g = 214, b = 90, w = 110;
    char buffer[64];

    fmt_print(1, "Color: ({} {} {}), {}\n", r, g, b, flag);
    fmt_print(1, "Wide: {}, {}\n", wstr, L"wide world");
    fmt_print(1, "{:10} {:10} {:10.2f}\n", 42ull, 43, pi);
    fmt_print(stdout, "{:>10} {:>10} {:>10}\n", z, z, w);
    fmt_print(stdout, "{:10} {:10} {:10}\n", "Hello", "Mad", "World");
    fmt_print(stderr, "100%: {:<20} {:.*} {}\n", string, 4, pi, x);
    fmt_print(buffer, "Precision: {} {:.10} {}", string, pi, x);
    fmt_print(1, "{}\n", buffer);
    fmt_print(1, "Vector: ({}, {}, {})\n", 3.2, 3.3, pi);

    fmt_buffer out[1] = {{.stream=1}};
    fmt_print(out, "{} {}", "Pi is:", pi);
    fmt_print(1,   "{}, len={}, cap={}\n", out->data, out->len, out->cap);
    fmt_print(out, "{} {}", ", Pi squared is:", pi*pi);
    fmt_print(1,   "{}, len={}, cap={}\n", out->data, out->len, out->cap);
    fmt_drop_buffer(out);
}
*/
#include <stdio.h>
#include <assert.h>

#define fmt_OVERLOAD(name, ...) \
    fmt_JOIN(name, fmt_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define fmt_CONCAT(a, b) a ## b
#define fmt_JOIN(a, b) fmt_CONCAT(a, b)
#define fmt_EXPAND(...) __VA_ARGS__
#define fmt_NUMARGS(...) _fmt_APPLY_ARG_N((__VA_ARGS__, _fmt_RSEQ_N))

#define _fmt_APPLY_ARG_N(args) fmt_EXPAND(_fmt_ARG_N args)
#define _fmt_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _fmt_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                   _14, _15, _16, N, ...) N

#if defined FMT_HEADER || defined FMT_IMPLEMENT
#  define FMT_API
#else
#  define FMT_API static inline
#endif
#if defined FMT_NDEBUG || defined NDEBUG
#  define fmt_OK(exp) (void)(exp)
#else
#  define fmt_OK(exp) assert(exp)
#endif

typedef struct { 
    char* data; 
    size_t cap, len; 
    _Bool stream;
} fmt_buffer;

FMT_API void fmt_drop_buffer(fmt_buffer* buf);
FMT_API int  _fmt_parse(char* p, int nargs, const char *fmt, ...);
FMT_API void _fmt_iprint(int fd, const char* fmt, ...);
FMT_API void _fmt_bprint(fmt_buffer*, const char* fmt, ...);

#ifndef FMT_MAX
#define FMT_MAX 256
#endif

/* Primary function. */
#define fmt_print(...) fmt_OVERLOAD(fmt_print, __VA_ARGS__)
#define fmt_print2(to, fmt) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 0, fmt); \
        fmt_OK(_n == 0); _fmt_fn(to)(to, fmt); } while (0)
#define fmt_print3(to, fmt, c) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 1, fmt, _fc(c)); \
        fmt_OK(_n == 1); _fmt_fn(to)(to, _fs, c); } while (0)
#define fmt_print4(to, fmt, c, d) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 2, fmt, _fc(c), _fc(d)); \
        fmt_OK(_n == 2); _fmt_fn(to)(to, _fs, c, d); } while (0)
#define fmt_print5(to, fmt, c, d, e) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 3, fmt, _fc(c), _fc(d), _fc(e)); \
        fmt_OK(_n == 3); _fmt_fn(to)(to, _fs, c, d, e); } while (0)
#define fmt_print6(to, fmt, c, d, e, f) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 4, fmt, _fc(c), _fc(d), _fc(e), _fc(f)); \
        fmt_OK(_n == 4); _fmt_fn(to)(to, _fs, c, d, e, f); } while (0)
#define fmt_print7(to, fmt, c, d, e, f, g) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 5, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g)); \
        fmt_OK(_n == 5); _fmt_fn(to)(to, _fs, c, d, e, f, g); } while (0)
#define fmt_print8(to, fmt, c, d, e, f, g, h) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 6, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h)); \
        fmt_OK(_n == 6); _fmt_fn(to)(to, _fs, c, d, e, f, g, h); } while (0)
#define fmt_print9(to, fmt, c, d, e, f, g, h, i) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 7, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), _fc(i)); \
        fmt_OK(_n == 7); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i); } while (0)
#define fmt_print10(to, fmt, c, d, e, f, g, h, i, j) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 8, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                     _fc(i), _fc(j)); \
        fmt_OK(_n == 8); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j); } while (0)
#define fmt_print11(to, fmt, c, d, e, f, g, h, i, j, k) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 9, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                     _fc(i), _fc(j), _fc(k)); \
        fmt_OK(_n == 9); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k); } while (0)
#define fmt_print12(to, fmt, c, d, e, f, g, h, i, j, k, m) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 10, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m)); \
        fmt_OK(_n == 10); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m); } while (0)
#define fmt_print13(to, fmt, c, d, e, f, g, h, i, j, k, m, n) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 11, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m), _fc(n)); \
        fmt_OK(_n == 11); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m, n); } while (0)
#define fmt_print14(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 12, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m), _fc(n), _fc(o)); \
        fmt_OK(_n == 12); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m, n, o); } while (0)

#define _fmt_fn(x) _Generic ((x), \
    FILE*: fprintf, \
    char*: sprintf, \
    int: _fmt_iprint, \
    fmt_buffer*: _fmt_bprint)

#if defined(_MSC_VER) && !defined(__clang__)
#  define _signed_char_hhd
#else
#  define _signed_char_hhd signed char: "hhd",
#endif

#define _fc(x) _Generic (x, \
    _Bool: "d", \
    unsigned char: "hhu", \
    _signed_char_hhd \
    char: "c", \
    short: "hd", \
    unsigned short: "hu", \
    int: "d", \
    unsigned: "u", \
    long: "ld", \
    unsigned long: "lu", \
    long long: "lld", \
    unsigned long long: "llu", \
    float: "g", \
    double: "@g", \
    long double: "@Lg", \
    char*: "s", \
    wchar_t*: "ls", \
    void*: "p", \
    const char*: "s", \
    const wchar_t*: "ls", \
    const void*: "p")

#if defined FMT_IMPLEMENT || !(defined FMT_HEADER || defined FMT_IMPLEMENT)

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

FMT_API void fmt_drop_buffer(fmt_buffer* buf) {
    free(buf->data);
}

FMT_API void _fmt_iprint(int fd, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(fd == 1 ? stdout : stderr, fmt, args);
    va_end(args);
}

FMT_API void _fmt_bprint(fmt_buffer* buf, const char* fmt, ...) {
    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);
    const int n = vsnprintf(NULL, 0U, fmt, args);
    const size_t pos = buf->stream ? buf->len : 0U;
    buf->len = pos + (size_t)n;
    if (buf->len > buf->cap) {
        buf->cap = buf->len + buf->cap/2;
        buf->data = (char*)realloc(buf->data, buf->cap + 1);
    }
    vsprintf(buf->data + pos, fmt, args2);
    va_end(args2);
    va_end(args);
}

FMT_API int _fmt_parse(char* p, int nargs, const char *fmt, ...) {
    char *arg, *p0, ch;
    int n = 0, empty;
    va_list args;
    va_start(args, fmt);
    do {
        switch ((ch = *fmt)) {
        case '%':
            *p++ = '%';
            break;
        case '}':
            if (*++fmt == '}') break; /* ok */
            n = 99;
            continue;
        case '{':
            if (*++fmt == '{') break; /* ok */
            if (++n > nargs) continue;
            if (*fmt != ':' && *fmt != '}') n = 99;
            fmt += (*fmt == ':');
            empty = *fmt == '}';
            arg = va_arg(args, char *);
            *p++ = '%', p0 = p;
            while (1) switch (*fmt) {
                case '\0': n = 99; /* nobreak */
                case '}': goto done;
                case '<': *p++ = '-', ++fmt; break;
                case '>': p0 = NULL; /* nobreak */
                case '-': ++fmt; break;
                case '*': if (++n <= nargs) arg = va_arg(args, char *); /* nobreak */
                default: *p++ = *fmt++;
            }
            done:
            switch (*arg) {
            case 'g': if (empty) memcpy(p, ".8", 2), p += 2; break;
            case '@': ++arg; if (empty) memcpy(p, ".16", 3), p += 3; break;
            }
            if (!strchr("csdioxXufFeEaAgGnp", fmt[-1]))
                while (*arg) *p++ = *arg++;
            if (p0 && (p[-1] == 's' || p[-1] == 'c')) /* left-align str */
                memmove(p0 + 1, p0, p++ - p0), *p0 = '-';
            fmt += *fmt == '}';
            continue;
        }
        *p++ = *fmt++;
    } while (ch);
    va_end(args);
    return n;
}
#endif
#endif
