/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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

/* stctest: A small and simple C11 unit-testing framework.
 
  Features:
    - Requires C11. Should work with any C compiler supporting _Generic.
    - No library dependencies. Not even itself. Just a header file.
    - Reports assertion failures, including expressions and line numbers.
    - ANSI color output for maximum visibility.
    - Easy to embed in apps for runtime tests (e.g. environment tests).
 
  Example:
 
    #include "stctest.h"
    #include "mylib.h"
 
    void test_sheep()
    {
        EXPECT_EQ("Sheep are cool", are_sheep_cool());
        EXPECT_EQ(4, sheep.legs);
    }
 
    void test_cheese()
    {
        EXPECT_GT(cheese.tanginess, 0);
        EXPECT_EQ("Wensleydale", cheese.name);
    }
 
    int main()
    {
        RUN_TEST(test_sheep);
        RUN_TEST(test_cheese);
        return REPORT_TESTS();
    }
 */

#ifndef STCTEST_INCLUDED
#define STCTEST_INCLUDED

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <stdarg.h>
#include <inttypes.h>

#define STC_FLOAT_EPSILON 1e-6
#define STC_DOUBLE_EPSILON 1e-13


#define EXPECT_TRUE(expr) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #expr, (expr) != 0)) puts(""); } while (0)

#define EXPECT_TRUE1(expr, v) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #expr, (expr) != 0)) { \
        char _fmt[32]; sprintf(_fmt, " ; %%s --> %s\n", _stctest_FMT(v)); \
        printf(_fmt, #v, v); \
    }} while (0)

#define EXPECT_FALSE(expr) EXPECT_TRUE(!(expr))
#define EXPECT_FALSE1(expr, v) EXPECT_TRUE1(!(expr), v)

/* NB! (char*) are compared as strings. Cast to (void*) to compare pointers only */
#define EXPECT_EQ(a, b) _stctest_CHECK(a, ==, b, -STC_DOUBLE_EPSILON)
#define EXPECT_NE(a, b) _stctest_CHECK(a, !=, b, -STC_DOUBLE_EPSILON)
#define EXPECT_GT(a, b) _stctest_CHECK(a, >, b, -STC_DOUBLE_EPSILON)
#define EXPECT_LT(a, b) _stctest_CHECK(a, <, b, -STC_DOUBLE_EPSILON)
#define EXPECT_LE(a, b) _stctest_CHECK(a, <=, b, -STC_DOUBLE_EPSILON)
#define EXPECT_GE(a, b) _stctest_CHECK(a, >=, b, -STC_DOUBLE_EPSILON)
#define EXPECT_FLOAT_EQ(a, b) _stctest_CHECK((float)(a), ==, (float)(b), -STC_FLOAT_EPSILON)
#define EXPECT_DOUBLE_EQ(a, b) _stctest_CHECK((double)(a), ==, (double)(b), -STC_DOUBLE_EPSILON)
#define EXPECT_NEAR(a, b, abs_error) _stctest_CHECK((double)(a), ==, (double)(b), abs_error)

/* Run a test() function */
#define RUN_TEST(test, ...) do { \
    puts(#test "(" #__VA_ARGS__ "):"); \
    const int ps = _stctest_s.passes; \
    const int fs = _stctest_s.fails; \
    const clock_t _start = clock(); \
    test(__VA_ARGS__); \
    const int _sum = (clock() - _start)*1000 / CLOCKS_PER_SEC; \
    _stctest_s.total_ms += _sum; \
    printf("    passed: %d/%d. duration: %d ms\n", \
            _stctest_s.passes - ps, _stctest_s.passes + _stctest_s.fails - (ps + fs), _sum); \
} while (0)

#define REPORT_TESTS() stctest_report()

/* ----------------------------------------------------------------------------- */

#define _stctest_CHECK(a, OP, b, e) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #a " " #OP " " #b, _stctest_CMP(a, OP, b, e))) { \
        char _fmt[32]; sprintf(_fmt, " ; %s %s %s\n", _stctest_FMT(a), #OP, _stctest_FMT(b)); \
        printf(_fmt, a, b); \
    }} while (0)

#define _stctest_CMP(a, OP, b, e) _Generic((a), \
    const char*: _stctest_strcmp, char*: _stctest_strcmp, \
    double: _Generic((b), double: _stctest_dblcmp, float: _stctest_dblcmp, default: _stctest_valcmp), \
    float: _Generic((b), double: _stctest_dblcmp, float: _stctest_dblcmp, default: _stctest_valcmp), \
    default: _stctest_valcmp)((a) OP (b), #OP, a, b, (double)(e))

#define _stctest_FMT(a) _Generic((a), \
    float: "%.8gf", double: "%.15g", \
    int64_t: "%" PRId64, int32_t: "%" PRId32, int16_t: "%" PRId16, int8_t: "%" PRId8, \
    uint64_t: "%" PRIu64 "u", uint32_t: "%" PRIu32 "u", uint16_t: "%" PRIu16 "u", uint8_t: "%" PRIu8 "u", \
    char*: "`%s`", const char*: "`%s`", \
    default: "%p")

static int _stctest_strcmp(int res, const char* OP, ...) { 
    va_list ap;
    va_start(ap, OP);
    const char* a = va_arg(ap, const char *);
    const char* b = va_arg(ap, const char *);
    va_end(ap);
    int c = strcmp(a, b);
    if     (OP[0] == '<') return OP[1] == '=' ? c <= 0 : c < 0;
    if     (OP[0] == '>') return OP[1] == '=' ? c >= 0 : c > 0;
    return (OP[0] == '!') ^ (c == 0);
}

// Knuth:
static int approximately_equal(double a, double b, double epsilon) {
    double d = a - b; if (d < 0) d = -d;
    if (a < 0) a = -a; if (b < 0) b = -b;
    return d <= ((a < b ? b : a) * epsilon); // (a > b ? b : a) => essentially_equal:
}

static int _stctest_dblcmp(int res, const char* OP, ...) { 
    va_list ap;
    va_start(ap, OP);
    double a = va_arg(ap, double);
    double b = va_arg(ap, double);
    double e = va_arg(ap, double);
    double c = a - b;
    va_end(ap);
    if     (OP[0] == '<') return OP[1] == '=' ? c <= 0 : c < 0;
    if     (OP[0] == '>') return OP[1] == '=' ? c >= 0 : c > 0;
    return (OP[0] == '!') ^ (e < 0 ? approximately_equal(a, b, -e) : (c < 0 ? -c : c) <= e);
}

static int _stctest_valcmp(int res, const char* OP, ...)
    { return res; }

#define _stctest_COLOR_CODE 0x1B
#define _stctest_COLOR_RED "[1;31m"
#define _stctest_COLOR_GREEN "[1;32m"
#define _stctest_COLOR_RESET "[0m"

static struct stctest_s {
    int passes;
    int fails;
    const char* current_file;
    int total_ms;
} _stctest_s = {0};

static int _stctest_assert(const char* file, int line, const char* expression, int pass) {
    if (pass) 
        _stctest_s.passes++;
    else {
        _stctest_s.fails++;
        printf("    failed \"%s:%d\": (%s)", file, line, expression);
    }
    _stctest_s.current_file = file;
    return pass;
}

static int stctest_report(void) {
    if (_stctest_s.fails) {
        printf("%c%sFAILED%c%s: \"%s\": (failed %d, passed %d, total %d)\n",
               _stctest_COLOR_CODE, _stctest_COLOR_RED, _stctest_COLOR_CODE, _stctest_COLOR_RESET,
               _stctest_s.current_file, _stctest_s.fails, _stctest_s.passes, _stctest_s.passes + _stctest_s.fails);
    } else {
        printf("%c%sPASSED%c%s: \"%s\": (total %d)\n", 
               _stctest_COLOR_CODE, _stctest_COLOR_GREEN, _stctest_COLOR_CODE, _stctest_COLOR_RESET,
               _stctest_s.current_file, _stctest_s.passes);
    }
    printf("    duration: %d ms\n", _stctest_s.total_ms); \
    return -_stctest_s.fails;
}

#endif
