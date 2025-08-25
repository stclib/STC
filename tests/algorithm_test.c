// https://mariusbancila.ro/blog/2019/01/20/cpp-code-samples-before-and-after-ranges/

#define T IVec, int
#include <stc/stack.h>

#include <stc/algorithm.h>
#include <stc/cstr.h>
#include <stc/cspan.h>
use_cspan(ISpan, int);

#include "ctest.h"


static cstr to_roman(int value)
{
    cstr result = {0};
    struct roman { int d; const char* r; };

    for (c_items(i, struct roman, {
        {1000, "M"}, {900, "CM"},
        {500, "D"}, {400, "CD"},
        {100, "C"}, {90, "XC"},
        {50, "L"}, {40, "XL"},
        {10, "X"}, {9, "IX"},
        {5, "V"}, {4, "IV"},
        {1, "I"}
    })){
        while (value >= i.ref->d) {
            cstr_append(&result, i.ref->r);
            value -= i.ref->d;
        }
    }
    return result;
}

TEST(algorithm, cstr_append)
{
    c_with (cstr s = to_roman(2024), cstr_drop(&s))
    {
        EXPECT_STREQ("MMXXIV", cstr_str(&s));
    }
}


TEST(algorithm, c_find_if)
{
    ISpan nums = c_make(ISpan, {1,1,2,3,5,8,13,21,34});

    ISpan_iter it;
    c_find_if(ISpan, nums, &it, *value == 13);

    EXPECT_EQ(13, *it.ref);
}


TEST(algorithm, c_filter)
{
    IVec vec = c_make(IVec, {1,1,2,3,5,8,13,21,34,10});
    #define f_is_even(v) ((v & 1) == 0)
    #define f_is_odd(v) ((v & 1) == 1)

    isize sum = 0;
    c_filter(IVec, vec
         , c_flt_skipwhile(f_is_odd(*value))
        && c_flt_skip(1)
        && f_is_even(*value)
        && c_flt_take(2)
        && (sum += *value));

    EXPECT_EQ(42, sum);

    uint64_t hash = 0;
    c_filter(IVec, vec, hash = c_hash_mix(hash, (uint64_t)*value));
    EXPECT_EQ(658, (isize)hash);

    hash = 0;
    c_filter_reverse(IVec, vec, hash = c_hash_mix(hash, (uint64_t)*value));
    EXPECT_EQ(10897, (isize)hash);

    sum = 0;
    c_filter(IVec, vec, sum += *value);
    EXPECT_EQ(98, sum);

    sum = 0;
    c_filter_reverse(IVec, vec, sum += *value);
    EXPECT_EQ(98, sum);

    sum = 0;
    c_filter_zip(IVec, vec, crange, c_iota(-6), sum += *value1 * *value2);
    EXPECT_EQ(73, sum);

    IVec_drop(&vec);
}

// TEST tagged_union:

c_union (Action,
    (ActionSpeak, cstr),
    (ActionQuit, bool),
    (ActionRunFunc, struct {
        int (*func)(int, int);
        int v1, v2;
    }),
);

void Action_drop(Action* self) {
    if (c_is(self, ActionSpeak, s))
        cstr_drop(s);
}

static void action(Action* action, cstr* out) {
    c_when (action) {
        c_is(ActionSpeak, s) {
            cstr_printf(out, "Asked to speak: %s\n", cstr_str(s));
        }
        c_is(ActionQuit) {
            cstr_printf(out, "Asked to quit!\n");
        }
        c_is(ActionRunFunc, r) {
            int res = r->func(r->v1, r->v2);
            cstr_printf(out, "v1: %d, v2: %d, res: %d\n", r->v1, r->v2, res);
        }
        c_otherwise assert("no match" == NULL);
    }
}

static int add(int a, int b) {
    return a + b;
}

TEST(algorithm, c_union) {
    Action act1 = c_variant(ActionSpeak, cstr_from("Hello"));
    Action act2 = c_variant(ActionQuit, 1);
    Action act3 = c_variant(ActionRunFunc, {add, 5, 6});

    cstr res = {0};
    action(&act1, &res);
    EXPECT_STREQ(cstr_str(&res), "Asked to speak: Hello\n");
    action(&act2, &res);
    EXPECT_STREQ(cstr_str(&res), "Asked to quit!\n");
    action(&act3, &res);
    EXPECT_STREQ(cstr_str(&res), "v1: 5, v2: 6, res: 11\n");

    EXPECT_TRUE(c_is_variant(&act1, ActionSpeak));
    EXPECT_STREQ(cstr_str(&act1.ActionSpeak.get), "Hello");

    c_drop(Action, &act1, &act2, &act3);
    c_drop(cstr, &res);
}
