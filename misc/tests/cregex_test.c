#define i_extern
#include <stc/cregex.h>
#include <stc/csview.h>
#include "ctest.h"

#define M_START(m) ((m).str - inp)
#define M_END(m) (M_START(m) + (m).size)


CTEST(cregex, compile_match_char)
{
    const char* inp;
    cregex re = cregex_from("äsdf");
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, inp="äsdf", &match, CREG_M_FULLMATCH), CREG_OK);
    ASSERT_EQ(M_START(match), 0);
    ASSERT_EQ(M_END(match), 5); // ä is two bytes wide

    ASSERT_EQ(cregex_find(&re, inp="zäsdf", &match), CREG_OK);
    ASSERT_EQ(M_START(match), 1);
    ASSERT_EQ(M_END(match), 6);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_anchors)
{
    const char* inp;
    cregex re = cregex_from(inp="^äs.f$");
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, inp="äsdf", &match), CREG_OK);
    ASSERT_EQ(M_START(match), 0);
    ASSERT_EQ(M_END(match), 5);

    ASSERT_TRUE(cregex_is_match(&re, "äs♥f"));
    ASSERT_TRUE(cregex_is_match(&re, "äsöf"));

    cregex_drop(&re);
}

CTEST(cregex, compile_match_quantifiers1)
{
    const char* inp;
    c_auto (cregex, re) {
        re = cregex_from("ä+");
        ASSERT_EQ(re.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re, inp="ääb", &match), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 4);

        ASSERT_EQ(cregex_find(&re, inp="bäbb", &match), CREG_OK);
        ASSERT_EQ(M_START(match), 1);
        ASSERT_EQ(M_END(match), 3);

        ASSERT_EQ(cregex_find(&re, "bbb", &match), CREG_NOMATCH);
    }
}

CTEST(cregex, compile_match_quantifiers2)
{   
    const char* inp; 
    c_auto (cregex, re) {
        re = cregex_from("bä*");
        ASSERT_EQ(re.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re, inp="bääb", &match), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 5);

        ASSERT_EQ(cregex_find(&re, inp="bäbb", &match), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 3);

        ASSERT_EQ(cregex_find(&re, inp="bbb", &match), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 1);
    }
}

CTEST(cregex, compile_match_escaped_chars)
{
    cregex re = cregex_from("\\n\\r\\t\\{");
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "\n\r\t{", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "\n\r\t", &match), CREG_NOMATCH);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_class_simple)
{
    c_auto (cregex, re1, re2, re3)
    {
        re1 = cregex_from("\\s");
        ASSERT_EQ(re1.error, 0);
        re2 = cregex_from("\\w");
        ASSERT_EQ(re2.error, 0);
        re3 = cregex_from("\\D");
        ASSERT_EQ(re3.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re1, " " , &match), CREG_OK);
        ASSERT_EQ(cregex_find(&re1, "\r", &match), CREG_OK);
        ASSERT_EQ(cregex_find(&re1, "\n", &match), CREG_OK);

        ASSERT_EQ(cregex_find(&re2, "a", &match), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "0", &match), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "_", &match), CREG_OK);

        ASSERT_EQ(cregex_find(&re3, "k", &match), CREG_OK);
        ASSERT_EQ(cregex_find(&re3, "0", &match), CREG_NOMATCH);
    }
}

CTEST(cregex, compile_match_or)
{
    c_auto (cregex, re, re2)
    {
        re = cregex_from("as|df");
        ASSERT_EQ(re.error, 0);

        csview match[4];
        ASSERT_EQ(cregex_find(&re, "as", match), CREG_OK);
        ASSERT_EQ(cregex_find(&re, "df", match), CREG_OK);

        re2 = cregex_from("(as|df)");
        ASSERT_EQ(re2.error, 0);

        ASSERT_EQ(cregex_find(&re2, "as", match), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "df", match), CREG_OK);
    }
}

CTEST(cregex, compile_match_class_complex_0)
{
    cregex re = cregex_from("[asdf]");
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "a", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "s", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "d", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "f", &match), CREG_OK);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_class_complex_1)
{
    cregex re = cregex_from("[a-zä0-9öA-Z]");
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "a", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "5", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "A", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "ä", &match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "ö", &match), CREG_OK);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_cap)
{
    cregex re = cregex_from("(abc)d");
    ASSERT_EQ(re.error, 0);

    csview match[4];
    ASSERT_EQ(cregex_find(&re, "abcd", match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "llljabcdkk", match), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "abc", match), CREG_NOMATCH);

    cregex_drop(&re);
}

CTEST(cregex, search_all)
{
    const char* inp;
    c_auto (cregex, re)
    {
        re = cregex_from("ab");
        csview m = {0};
        int res;
        ASSERT_EQ(re.error, CREG_OK);
        inp="ab,ab,ab";
        res = cregex_find(&re, inp, &m, CREG_M_NEXT);
        ASSERT_EQ(M_START(m), 0);
        res = cregex_find(&re, inp, &m, CREG_M_NEXT);
        ASSERT_EQ(res, CREG_OK);
        ASSERT_EQ(M_START(m), 3);
        res = cregex_find(&re, inp, &m, CREG_M_NEXT);
        ASSERT_EQ(M_START(m), 6);
        res = cregex_find(&re, inp, &m, CREG_M_NEXT);
        ASSERT_NE(res, CREG_OK);
    }
}

CTEST(cregex, captures_len)
{
    c_auto (cregex, re) {
       re = cregex_from("(ab(cd))(ef)");
       ASSERT_EQ(cregex_captures(&re), 4);
    }
}

CTEST(cregex, captures_cap)
{
    const char* inp;
    c_auto (cregex, re) {
        re = cregex_from("(ab)((cd)+)");
        ASSERT_EQ(cregex_captures(&re), 4);

        csview cap[5];
        ASSERT_EQ(cregex_find(&re, inp="xxabcdcde", cap), CREG_OK);
        ASSERT_TRUE(csview_equals(cap[0], "abcdcd"));

        ASSERT_EQ(M_END(cap[0]), 8);
        ASSERT_EQ(M_START(cap[1]), 2);
        ASSERT_EQ(M_END(cap[1]), 4);
        ASSERT_EQ(M_START(cap[2]), 4);
        ASSERT_EQ(M_END(cap[2]), 8);
    
        ASSERT_TRUE(cregex_is_match(&re, "abcdcde"));
        ASSERT_TRUE(cregex_is_match(&re, "abcdcdcd"));
    }
}

static bool add_10_years(int i, csview match, cstr* out) {
    if (i == 1) { // group 1 matches year
        int year;
        sscanf(match.str, "%4d", &year); // scan 4 chars only
        cstr_printf(out, "%04d", year + 10);
        return true;
    }
    return false;
}

CTEST(cregex, replace)
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
        ASSERT_STREQ(cstr_str(&str), "start date: YYYY-MM-DD, end date: YYYY-MM-DD");

        // US date format, and add 10 years to dates:
        cstr_take(&str, cregex_replace_pattern(pattern, input, "$1/$3/$2", 0, add_10_years, CREG_DEFAULT));
        ASSERT_STREQ(cstr_str(&str), "start date: 2025/31/12, end date: 2032/28/02");

        // Wrap first date inside []:
        cstr_take(&str, cregex_replace_pattern(pattern, input, "[$0]", 1));
        ASSERT_STREQ(cstr_str(&str), "start date: [2015-12-31], end date: 2022-02-28");

        // Wrap all words in ${}
        cstr_take(&str, cregex_replace_pattern("[a-z]+", "52 apples and 31 mangoes", "$${$0}"));
        ASSERT_STREQ(cstr_str(&str), "52 ${apples} ${and} 31 ${mangoes}");

        // Compile RE separately
        re = cregex_from(pattern);
        ASSERT_EQ(cregex_captures(&re), 4);

        // European date format.
        cstr_take(&str, cregex_replace(&re, input, "$3.$2.$1"));
        ASSERT_STREQ(cstr_str(&str), "start date: 31.12.2015, end date: 28.02.2022");

        // Strip out everything but the matches
        cstr_take(&str, cregex_replace_sv(&re, csview_from(input), "$3.$2.$1;", 0, NULL, CREG_R_STRIP));
        ASSERT_STREQ(cstr_str(&str), "31.12.2015;28.02.2022;");
    }
}
