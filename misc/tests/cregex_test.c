#define i_import
#include "stc/cregex.h"
#include "stc/csview.h"
#include "stc/algo/utility.h"
#include "ctest.h"

#define M_START(m) ((m).buf - inp)
#define M_END(m) (M_START(m) + (m).size)


TEST(cregex, compile_match_char)
{
    const char* inp;
    cregex re = cregex_from("äsdf");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find(&re, inp="äsdf", &match, CREG_FULLMATCH), CREG_OK);
    EXPECT_EQ(M_START(match), 0);
    EXPECT_EQ(M_END(match), 5); // ä is two bytes wide

    EXPECT_EQ(cregex_find(&re, inp="zäsdf", &match), CREG_OK);
    EXPECT_EQ(M_START(match), 1);
    EXPECT_EQ(M_END(match), 6);

    cregex_drop(&re);
}

TEST(cregex, compile_match_anchors)
{
    const char* inp;
    cregex re = cregex_from(inp="^äs.f$");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find(&re, inp="äsdf", &match), CREG_OK);
    EXPECT_EQ(M_START(match), 0);
    EXPECT_EQ(M_END(match), 5);

    EXPECT_TRUE(cregex_is_match(&re, "äs♥f"));
    EXPECT_TRUE(cregex_is_match(&re, "äsöf"));

    cregex_drop(&re);
}

TEST(cregex, compile_match_quantifiers1)
{
    const char* inp;
    c_scoped (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("ä+");
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_find(&re, inp="ääb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 4);

        EXPECT_EQ(cregex_find(&re, inp="bäbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 1);
        EXPECT_EQ(M_END(match), 3);

        EXPECT_EQ(cregex_find(&re, "bbb", &match), CREG_NOMATCH);
    }
}

TEST(cregex, compile_match_quantifiers2)
{
    const char* inp;
    c_scoped (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("bä*");
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_find(&re, inp="bääb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 5);

        EXPECT_EQ(cregex_find(&re, inp="bäbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 3);

        EXPECT_EQ(cregex_find(&re, inp="bbb", &match), CREG_OK);
        EXPECT_EQ(M_START(match), 0);
        EXPECT_EQ(M_END(match), 1);
    }
}

TEST(cregex, compile_match_escaped_chars)
{
    cregex re = cregex_from("\\n\\r\\t\\{");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find(&re, "\n\r\t{", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "\n\r\t", &match), CREG_NOMATCH);

    cregex_drop(&re);
}

TEST(cregex, compile_match_class_simple)
{
    cregex re1 = cregex_from("\\s");
    cregex re2 = cregex_from("\\w");
    cregex re3 = cregex_from("\\D");
    c_defer (cregex_drop(&re1), cregex_drop(&re2), cregex_drop(&re3))
    {
        EXPECT_EQ(re1.error, 0);
        EXPECT_EQ(re2.error, 0);
        EXPECT_EQ(re3.error, 0);

        csview match;
        EXPECT_EQ(cregex_find(&re1, " " , &match), CREG_OK);
        EXPECT_EQ(cregex_find(&re1, "\r", &match), CREG_OK);
        EXPECT_EQ(cregex_find(&re1, "\n", &match), CREG_OK);

        EXPECT_EQ(cregex_find(&re2, "a", &match), CREG_OK);
        EXPECT_EQ(cregex_find(&re2, "0", &match), CREG_OK);
        EXPECT_EQ(cregex_find(&re2, "_", &match), CREG_OK);

        EXPECT_EQ(cregex_find(&re3, "k", &match), CREG_OK);
        EXPECT_EQ(cregex_find(&re3, "0", &match), CREG_NOMATCH);
    }
}

TEST(cregex, compile_match_or)
{
    cregex re, re2;
    c_defer (cregex_drop(&re), cregex_drop(&re2))
    {
        re = cregex_from("as|df");
        EXPECT_EQ(re.error, 0);

        csview match[4];
        EXPECT_EQ(cregex_find(&re, "as", match), CREG_OK);
        EXPECT_EQ(cregex_find(&re, "df", match), CREG_OK);

        re2 = cregex_from("(as|df)");
        EXPECT_EQ(re2.error, 0);

        EXPECT_EQ(cregex_find(&re2, "as", match), CREG_OK);
        EXPECT_EQ(cregex_find(&re2, "df", match), CREG_OK);
    }
}

TEST(cregex, compile_match_class_complex_0)
{
    cregex re = cregex_from("[asdf]");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find(&re, "a", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "s", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "d", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "f", &match), CREG_OK);

    cregex_drop(&re);
}

TEST(cregex, compile_match_class_complex_1)
{
    cregex re = cregex_from("[a-zä0-9öA-Z]");
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find(&re, "a", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "5", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "A", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "ä", &match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "ö", &match), CREG_OK);

    cregex_drop(&re);
}

TEST(cregex, compile_match_cap)
{
    cregex re = cregex_from("(abc)d");
    EXPECT_EQ(re.error, 0);

    csview match[4];
    EXPECT_EQ(cregex_find(&re, "abcd", match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "llljabcdkk", match), CREG_OK);
    EXPECT_EQ(cregex_find(&re, "abc", match), CREG_NOMATCH);

    cregex_drop(&re);
}

TEST(cregex, search_all)
{
    const char* inp;
    c_scoped (cregex re = {0}, cregex_drop(&re))
    {
        re = cregex_from("ab");
        csview m = {0};
        int res;
        EXPECT_EQ(re.error, CREG_OK);
        inp="ab,ab,ab";
        res = cregex_find(&re, inp, &m, CREG_NEXT);
        EXPECT_EQ(M_START(m), 0);
        res = cregex_find(&re, inp, &m, CREG_NEXT);
        EXPECT_EQ(res, CREG_OK);
        EXPECT_EQ(M_START(m), 3);
        res = cregex_find(&re, inp, &m, CREG_NEXT);
        EXPECT_EQ(M_START(m), 6);
        res = cregex_find(&re, inp, &m, CREG_NEXT);
        EXPECT_NE(res, CREG_OK);
    }
}

TEST(cregex, captures_len)
{
    c_scoped (cregex re = {0}, cregex_drop(&re)) {
       re = cregex_from("(ab(cd))(ef)");
       EXPECT_EQ(cregex_captures(&re), 3);
    }
}

TEST(cregex, captures_cap)
{
    const char* inp;
    c_scoped (cregex re = {0}, cregex_drop(&re)) {
        re = cregex_from("(ab)((cd)+)");
        EXPECT_EQ(cregex_captures(&re), 3);

        csview cap[5];
        EXPECT_EQ(cregex_find(&re, inp="xxabcdcde", cap), CREG_OK);
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
    const char* pattern = "\\b(\\d\\d\\d\\d)-(1[0-2]|0[1-9])-(3[01]|[12][0-9]|0[1-9])\\b";
    const char* input = "start date: 2015-12-31, end date: 2022-02-28";
    cstr str = {0};
    cregex re = {0};
    c_defer(
        cstr_drop(&str),
        cregex_drop(&re)
    ){
        // replace with a fixed string, extended all-in-one call:
        cstr_take(&str, cregex_replace_pattern(pattern, input, "YYYY-MM-DD"));
        EXPECT_STREQ(cstr_str(&str), "start date: YYYY-MM-DD, end date: YYYY-MM-DD");

        // US date format, and add 10 years to dates:
        cstr_take(&str, cregex_replace_pattern(pattern, input, "$1/$3/$2", 0, add_10_years, CREG_DEFAULT));
        EXPECT_STREQ(cstr_str(&str), "start date: 2025/31/12, end date: 2032/28/02");

        // Wrap first date inside []:
        cstr_take(&str, cregex_replace_pattern(pattern, input, "[$0]", 1));
        EXPECT_STREQ(cstr_str(&str), "start date: [2015-12-31], end date: 2022-02-28");

        // Wrap all words in ${}
        cstr_take(&str, cregex_replace_pattern("[a-z]+", "52 apples and 31 mangoes", "$${$0}"));
        EXPECT_STREQ(cstr_str(&str), "52 ${apples} ${and} 31 ${mangoes}");

        // Compile RE separately
        re = cregex_from(pattern);
        EXPECT_EQ(cregex_captures(&re), 3);

        // European date format.
        cstr_take(&str, cregex_replace(&re, input, "$3.$2.$1"));
        EXPECT_STREQ(cstr_str(&str), "start date: 31.12.2015, end date: 28.02.2022");

        // Strip out everything but the matches
        cstr_take(&str, cregex_replace_sv(&re, csview_from(input), "$3.$2.$1;", 0, NULL, CREG_STRIP));
        EXPECT_STREQ(cstr_str(&str), "31.12.2015;28.02.2022;");
    }
}
