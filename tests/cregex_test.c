#if 0
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include <check.h>
#include <stc/cregex.h>

#define START_TEST(f) void f(void)
#define END_TEST

#define ck_assert(x) assert(x)
#define ck_assert_uint_ne(a, b) assert(a != b)
#define ck_assert_uint_eq(a, b) if (!((a) == (b))) printf("%u == %u: ", a, b), assert((a) == (b))
#define ck_assert_int_ne(a, b) assert(a != b)
#define ck_assert_int_eq(a, b) assert(a == b)
#define ck_assert_ptr_ne(a, b) assert(a != b)
#define ck_assert_ptr_eq(a, b) assert(a == b)


START_TEST(compile_match_char)
{
    cregex re = cregex_new("äsdf");
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(&re, "äsdf", &match));
    ck_assert_uint_eq(match.start, 0);
    ck_assert_uint_eq(match.end, 5); // ä is two bytes wide

    ck_assert(cregex_find(&re, "zäsdf", &match));
    ck_assert_uint_eq(match.start, 1);
    ck_assert_uint_eq(match.end, 6);

    ck_assert(cregex_find(&re, "äsdf", &match));
    ck_assert_uint_eq(match.start, 0);
    ck_assert_uint_eq(match.end, 5);

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_anchors)
{
    cregex re[1] = {cregex_new("^äs.f$")};
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(re, "äsdf", &match));
    ck_assert_uint_eq(match.start, 0);
    ck_assert_uint_eq(match.end, 5);

    ck_assert(cregex_find(re, "äs♥f", &match));
    ck_assert(cregex_find(re, "äsöf", &match));

    cregex_drop(re);
}
END_TEST

START_TEST(compile_match_quantifiers)
{
    c_auto (cregex, re) {
        re = cregex_new("ä+");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        cregex_match match;
        ck_assert(cregex_find(&re, "ääb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 4);

        ck_assert(cregex_find(&re, "bäbb", &match));
        ck_assert_uint_eq(match.start, 1);
        ck_assert_uint_eq(match.end, 3);

        ck_assert(!cregex_find(&re, "bbb", &match));
    }
    c_auto (cregex, re) {
        re = cregex_new("bä*");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        cregex_match match;
        ck_assert(cregex_find(&re, "bääb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 5);

        ck_assert(cregex_find(&re, "bäbb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 3);

        ck_assert(cregex_find(&re, "bbb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 1);
    }
}
END_TEST

START_TEST(compile_match_complex_quants)
{
    c_auto (cregex, re1, re2, re3, re4)
    {
        re1 = cregex_new("ä{1,3}");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        re2 = cregex_new("ä{1}");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        re3 = cregex_new("ä{,}");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        re4 = cregex_new("ä{,3}");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        cregex_match match;
        ck_assert(cregex_find(&re1, "ääb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 4);
        ck_assert(cregex_find(&re1, "äääb", &match));
        ck_assert(cregex_find(&re1, "äb", &match));
        ck_assert(!cregex_find(&re1, "b", &match));

        ck_assert(cregex_find(&re2, "ää", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 2);
        ck_assert(cregex_find(&re2, "bbäb", &match));
        ck_assert(!cregex_find(&re2, "bbbb", &match));

        ck_assert(cregex_find(&re3, "ääääääääääb", &match));
        ck_assert_uint_eq(match.start, 0);
        ck_assert_uint_eq(match.end, 20);
        ck_assert(cregex_find(&re3, "b", &match));

        ck_assert(cregex_find(&re4, "bä", &match));
        ck_assert(cregex_find(&re4, "bää", &match));
        ck_assert(cregex_find(&re4, "bäää", &match));
    }
}
END_TEST

START_TEST(compile_match_escaped_chars)
{
    cregex re = cregex_new("\\n\\r\\t\\{");
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(&re, "\n\r\t{", &match));
    ck_assert(!cregex_find(&re, "\n\r\t", &match));

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_class_simple)
{
    c_auto (cregex, re1, re2, re3)
    {
        re1 = cregex_new("\\s");
        ck_assert_int_eq(cregex_error(), cregex_OK);
        re2 = cregex_new("\\w");
        ck_assert_int_eq(cregex_error(), cregex_OK);
        re3 = cregex_new("\\D");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        cregex_match match;
        ck_assert(cregex_find(&re1, " ", &match));
        ck_assert(cregex_find(&re1, "\r", &match));
        ck_assert(cregex_find(&re1, "\n", &match));

        ck_assert(cregex_find(&re2, "a", &match));
        ck_assert(cregex_find(&re2, "0", &match));
        ck_assert(cregex_find(&re2, "_", &match));

        ck_assert(cregex_find(&re3, "k", &match));
        ck_assert(!cregex_find(&re3, "0", &match));
    }
}
END_TEST

START_TEST(compile_match_or)
{
    c_auto (cregex, re, re2)
    {
        re = cregex_new("as|df");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        cregex_match match;
        ck_assert(cregex_find(&re, "as", &match));
        ck_assert(cregex_find(&re, "df", &match));

        re2 = cregex_new("(as|df)");
        ck_assert_int_eq(cregex_error(), cregex_OK);

        ck_assert(cregex_find(&re2, "as", &match));
        ck_assert(cregex_find(&re2, "df", &match));
    }
}
END_TEST

START_TEST(compile_match_class_complex_0)
{
    cregex re = cregex_new("[asdf]");
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(&re, "a", &match));
    ck_assert(cregex_find(&re, "s", &match));
    ck_assert(cregex_find(&re, "d", &match));
    ck_assert(cregex_find(&re, "f", &match));

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_class_complex_1)
{
    cregex re = cregex_new("[a-zä0-9öA-Z]");
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(&re, "a", &match));
    ck_assert(cregex_find(&re, "5", &match));
    ck_assert(cregex_find(&re, "A", &match));
    ck_assert(cregex_find(&re, "ä", &match));
    ck_assert(cregex_find(&re, "ö", &match));

    cregex_drop(&re);
}
END_TEST

START_TEST(compile_match_cap)
{
    cregex re = cregex_new("(abc)d");
    ck_assert_int_eq(cregex_error(), cregex_OK);

    cregex_match match;
    ck_assert(cregex_find(&re, "abcd", &match));
    ck_assert(cregex_find(&re, "llljabcdkk", &match));
    ck_assert(!cregex_find(&re, "abc", &match));

    cregex_drop(&re);
}
END_TEST

START_TEST(invalid_quantifier)
{
    ck_assert_ptr_eq(cregex_new("+").nodes, NULL);
    ck_assert_int_eq(cregex_error(), cregex_EARLY_QUANTIFIER);
}
END_TEST

/* Test that invalid utf8 sequences cause a
 * cregex_INVALID_UTF8 error */
START_TEST(invalid_utf8)
{
    char s1[3] = "ä";
    s1[1] = 65; // invalid continuation byte

    ck_assert_ptr_eq(cregex_new(s1).nodes, NULL);
    ck_assert_int_eq(cregex_error(), cregex_INVALID_UTF8);
}
END_TEST

START_TEST(search_all)
{
    c_auto (cregex, re)
    {
        re = cregex_new("ab");
        cregex_match m = {0};
        bool res;
        
        res = cregex_find_next(&re, "ab,ab,ab", &m);
        ck_assert(res && m.start == 0);
        res = cregex_find_next(&re, "ab,ab,ab", &m);
        ck_assert(res && m.start == 3);
        res = cregex_find_next(&re, "ab,ab,ab", &m);
        ck_assert(res && m.start == 6);
        res = cregex_find_next(&re, "ab,ab,ab", &m);
        ck_assert(!res);
    }
}
END_TEST

START_TEST(captures_len)
{
    c_auto (cregex, re) {
       cregex re = cregex_new("(ab(cd))(ef)");
       ck_assert_uint_eq(cregex_capture_size(re), 3);
    }
}
END_TEST

START_TEST(captures_cap)
{
    c_auto (cregex, re) {
        re = cregex_new("(ab)((cd)+)");
        ck_assert_uint_eq(cregex_capture_size(re), 3);

        cregex_match match;
        ck_assert(cregex_find(&re, "xxabcdcde", &match));

        cregex_match cap0, cap1, cap2;
        cregex_capture(&re, 0, &cap0);
        cregex_capture(&re, 1, &cap1);
        cregex_capture(&re, 2, &cap2);

        ck_assert_uint_eq(cap0.start, 2);
        ck_assert_uint_eq(cap0.end, 8);
        ck_assert_uint_eq(cap1.start, 2);
        ck_assert_uint_eq(cap1.end, 4);
        ck_assert_uint_eq(cap2.start, 4);
        ck_assert_uint_eq(cap2.end, 8);

        ck_assert(!cregex_matches(&re, "abcdcde"));
        ck_assert(cregex_matches(&re, "abcdcdcd"));
    }
}
END_TEST

/*
Suite *cregex_test_suite(void)
{
    Suite *ret = suite_create("mregexp");
    TCase *tcase = tcase_create("mregexp");

    tcase_add_test(tcase, compile_match_char);
    tcase_add_test(tcase, invalid_utf8);
    tcase_add_test(tcase, compile_match_anchors);
    tcase_add_test(tcase, compile_match_quantifiers);
    tcase_add_test(tcase, invalid_quantifier);
    tcase_add_test(tcase, compile_match_complex_quants);
    tcase_add_test(tcase, compile_match_escaped_chars);
    tcase_add_test(tcase, compile_match_class_simple);
    tcase_add_test(tcase, compile_match_class_complex_0);
    tcase_add_test(tcase, compile_match_class_complex_1);
    tcase_add_test(tcase, compile_match_cap);
    tcase_add_test(tcase, search_all);
    //tcase_add_test(tcase, captures_len);
    //tcase_add_test(tcase, captures_cap);
    tcase_add_test(tcase, compile_match_or);

    suite_add_tcase(ret, tcase);
    return ret;
}

int main(void)
{
    SRunner *sr = srunner_create(cregex_test_suite());
    srunner_run_all(sr, CK_NORMAL);
    int fails = srunner_ntests_failed(sr);
    srunner_free(sr);
    return fails ? EXIT_FAILURE : EXIT_SUCCESS;
}
*/

int main()
{
    compile_match_char();
    invalid_utf8();
    compile_match_anchors();
    compile_match_quantifiers();
    invalid_quantifier();
    compile_match_complex_quants();
    compile_match_escaped_chars();
    compile_match_class_simple();
    compile_match_class_complex_0();
    compile_match_class_complex_1();
    compile_match_cap();
    search_all();
    //captures_len();
    //captures_cap();
    compile_match_or();    
    printf("All tests succesful.\n");
}
#endif