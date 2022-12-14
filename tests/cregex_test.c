#if 0
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stc/csview.h>
#include <stc/stctest.h>
#include <stc/cregex.h>

#define START_TEST(f) void f(void)
#define END_TEST

#define M_START(re, m) ((m).str - (re).input)
#define M_END(re, m) (M_START(re, m) + (m).size)

START_TEST(compile_match_char)
{
    cregex re = cregex_from("äsdf", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_TRUE(cregex_find("äsdf", &re, &match, 0));
    EXPECT_EQ(M_START(re, match), 0);
    EXPECT_EQ(M_END(re, match), 5); // ä is two bytes wide

    EXPECT_EQ(cregex_find("zäsdf", &re, &match, 0), 1);
    EXPECT_EQ(M_START(re, match), 1);
    EXPECT_EQ(M_END(re, match), 6);

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_anchors)
{
    cregex re = cregex_from("^äs.f$", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_TRUE(cregex_find("äsdf", &re, &match, 0));
    EXPECT_EQ(M_START(re, match), 0);
    EXPECT_EQ(M_END(re, match), 5);

    EXPECT_TRUE(cregex_is_match("äs♥f", &re));
    EXPECT_TRUE(cregex_is_match("äsöf", &re));

    cregex_drop(re);
}
END_TEST

START_TEST(compile_match_quantifiers)
{
    c_auto (cregex, re) {
        re = cregex_from("ä+", 0);
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_find("ääb", &re, &match, 0), 1);
        EXPECT_EQ(M_START(re, match), 0);
        EXPECT_EQ(M_END(re, match), 4);

        EXPECT_EQ(cregex_find("bäbb", &re, &match, 0), 1);
        EXPECT_EQ(M_START(re, match), 1);
        EXPECT_EQ(M_END(re, match), 3);

        EXPECT_FALSE(cregex_find("bbb", &re, &match, 0));
    }
    c_auto (cregex, re) {
        re = cregex_from("bä*", 0);
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_find("bääb", &re, &match, 0), 1);
        EXPECT_EQ(M_START(re, match), 0);
        EXPECT_EQ(M_END(re, match), 5);

        EXPECT_EQ(cregex_find("bäbb", &re, &match, 0), 1);
        EXPECT_EQ(M_START(re, match), 0);
        EXPECT_EQ(M_END(re, match), 3);

        EXPECT_EQ(cregex_find("bbb", &re, &match, 0), 1);
        EXPECT_EQ(M_START(re, match), 0);
        EXPECT_EQ(M_END(re, match), 1);
    }
}
END_TEST

START_TEST(compile_match_escaped_chars)
{
    cregex re = cregex_from("\\n\\r\\t\\{", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find("\n\r\t{", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("\n\r\t", &re, &match, 0), 0);

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_class_simple)
{
    c_auto (cregex, re1, re2, re3)
    {
        re1 = cregex_from("\\s", 0);
        EXPECT_EQ(re1.error, 0);
        re2 = cregex_from("\\w", 0);
        EXPECT_EQ(re2.error, 0);
        re3 = cregex_from("\\D", 0);
        EXPECT_EQ(re3.error, 0);

        csview match;
        EXPECT_EQ(cregex_find(" ", &re1, &match, 0), 1);
        EXPECT_EQ(cregex_find("\r", &re1, &match, 0), 1);
        EXPECT_EQ(cregex_find("\n", &re1, &match, 0), 1);

        EXPECT_EQ(cregex_find("a", &re2, &match, 0), 1);
        EXPECT_EQ(cregex_find("0", &re2, &match, 0), 1);
        EXPECT_EQ(cregex_find("_", &re2, &match, 0), 1);

        EXPECT_EQ( cregex_find("k", &re3, &match, 0), 1);
        EXPECT_EQ(cregex_find("0", &re3, &match, 0), 0);
    }
}
END_TEST

START_TEST(compile_match_or)
{
    c_auto (cregex, re, re2)
    {
        re = cregex_from("as|df", 0);
        EXPECT_EQ(re.error, 0);

        csview match;
        EXPECT_EQ(cregex_find("as", &re, &match, 0), 1);
        EXPECT_EQ(cregex_find("df", &re, &match, 0), 1);

        re2 = cregex_from("(as|df)", 0);
        EXPECT_EQ(re2.error, 0);

        EXPECT_EQ(cregex_find(&re2, "as", &re, &match, 0), 1);
        EXPECT_EQ(cregex_find(&re2, "df", &re, &match, 0), 1);
    }
}
END_TEST

START_TEST(compile_match_class_complex_0)
{
    cregex re = cregex_from("[asdf]", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find("a", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("s", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("d", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("f", &re, &match, 0), 1);

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_class_complex_1)
{
    cregex re = cregex_from("[a-zä0-9öA-Z]", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find("a", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("5", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("A", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("ä", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("ö", &re, &match, 0), 1);

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_cap)
{
    cregex re = cregex_from("(abc)d", 0);
    EXPECT_EQ(re.error, 0);

    csview match;
    EXPECT_EQ(cregex_find("abcd", &re, &match, 0)), 1;
    EXPECT_EQ(cregex_find("llljabcdkk", &re, &match, 0), 1);
    EXPECT_EQ(cregex_find("abc", &re, &match, 0), 0);

    cregex_drop(&re);
}
END_TEST

START_TEST(search_all)
{
    c_auto (cregex, re)
    {
        re = cregex_from("ab", 0);
        csview m = {0};
        int res;
        
        res = cregex_find("ab,ab,ab", &m, CREG_M_NEXT);
        EXPECT_TRUE(res==1 && M_START(re, m) == 0);
        res = cregex_find("ab,ab,ab", &m, CREG_M_NEXT);
        EXPECT_TRUE(res==1 && M_START(re, m) == 3);
        res = cregex_find("ab,ab,ab", &m, CREG_M_NEXT);
        EXPECT_TRUE(res==1 && M_START(re, m) == 6);
        res = cregex_find("ab,ab,ab", &m, CREG_M_NEXT);
        EXPECT_NE(res, 1);
    }
}
END_TEST

START_TEST(captures_len)
{
    c_auto (cregex, re) {
       cregex re = cregex_from("(ab(cd))(ef)", 0);
       EXPECT_EQ(cregex_captures(&re), 3);
    }
}
END_TEST

START_TEST(captures_cap)
{
    c_auto (cregex, re) {
        re = cregex_from("(ab)((cd)+)", 0);
        EXPECT_EQ(cregex_captures(&re), 3);

        csview cap[5];
        EXPECT_EQ(cregex_find("xxabcdcde", &re, cap, 0), 1);
        EXPECT_TRUE(csview_equals(cap.match[0], c_sv("abcdcd")));
        /*
        EXPECT_EQ(cap0.end, 8);
        EXPECT_EQ(cap1.start, 2);
        EXPECT_EQ(cap1.end, 4);
        EXPECT_EQ(cap2.start, 4);
        EXPECT_EQ(cap2.end, 8);
        */
        EXPECT_FALSE(cregex_is_match("abcdcde", &re));
        EXPECT_TRUE(cregex_is_match("abcdcdcd", &re));
    }
}
END_TEST


int main()
{
    RUN_TEST(compile_match_char);
    RUN_TEST(compile_match_anchors);
    RUN_TEST(compile_match_quantifiers);
    RUN_TEST(compile_match_escaped_chars);
    RUN_TEST(compile_match_class_simple);
    RUN_TEST(compile_match_class_complex_0);
    RUN_TEST(compile_match_class_complex_1);
    RUN_TEST(compile_match_cap);
    RUN_TEST(search_all);
    RUN_TEST(captures_len);
    RUN_TEST(captures_cap);
    RUN_TEST(compile_match_or);    
    return REPORT_TESTS();
}
#endif

