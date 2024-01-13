// https://mariusbancila.ro/blog/2019/01/20/cpp-code-samples-before-and-after-ranges/

#define i_TYPE IVec,int
#include "stc/stack.h"
#include "stc/algorithm.h"

#define i_static
#include "stc/cstr.h"
#include "stc/cspan.h"
using_cspan(ISpan, int);

#include "ctest.h"


static cstr to_roman(int value)
{
    cstr result = {0};
    struct roman { int d; const char* r; };

    c_forlist (i, struct roman, {
        {1000, "M"}, {900, "CM"},
        {500, "D"}, {400, "CD"},
        {100, "C"}, {90, "XC"},
        {50, "L"}, {40, "XL"},
        {10, "X"}, {9, "IX"},
        {5, "V"}, {4, "IV"},
        {1, "I"}
    }){
        while (value >= i.ref->d) {
            cstr_append(&result, i.ref->r);
            value -= i.ref->d;
        }
    }
    return result;
}

CTEST(algorithm, cstr_append)
{
    c_scoped (cstr s = to_roman(2024), cstr_drop(&s))
    {
        ASSERT_STREQ("MMXXIV", cstr_str(&s));
    }
}


CTEST(algorithm, c_find_if)
{
    ISpan nums = c_init(ISpan, {1,1,2,3,5,8,13,21,34});

    ISpan_iter it;
    c_find_if(ISpan, nums, &it, *value == 13);

    ASSERT_EQ(13, *it.ref);
}


CTEST(algorithm, c_filter)
{
    IVec vec = c_init(IVec, {1,1,2,3,5,8,13,21,34});
    #define f_is_even(v) ((v & 1) == 0)
    #define f_is_odd(v) ((v & 1) == 1)

    int sum = 0;
    c_filter(IVec, vec
         , c_flt_skip(3)
        && c_flt_skipwhile(f_is_odd(*value))
        && c_flt_take(4)
        && f_is_even(*value)
        && (sum += *value));

    ASSERT_EQ(42, sum);

    uint64_t hash = 0;
    c_filter(IVec, vec, hash = c_hash_mix(hash, *value));
    ASSERT_EQ(332, hash);

    hash = 0;
    c_filter_reverse(IVec, vec, hash = c_hash_mix(hash, *value));
    ASSERT_EQ(10897, hash);

    IVec_drop(&vec);
}

