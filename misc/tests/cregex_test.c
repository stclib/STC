#define i_extern
#include <stc/cregex.h>
#include <stc/csview.h>
#include "ctest.h"

#define M_START(m) ((m).str - inp)
#define M_END(m) (M_START(m) + (m).size)


CTEST(cregex, compile_match_char)
{
    const char* inp;
    cregex re = cregex_from("äsdf", 0);
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, inp="äsdf", &match, CREG_M_FULLMATCH), CREG_OK);
    ASSERT_EQ(M_START(match), 0);
    ASSERT_EQ(M_END(match), 5); // ä is two bytes wide

    ASSERT_EQ(cregex_find(&re, inp="zäsdf", &match, 0), CREG_OK);
    ASSERT_EQ(M_START(match), 1);
    ASSERT_EQ(M_END(match), 6);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_anchors)
{
    const char* inp;
    cregex re = cregex_from(inp="^äs.f$", 0);
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, inp="äsdf", &match, 0), CREG_OK);
    ASSERT_EQ(M_START(match), 0);
    ASSERT_EQ(M_END(match), 5);

    ASSERT_TRUE(cregex_is_match(&re, "äs♥f"));
    ASSERT_TRUE(cregex_is_match(&re, "äsöf"));

    cregex_drop(&re);
}

CTEST(cregex, compile_match_quantifiers1)
{
    const char* inp;
    c_AUTO (cregex, re) {
        re = cregex_from("ä+", 0);
        ASSERT_EQ(re.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re, inp="ääb", &match, 0), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 4);

        ASSERT_EQ(cregex_find(&re, inp="bäbb", &match, 0), CREG_OK);
        ASSERT_EQ(M_START(match), 1);
        ASSERT_EQ(M_END(match), 3);

        ASSERT_EQ(cregex_find(&re, "bbb", &match, 0), CREG_NOMATCH);
    }
}

CTEST(cregex, compile_match_quantifiers2)
{   
    const char* inp; 
    c_AUTO (cregex, re) {
        re = cregex_from("bä*", 0);
        ASSERT_EQ(re.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re, inp="bääb", &match, 0), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 5);

        ASSERT_EQ(cregex_find(&re, inp="bäbb", &match, 0), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 3);

        ASSERT_EQ(cregex_find(&re, inp="bbb", &match, 0), CREG_OK);
        ASSERT_EQ(M_START(match), 0);
        ASSERT_EQ(M_END(match), 1);
    }
}

CTEST(cregex, compile_match_escaped_chars)
{
    cregex re = cregex_from("\\n\\r\\t\\{", 0);
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "\n\r\t{", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "\n\r\t", &match, 0), CREG_NOMATCH);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_class_simple)
{
    c_AUTO (cregex, re1, re2, re3)
    {
        re1 = cregex_from("\\s", 0);
        ASSERT_EQ(re1.error, 0);
        re2 = cregex_from("\\w", 0);
        ASSERT_EQ(re2.error, 0);
        re3 = cregex_from("\\D", 0);
        ASSERT_EQ(re3.error, 0);

        csview match;
        ASSERT_EQ(cregex_find(&re1, " " , &match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re1, "\r", &match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re1, "\n", &match, 0), CREG_OK);

        ASSERT_EQ(cregex_find(&re2, "a", &match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "0", &match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "_", &match, 0), CREG_OK);

        ASSERT_EQ(cregex_find(&re3, "k", &match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re3, "0", &match, 0), CREG_NOMATCH);
    }
}

CTEST(cregex, compile_match_or)
{
    c_AUTO (cregex, re, re2)
    {
        re = cregex_from("as|df", 0);
        ASSERT_EQ(re.error, 0);

        csview match[4];
        ASSERT_EQ(cregex_find(&re, "as", match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re, "df", match, 0), CREG_OK);

        re2 = cregex_from("(as|df)", 0);
        ASSERT_EQ(re2.error, 0);

        ASSERT_EQ(cregex_find(&re2, "as", match, 0), CREG_OK);
        ASSERT_EQ(cregex_find(&re2, "df", match, 0), CREG_OK);
    }
}

CTEST(cregex, compile_match_class_complex_0)
{
    cregex re = cregex_from("[asdf]", 0);
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "a", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "s", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "d", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "f", &match, 0), CREG_OK);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_class_complex_1)
{
    cregex re = cregex_from("[a-zä0-9öA-Z]", 0);
    ASSERT_EQ(re.error, 0);

    csview match;
    ASSERT_EQ(cregex_find(&re, "a", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "5", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "A", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "ä", &match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "ö", &match, 0), CREG_OK);

    cregex_drop(&re);
}

CTEST(cregex, compile_match_cap)
{
    cregex re = cregex_from("(abc)d", 0);
    ASSERT_EQ(re.error, 0);

    csview match[4];
    ASSERT_EQ(cregex_find(&re, "abcd", match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "llljabcdkk", match, 0), CREG_OK);
    ASSERT_EQ(cregex_find(&re, "abc", match, 0), CREG_NOMATCH);

    cregex_drop(&re);
}

CTEST(cregex, search_all)
{
    const char* inp;
    c_AUTO (cregex, re)
    {
        re = cregex_from("ab", 0);
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
    c_AUTO (cregex, re) {
       cregex re = cregex_from("(ab(cd))(ef)", 0);
       ASSERT_EQ(cregex_captures(&re), 4);
    }
}

CTEST(cregex, captures_cap)
{
    const char* inp;
    c_AUTO (cregex, re) {
        re = cregex_from("(ab)((cd)+)", 0);
        ASSERT_EQ(cregex_captures(&re), 4);

        csview cap[5];
        ASSERT_EQ(cregex_find(&re, inp="xxabcdcde", cap, 0), CREG_OK);
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
