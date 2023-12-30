#ifndef FMT_H_INCLUDED
#define FMT_H_INCLUDED
/*
VER 2.2: NEW API:
void        fmt_print(fmt, ...);
void        fmt_println(fmt, ...);
void        fmt_printd(dst, fmt, ...);
const char* fmt_tm(fmt, struct tm* tp);
void        fmt_close(fmt_stream* ss);

  dst - destination, one of:
    FILE* fp        Write to a file
    char* strbuf    Write to a pre-allocated string buffer
    fmt_stream* ss  Write to a string-stream (auto allocated).
                    Set ss->overwrite=1 for overwrite-mode.
                    Call fmt_close(ss) after usage.

  fmt - format string (const char*)
    {}              Auto-detected format. If :MOD is not specified,
                    float will use ".8g" format, and double ".16g".
    {:MODS}         Format modifiers: '<' left align (replaces '-'). Default for char* and char.
                                      '>' right align. Default for numbers.
                    Other than that MODS can be regular printf() format modifiers.
    {{  }}  %       Print the '{', '}', and '%' characters.

* C11 or higher required.
* MAX 255 chars fmt string by default. MAX 12 arguments after fmt string.
* Define FMT_IMPLEMENT, STC_IMPLEMENT or i_implement prior to #include in one translation unit.
* (c) operamint, 2022, MIT License.
-----------------------------------------------------------------------------------
#define FMT_IMPLEMENT
#include "c11/fmt.h"

int main(void) {
    const double pi = 3.141592653589793;
    const size_t x = 1234567890;
    const char* string = "Hello world";
    const wchar_t* wstr = L"The whole";
    const char z = 'z';
    _Bool flag = 1;
    unsigned char r = 123, g = 214, b = 90, w = 110;
    char buffer[64];

    fmt_print("Color: ({} {} {}), {}\n", r, g, b, flag);
    fmt_println("Wide: {}, {}", wstr, L"wide world");
    fmt_println("{:10} {:10} {:10.2f}", 42ull, 43, pi);
    fmt_println("{:>10} {:>10} {:>10}", z, z, w);
    fmt_printd(stdout, "{:10} {:10} {:10}\n", "Hello", "Mad", "World");
    fmt_printd(stderr, "100%: {:<20} {:.*} {}\n", string, 4, pi, x);
    fmt_printd(buffer, "Precision: {} {:.10} {}", string, pi, x);
    fmt_println("{}", buffer);
    fmt_println("Vector: ({}, {}, {})", 3.2, 3.3, pi);

    fmt_stream ss[1] = {0};
    fmt_printd(ss, "{} {}", "Pi is:", pi);
    fmt_print("{}, len={}, cap={}\n", ss->data, ss->len, ss->cap);
    fmt_printd(ss, "{} {}", ", Pi squared is:", pi*pi);
    fmt_print("{}, len={}, cap={}\n", ss->data, ss->len, ss->cap);
    fmt_close(ss);

    time_t now = time(NULL);
    struct tm t1 = *localtime(&now), t2 = t1;
    t2.tm_year += 2;
    // NB! max 2 fmt_tm() calls per fmt_print()!
    fmt_print("Dates: {} and {}\n", fmt_tm("%Y-%m-%d %X %Z", &t1),
                                    fmt_tm("%Y-%m-%d %X %Z", &t2));
}
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
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

#if defined FMT_NDEBUG || defined STC_NDEBUG || defined NDEBUG
#  define fmt_OK(exp) (void)(exp)
#else
#  define fmt_OK(exp) assert(exp)
#endif

typedef struct {
    char* data;
    intptr_t cap, len;
    _Bool overwrite;
} fmt_stream;

#if defined FMT_STATIC || defined STC_STATIC || defined i_static
  #define FMT_API static
  #define FMT_DEF static
#elif defined FMT_IMPLEMENT || defined STC_IMPLEMENT || defined i_implement
  #define FMT_API extern
  #define FMT_DEF
#else
  #define FMT_API
#endif

struct tm;  /* Max 2 usages. Buffer = 64 chars. */
FMT_API const char* fmt_tm(const char *fmt, const struct tm *tp);
FMT_API void        fmt_close(fmt_stream* ss);
FMT_API int        _fmt_parse(char* p, int nargs, const char *fmt, ...);
FMT_API void       _fmt_sprint(fmt_stream*, const char* fmt, ...);

#ifndef FMT_MAX
#define FMT_MAX 128
#endif

#define fmt_print(...) fmt_printd(stdout, __VA_ARGS__)
#define fmt_println(...) fmt_printd((fmt_stream*)0, __VA_ARGS__)
#define fmt_printd(...) fmt_OVERLOAD(fmt_printd, __VA_ARGS__)

/* Primary function. */
#define fmt_printd2(to, fmt) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 0, fmt); \
        fmt_OK(_n == 0); _fmt_fn(to)(to, fmt); } while (0)
#define fmt_printd3(to, fmt, c) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 1, fmt, _fc(c)); \
        fmt_OK(_n == 1); _fmt_fn(to)(to, _fs, c); } while (0)
#define fmt_printd4(to, fmt, c, d) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 2, fmt, _fc(c), _fc(d)); \
        fmt_OK(_n == 2); _fmt_fn(to)(to, _fs, c, d); } while (0)
#define fmt_printd5(to, fmt, c, d, e) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 3, fmt, _fc(c), _fc(d), _fc(e)); \
        fmt_OK(_n == 3); _fmt_fn(to)(to, _fs, c, d, e); } while (0)
#define fmt_printd6(to, fmt, c, d, e, f) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 4, fmt, _fc(c), _fc(d), _fc(e), _fc(f)); \
        fmt_OK(_n == 4); _fmt_fn(to)(to, _fs, c, d, e, f); } while (0)
#define fmt_printd7(to, fmt, c, d, e, f, g) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 5, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g)); \
        fmt_OK(_n == 5); _fmt_fn(to)(to, _fs, c, d, e, f, g); } while (0)
#define fmt_printd8(to, fmt, c, d, e, f, g, h) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 6, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h)); \
        fmt_OK(_n == 6); _fmt_fn(to)(to, _fs, c, d, e, f, g, h); } while (0)
#define fmt_printd9(to, fmt, c, d, e, f, g, h, i) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 7, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), _fc(i)); \
        fmt_OK(_n == 7); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i); } while (0)
#define fmt_printd10(to, fmt, c, d, e, f, g, h, i, j) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 8, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                     _fc(i), _fc(j)); \
        fmt_OK(_n == 8); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j); } while (0)
#define fmt_printd11(to, fmt, c, d, e, f, g, h, i, j, k) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 9, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                     _fc(i), _fc(j), _fc(k)); \
        fmt_OK(_n == 9); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k); } while (0)
#define fmt_printd12(to, fmt, c, d, e, f, g, h, i, j, k, m) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 10, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m)); \
        fmt_OK(_n == 10); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m); } while (0)
#define fmt_printd13(to, fmt, c, d, e, f, g, h, i, j, k, m, n) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 11, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m), _fc(n)); \
        fmt_OK(_n == 11); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m, n); } while (0)
#define fmt_printd14(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o) \
    do { char _fs[FMT_MAX]; int _n = _fmt_parse(_fs, 12, fmt, _fc(c), _fc(d), _fc(e), _fc(f), _fc(g), _fc(h), \
                                                                      _fc(i), _fc(j), _fc(k), _fc(m), _fc(n), _fc(o)); \
        fmt_OK(_n == 12); _fmt_fn(to)(to, _fs, c, d, e, f, g, h, i, j, k, m, n, o); } while (0)

#define _fmt_fn(x) _Generic ((x), \
    FILE*: fprintf, \
    char*: sprintf, \
    fmt_stream*: _fmt_sprint)

#if defined(_MSC_VER) && !defined(__clang__)
  #define _signed_char_hhd
#else
  #define _signed_char_hhd signed char: "hhd",
#endif
#ifdef __GNUC__
  #define FMT_UNUSED __attribute__((unused))
#else
  #define FMT_UNUSED
#endif

#define _fc(x) _Generic (x, \
    _Bool: "d", \
    unsigned char: "hhu", \
    _signed_char_hhd \
    char: "hhd", \
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

#if defined FMT_DEF

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

FMT_DEF FMT_UNUSED void fmt_close(fmt_stream* ss) {
    free(ss->data);
}

FMT_DEF FMT_UNUSED const char* fmt_tm(const char *fmt, const struct tm *tp) {
    static char buf[2][64];
    static int i;
    strftime(buf[(i = !i)], sizeof(buf[0]) - 1, fmt, tp);
    return buf[i];
}

FMT_DEF void _fmt_sprint(fmt_stream* ss, const char* fmt, ...) {
    va_list args, args2;
    va_start(args, fmt);
    if (ss == NULL) {
        vprintf(fmt, args); putchar('\n');
        goto done1;
    }
    va_copy(args2, args);
    const int n = vsnprintf(NULL, 0U, fmt, args);
    if (n < 0) goto done2;
    const intptr_t pos = ss->overwrite ? 0 : ss->len;
    ss->len = pos + n;
    if (ss->len > ss->cap) {
        ss->cap = ss->len + ss->cap/2;
        ss->data = (char*)realloc(ss->data, (size_t)ss->cap + 1U);
    }
    vsprintf(ss->data + pos, fmt, args2);
    done2: va_end(args2);
    done1: va_end(args);
}

FMT_DEF int _fmt_parse(char* p, int nargs, const char *fmt, ...) {
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
                case '\0': n = 99; /* fall through */
                case '}': goto done;
                case '<': *p++ = '-', ++fmt; break;
                case '>': p0 = NULL; /* fall through */
                case '-': ++fmt; break;
                case '*': if (++n <= nargs) arg = va_arg(args, char *); /* fall through */
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
                memmove(p0 + 1, p0, (size_t)(p++ - p0)), *p0 = '-';
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
#undef i_implement
#undef i_static
