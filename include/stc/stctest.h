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
#include <stdarg.h>
#include <math.h>
#include <inttypes.h>

#define STCTEST_FLOAT_LIMIT 0.00000001


#define EXPECT_TRUE(expression) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #expression, (expression) != 0)) puts(""); } while(0)

#define EXPECT_FALSE(expression) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #expression, (expression) == 0)) puts(""); } while(0)

/* NB! (char*) are compared as strings. Cast to (void*) to compare pointers only */
#define EXPECT_EQ(a, b) _stctest_COMPARE(a, ==, b)
#define EXPECT_NE(a, b) _stctest_COMPARE(a, !=, b)
#define EXPECT_GT(a, b) _stctest_COMPARE(a, >, b)
#define EXPECT_LT(a, b) _stctest_COMPARE(a, <, b)
#define EXPECT_LE(a, b) _stctest_COMPARE(a, <=, b)
#define EXPECT_GE(a, b) _stctest_COMPARE(a, >=, b)

/* Run a test() function */
#define RUN_TEST(test, ...) do { \
    puts(#test "(" #__VA_ARGS__ "):"); \
    const int ps = _stctest_s.passes; \
    const int fs = _stctest_s.fails; \
    const clock_t start = clock(); \
    test(__VA_ARGS__); \
    printf("    summary: %d/%d passed, duration: %dms\n", \
            _stctest_s.passes - ps, _stctest_s.passes + _stctest_s.fails - (ps + fs), \
            (int)((clock() - start) * 1000 / CLOCKS_PER_SEC)); \
} while (0)

#define REPORT_TESTS() stctest_report()

/* ----------------------------------------------------------------------------- */

#define _stctest_COMPARE(a, OP, b) \
    do { if (!_stctest_assert(__FILE__, __LINE__, #a " " #OP " " #b, _stctest_CMP(a, OP, b))) { \
        char _fmt[32]; sprintf(_fmt, ": %s %s %s\n", _stctest_FMT(a), #OP, _stctest_FMT(b)); \
        printf(_fmt, a, b); \
    }} while (0)

#define _stctest_CMP(a, OP, b) _Generic((a), \
    const char*: _stctest_strcmp, char*: _stctest_strcmp, \
    double: _stctest_dblcmp, float: _stctest_dblcmp, \
    default: _stctest_valcmp)((a) OP (b), #OP, a, b)

#define _stctest_FMT(a) _Generic((a), \
    float: "%.8gf", double: "%.14g", \
    int64_t: "%" PRId64 "'i64", int32_t: "%" PRId32 "'i32", int16_t: "%" PRId16 "'i64", int8_t: "%" PRId8 "'i8", \
    uint64_t: "%" PRIu64 "'u64", uint32_t: "%" PRIu32 "'u32", uint16_t: "%" PRIu16 "'u16", uint8_t: "%" PRIu8 "'u8", \
    char*: "`%s`", const char*: "`%s`", \
    default: "{%p}")

static inline int _stctest_strcmp(int res, const char* OP, ...) { 
    va_list ap;
    va_start(ap, OP);
    const char* a = va_arg(ap, const char *);
    const char* b = va_arg(ap, const char *);
    va_end(ap);
    int c = strcmp(a, b);
    switch (OP[0]) {
        case '=': return c == 0;
        case '!': return c != 0;
        case '<': return OP[1] == '=' ? c <= 0 : c < 0;
        case '>': return OP[1] == '=' ? c >= 0 : c > 0;
    }
    return c;
}

static inline int _stctest_dblcmp(int res, const char* OP, ...) { 
    va_list ap;
    va_start(ap, OP);
    double a = va_arg(ap, double);
    double b = va_arg(ap, double);
    double c = a - b;
    va_end(ap);
    switch (OP[0]) {
        case '=': return fabs(c) < STCTEST_FLOAT_LIMIT;
        case '!': return fabs(c) > STCTEST_FLOAT_LIMIT;
        case '<': return OP[1] == '=' ? c <= 0 : c < 0;
        case '>': return OP[1] == '=' ? c >= 0 : c > 0;
    }    
    return res;
}

static inline int _stctest_valcmp(int res, const char* OP, ...)
    { return res; }

#define _stctest_COLOR_CODE 0x1B
#define _stctest_COLOR_RED "[1;31m"
#define _stctest_COLOR_GREEN "[1;32m"
#define _stctest_COLOR_RESET "[0m"

static struct stctest_s {
    int passes;
    int fails;
    const char* current_file;
} _stctest_s = {0};

static int _stctest_assert(const char* file, int line, const char* expression, int pass) {
    if (pass) 
        _stctest_s.passes++;
    else {
        _stctest_s.fails++;
        printf("    failed: %s:%d, In test (%s)", file, line, expression);
    }
    _stctest_s.current_file = file;
    return pass;
}

static int stctest_report(void) {
    if (_stctest_s.fails) {
        printf("%c%sFAILED%c%s [%s] (passed:%d, failed:%d, total:%d)\n",
               _stctest_COLOR_CODE, _stctest_COLOR_RED, _stctest_COLOR_CODE, _stctest_COLOR_RESET,
               _stctest_s.current_file, _stctest_s.passes, _stctest_s.fails, _stctest_s.passes + _stctest_s.fails);
        return -_stctest_s.fails;
    } else {
        printf("%c%sPASSED%c%s [%s] (total:%d)\n", 
               _stctest_COLOR_CODE, _stctest_COLOR_GREEN, _stctest_COLOR_CODE, _stctest_COLOR_RESET,
               _stctest_s.current_file, _stctest_s.passes);
        return 0;
    }
}

#endif
