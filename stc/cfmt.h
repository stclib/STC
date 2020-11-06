/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CFMT__H__
#define CFMT__H__

/*
// https://gist.github.com/alexameen/4440e4bcad557a464dcc9ff884763049
#include <stc/cfmt.h>

int main() {
    const double pi = 3.141592653589793;
    const size_t x = 1234567890;
    cstr_t string = cstr_from("Hello world");
    const char z = 'z', *test = "TEST";
    bool flag = true;
    time_t rawtime = time(NULL);
    struct tm* tm = localtime(&rawtime);
    unsigned char r = 123, g = 214, b = 90, w = 110;
    c_print(1, "Date: {} {}\n", c_ftime("%Y-%m-%d %X %Z", tm), test);
    c_print(1, "Color: ({} {} {}), {}\n", r, g, b, flag);
    c_print(1, "{:10}, {:10}, {:.2f}\n", 42, 43, 3.141592267);
    c_print(stdout, "{:>10} {:>10} {:>10}\n", z, z, w);
    c_print(stderr, "100%: {:>} {:.*} {}\n", string.str, 4, pi, x);
    c_print(&string, "Precision: {} {:.16} {}", string.str, pi, x);
    c_print(1, "{}\nvector: ({}, {}, {})\n", string.str, 3.2, 3.3, 4.2/3.2);
    cstr_del(&string);
}
*/
#include <time.h>
#include <stc/cstr.h>

STC_API char* _cfmt_parse(int nargs, const char *fmt, ...);
STC_API void _cfmt_printf(int s, const char* fmt, ...);
enum {_cfmt_bn=4, _cfmt_sn=80};
STC_API const char* _cfmt_strftime(char* n, char buf[][_cfmt_sn], size_t maxsize, const char *fmt, const struct tm *timeptr);

#ifndef __cplusplus
#define _cfmt_fn(x) _Generic ((x), \
    FILE*: fprintf, \
    char *: sprintf, \
    int: _cfmt_printf, \
    cstr_t *: cstr_fmt)

#if !defined(_MSC_VER) || defined(__clang__)
#define _cfmt_uchar() signed char: "c",
#endif

#define _cfmt(x) _Generic (x, \
    _Bool: "hhu", \
    char: "c", \
    unsigned char: "hhu", \
    _cfmt_uchar() \
    short: "hd", \
    unsigned short: "hu", \
    int: "d", \
    unsigned: "u", \
    long: "ld", \
    unsigned long: "lu", \
    long long: "lld", \
    unsigned long long: "llu", \
    float: "g", \
    double: "g", \
    long double: "Lg", \
    char *: "s", \
    void *: "p", \
    const char *: "s", \
    const void *: "p")
#else /* c++ */
    inline auto _cfmt_fn(FILE*) {return fprintf;}
    inline auto _cfmt_fn(char*) {return sprintf;}
    inline auto _cfmt_fn(int s) {return _cfmt_printf;}
    inline auto _cfmt_fn(cstr_t*) {return cstr_fmt;}
    inline auto _cfmt(bool x) {return "hhu";}
    inline auto _cfmt(char x) {return "c";}
    inline auto _cfmt(unsigned char x) {return "hhu";}
    inline auto _cfmt(short x) {return "hd";}
    inline auto _cfmt(unsigned short x) {return "hu";}
    inline auto _cfmt(int x) {return "d";}
    inline auto _cfmt(unsigned x) {return "u";}
    inline auto _cfmt(long x) {return "ld";}
    inline auto _cfmt(unsigned long x) {return "lu";}
    inline auto _cfmt(long long x) {return "lld";}
    inline auto _cfmt(unsigned long long x) {return "llu";}
    inline auto _cfmt(float x) {return "g";}
    inline auto _cfmt(double x) {return "g";}
    inline auto _cfmt(long double x) {return "Lg";}
    inline auto _cfmt(const char *x) {return "s";}
    inline auto _cfmt(const void *x) {return "p";}
#endif

/* Only for argument to c_print. Max 4 uses per c_print() statement. Buffer = 64 chars. */
#define c_ftime(fmt, timeptr) _cfmt_strftime(&_tn, _tm, sizeof _tm[0], fmt, timeptr)

/* Primary function. */
#define c_print(...) c_MACRO_OVERLOAD(c_print, __VA_ARGS__)
#define c_print_2(to, fmt) \
    do { char *_fm = _cfmt_parse(0, fmt); \
        _cfmt_fn(to)(to, _fm); free(_fm); } while (0)
#define c_print_3(to, fmt, c) \
    do { char _tm[1][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(1, fmt, _cfmt(c)); \
        _cfmt_fn(to)(to, _fm, c); free(_fm); } while (0)
#define c_print_4(to, fmt, c, d) \
    do { char _tm[2][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(2, fmt, _cfmt(c), _cfmt(d)); \
        _cfmt_fn(to)(to, _fm, c, d); free(_fm); } while (0)
#define c_print_5(to, fmt, c, d, e) \
    do { char _tm[3][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(3, fmt, _cfmt(c), _cfmt(d), _cfmt(e)); \
        _cfmt_fn(to)(to, _fm, c, d, e); free(_fm); } while (0)
#define c_print_6(to, fmt, c, d, e, f) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(4, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f); free(_fm); } while (0)
#define c_print_7(to, fmt, c, d, e, f, g) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(5, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g); free(_fm); } while (0)
#define c_print_8(to, fmt, c, d, e, f, g, h) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(6, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h); free(_fm); } while (0)
#define c_print_9(to, fmt, c, d, e, f, g, h, i) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(7, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i); free(_fm); } while (0)
#define c_print_10(to, fmt, c, d, e, f, g, h, i, j) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(8, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j); free(_fm); } while (0)
#define c_print_11(to, fmt, c, d, e, f, g, h, i, j, k) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(9, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k); free(_fm); } while (0)
#define c_print_12(to, fmt, c, d, e, f, g, h, i, j, k, m) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(10, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m); free(_fm); } while (0)
#define c_print_13(to, fmt, c, d, e, f, g, h, i, j, k, m, n) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(11, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n); free(_fm); } while (0)
#define c_print_14(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(12, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n), _cfmt(o)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n, o); free(_fm); } while (0)
#define c_print_15(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o, p) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(13, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n), _cfmt(o), _cfmt(p)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n, o, p); free(_fm); } while (0)
#define c_print_16(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o, p, q) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(14, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n), _cfmt(o), _cfmt(p), _cfmt(p)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n, o, p, q); free(_fm); } while (0)
#define c_print_17(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o, p, q, r) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(15, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n), _cfmt(o), _cfmt(p), _cfmt(p), _cfmt(r)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n, o, p, q, r); free(_fm); } while (0)
#define c_print_18(to, fmt, c, d, e, f, g, h, i, j, k, m, n, o, p, q, r, s) \
    do { char _tm[_cfmt_bn][_cfmt_sn], _tn=0, *_fm = _cfmt_parse(16, fmt, _cfmt(c), _cfmt(d), _cfmt(e), _cfmt(f), _cfmt(g), _cfmt(h), _cfmt(i), _cfmt(j), _cfmt(k), _cfmt(m), _cfmt(n), _cfmt(o), _cfmt(p), _cfmt(p), _cfmt(r), _cfmt(s)); \
        _cfmt_fn(to)(to, _fm, c, d, e, f, g, h, i, j, k, m, n, o, p, q, r, s); free(_fm); } while (0)

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#include <stdarg.h>
#include <time.h>

STC_DEF void
_cfmt_printf(int s, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(s == 1 ? stdout : stderr, fmt, args);
    va_end(args);
}

STC_DEF const char* _cfmt_strftime(char* n, char buf[][_cfmt_sn], size_t maxsize, const char *fmt, const struct tm *timeptr) {
    if (*n >= _cfmt_bn) return fmt;
    int k = (*n)++; buf[k][0] = '\0';
    strftime(buf[k], maxsize, fmt, timeptr);
    return buf[k];
}

STC_DEF char *
_cfmt_parse(int nargs, const char *fmt, ...) {
    char *fmt1 = (char *) malloc(strlen(fmt)*25/10 + 1), *p = fmt1, *p0, *arg, ch;
    const char *fmt0 = fmt;
    int n = 0, align;
    va_list args;
    va_start(args, fmt);
    do {
        switch ((ch = *fmt)) {
        case '%':
            *p++ = '%';
            break;
        case '}':
            fmt += fmt[1] == '}';
            break;
        case '{':
            if (fmt[1] == '{') { ++fmt; break; }
            if (fmt[1] != ':' && fmt[1] != '}') break;
            if (++n > nargs) break;
            fmt += 1 + (fmt[1] == ':');
            arg = va_arg(args, char *);
            *p++ = '%'; p0 = p; align = 0;
            while (1) switch (*fmt) {
                case '}': case '\0': goto done;
                case '-': ++fmt; break;
                case '>': ++fmt, align = 1; break;
                case '<': *p++ = '-', ++fmt, align = 1; break;
                case '*': if (++n <= nargs) arg = va_arg(args, char *); /* nobreak */
                default: *p++ = *fmt++;
            }
            done:
            if (!strchr("csdioxXufFeEaAgGnp", fmt[-1]))
                while (*arg) *p++ = *arg++;
            if (!align && strchr("cs", p[-1]))
                memmove(p0+1, p0, p-p0), *p0 = '-', ++p;
            fmt += *fmt == '}';
            continue;
        }
        *p++ = *fmt++;
    } while (ch);
    va_end(args);
    if (n != nargs) {
        fprintf(stderr, "error: c_print(to, fmt, ...) expected %d arg(s) after fmt, got %d:\n\tfmt: %s\n", n, nargs, fmt0);
        fmt1[0] = '\0';
    }
    return fmt1;
}

#endif

#endif