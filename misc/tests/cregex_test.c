#include "stc/cregex.h"
#include "stc/cstr.h"
#include "stc/csview.h"
#include "stc/algorithm.h"
#include "ctest.h"

#define M_START(m) ((m).buf - input)
#define M_END(m) (M_START(m) + (m).size)


TEST(cregex, ISO8601_parse_result)
{
    const char* pattern = "(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)";
    const char* input = "2024-02-28";
    csview match[4];
    EXPECT_EQ(cregex_match_aio(pattern, input, match), CREG_OK);
    EXPECT_TRUE(csview_equals(match[1], "2024"));
    EXPECT_TRUE(csview_equals(match[2], "02"));
    EXPECT_TRUE(csview_equals(match[3], "28"));
}

TEST(cregex, compile_match_char)
{
    const char* input;
    cregex re = cregex_from("äsdf");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_match_pro(&re, input="äsdf", &match, CREG_FULLMATCH), CREG_OK);
    EXPECT_EQ(M_START(match), 0);
    EXPECT_EQ(M_END(match), 5); // ä is two bytes wide

    EXPECT_EQ(cregex_match(&re, input="zäsdf", &match), CREG_OK);
    EXPECT_EQ(M_START(match), 1);
    EXPECT_EQ(M_END(match), 6);

    cregex_drop(&re);
}

TEST(cregex, compile_match_anchors)
{
    const char* input;
    cregex re = cregex_from(input="^äs.f$");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_match(&re, input="äsdf", &match), CREG_OK);
    EXPECT_EQ(M_START(match), 0);
    EXPECT_EQ(M_END(match), 5);

    EXPECT_TRUE(cregex_is_match(&re, "äs♥f"));
    EXPECT_TRUE(cregex_is_match(&re, "äsöf"));

    cregex_drop(&re);
}

TEST(cregex, compile_match_quantifiers1)
{
    const char* input;
    c_with (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("ä+");
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_match(&re, input="ääb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 4);

        EXPECT_EQ(cregex_match(&re, input="bäbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 1);
        EXPECT_EQ(M_END(match), 3);

        EXPECT_EQ(cregex_match(&re, "bbb", &match), CREG_NOMATCH);
    }
}

TEST(cregex, compile_match_quantifiers2)
{
    const char* input;
    c_with (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("bä*");
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_match(&re, input="bääb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 5);

        EXPECT_EQ(cregex_match(&re, input="bäbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 3);

        EXPECT_EQ(cregex_match(&re, input="bbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 1);
    }
}

TEST(cregex, compile_match_escaped_chars)
{
    cregex re = cregex_from("\\n\\r\\t\\{");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_match(&re, "\n\r\t{", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "\n\r\t", &match), CREG_NOMATCH);

    cregex_drop(&re);
}

TEST(cregex, compile_match_class_simple)
{
    cregex re1 = cregex_from("\\s");
    cregex re2 = cregex_from("\\w");
    cregex re3 = cregex_from("\\D");
    c_defer(
        cregex_drop(&re1), cregex_drop(&re2), cregex_drop(&re3)
    ){
        EXPECT_EQ(re1.error, 0);
        EXPECT_EQ(re2.error, 0);
        EXPECT_EQ(re3.error, 0);

        csview match;
        EXPECT_EQ(cregex_match(&re1, " " , &match), CREG_OK);
        EXPECT_EQ(cregex_match(&re1, "\r", &match), CREG_OK);
        EXPECT_EQ(cregex_match(&re1, "\n", &match), CREG_OK);

        EXPECT_EQ(cregex_match(&re2, "a", &match), CREG_OK);
        EXPECT_EQ(cregex_match(&re2, "0", &match), CREG_OK);
        EXPECT_EQ(cregex_match(&re2, "_", &match), CREG_OK);

        EXPECT_EQ(cregex_match(&re3, "k", &match), CREG_OK);
        EXPECT_EQ(cregex_match(&re3, "0", &match), CREG_NOMATCH);
    }
}

TEST(cregex, compile_match_or)
{
    cregex re, re2;
    c_defer(
        cregex_drop(&re), cregex_drop(&re2)
    ){
        re = cregex_from("as|df");
        EXPECT_EQ(re.error, 0);

        csview match[4];
        EXPECT_EQ(cregex_match(&re, "as", match), CREG_OK);
        EXPECT_EQ(cregex_match(&re, "df", match), CREG_OK);

        re2 = cregex_from("(as|df)");
        EXPECT_EQ(re2.error, 0);

        EXPECT_EQ(cregex_match(&re2, "as", match), CREG_OK);
        EXPECT_EQ(cregex_match(&re2, "df", match), CREG_OK);
    }
}

TEST(cregex, compile_match_class_complex_0)
{
    cregex re = cregex_from("[asdf]");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_match(&re, "a", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "s", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "d", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "f", &match), CREG_OK);

    cregex_drop(&re);
}

TEST(cregex, compile_match_class_complex_1)
{
    cregex re = cregex_from("[a-zä0-9öA-Z]");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_match(&re, "a", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "5", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "A", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "ä", &match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "ö", &match), CREG_OK);

    cregex_drop(&re);
}

TEST(cregex, compile_match_cap)
{
    cregex re = cregex_from("(abc)d");
    EXPECT_EQ(re.error, 0);

    csview match[4];
    EXPECT_EQ(cregex_match(&re, "abcd", match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "llljabcdkk", match), CREG_OK);
    EXPECT_EQ(cregex_match(&re, "abc", match), CREG_NOMATCH);

    cregex_drop(&re);
}

TEST(cregex, search_all)
{
    const char* input;
    c_with (cregex re = {0}, cregex_drop(&re))
    {
        re = cregex_from("ab");
        csview m = {0};
        int res;
        EXPECT_EQ(re.error, CREG_OK);
        input="ab,ab|,ab";
        res = cregex_match_next(&re, input, &m);
        EXPECT_EQ(M_START(m), 0);
        res = cregex_match_next(&re, input, &m);
        EXPECT_EQ(res, CREG_OK);
        EXPECT_EQ(M_START(m), 3);
        res = cregex_match_next(&re, input, &m);
        EXPECT_EQ(M_START(m), 7);
        res = cregex_match_next(&re, input, &m);
        EXPECT_NE(res, CREG_OK);
    }
}

TEST(cregex, captures_len)
{
    c_with (cregex re = {0}, cregex_drop(&re)) {
       re = cregex_from("(ab(cd))(ef)");
       EXPECT_EQ(cregex_captures(&re), 3);
    }
}

TEST(cregex, captures_cap)
{
    const char* input;
    c_with (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("(ab)((cd)+)");
        EXPECT_EQ(cregex_captures(&re), 3);

        csview cap[5];
        EXPECT_EQ(cregex_match(&re, input="xxabcdcde", cap), CREG_OK);
        EXPECT_TRUE(csview_equals(cap[0], "abcdcd"));

        EXPECT_EQ(M_END(cap[0]), 8);
        EXPECT_EQ(M_START(cap[1]), 2);
        EXPECT_EQ(M_END(cap[1]), 4);
        EXPECT_EQ(M_START(cap[2]), 4);
        EXPECT_EQ(M_END(cap[2]), 8);

        EXPECT_TRUE(cregex_is_match(&re, "abcdcde"));
        EXPECT_TRUE(cregex_is_match(&re, "abcdcdcd"));
    }
}

static bool add_10_years(int i, csview match, cstr* out) {
    if (i == 1) { // group 1 matches year
        int year;
        sscanf(match.buf, "%4d", &year); // scan 4 chars only
        cstr_printf(out, "%04d", year + 10);
        return true;
    }
    return false;
}

TEST(cregex, replace)
{
    const char* pattern = "\\b(\\d\\d\\d\\d)-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])\\b";
    const char* input = "start date: 2015-12-31, end date: 2022-02-28";
    cstr str = {0};
    cregex re = {0};
    c_defer(
        cstr_drop(&str), cregex_drop(&re)
    ){
        // replace with a fixed string, extended all-in-one call:
        cstr_take(&str, cregex_replace_aio(pattern, input, "YYYY-MM-DD"));
        EXPECT_STREQ(cstr_str(&str), "start date: YYYY-MM-DD, end date: YYYY-MM-DD");

        // US date format, and add 10 years to dates:
        cstr_take(&str, cregex_replace_aio_pro(pattern, csview_from(input), "$1/$3/$2",
                                               INT32_MAX, add_10_years, CREG_DEFAULT));
        EXPECT_STREQ(cstr_str(&str), "start date: 2025/31/12, end date: 2032/28/02");

        // Wrap first date inside []:
        cstr_take(&str, cregex_replace_aio_pro(pattern, csview_from(input), "[$0]", 1, NULL, CREG_DEFAULT));
        EXPECT_STREQ(cstr_str(&str), "start date: [2015-12-31], end date: 2022-02-28");

        // Wrap all words in ${}
        cstr_take(&str, cregex_replace_aio("[a-z]+", "52 apples and 31 mangoes", "$${$0}"));
        EXPECT_STREQ(cstr_str(&str), "52 ${apples} ${and} 31 ${mangoes}");

        // Compile RE separately
        re = cregex_from(pattern);
        EXPECT_EQ(cregex_captures(&re), 3);

        // European date format.
        cstr_take(&str, cregex_replace(&re, input, "$3.$2.$1"));
        EXPECT_STREQ(cstr_str(&str), "start date: 31.12.2015, end date: 28.02.2022");

        // Strip out everything but the matches
        cstr_take(&str, cregex_replace_pro(&re, csview_from(input), "$3.$2.$1;",
                                           INT32_MAX, NULL, CREG_STRIP));
        EXPECT_STREQ(cstr_str(&str), "31.12.2015;28.02.2022;");
    }
}
