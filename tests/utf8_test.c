#include <stc/cstr.h>
#include <stc/utf8.h>
#include <stc/csview.h>
#include <stc/coroutine.h>
#include "ctest.h"

static uint32_t utf8_casefold_bruteforce(uint32_t c) {
    for (int i=0; i < casefold_len; ++i) {
        const struct CaseMapping entry = casemappings[i];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

static uint32_t utf8_tolower_bruteforce(uint32_t c) {
    for (int i=0; i < (int)(sizeof upcase_ind/sizeof *upcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

static uint32_t utf8_toupper_bruteforce(uint32_t c) {
    for (int i=0; i < (int)(sizeof lowcase_ind/sizeof *lowcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        if (c <= entry.m2) {
            int d = entry.m2 - entry.c2;
            if (c < (uint32_t)(entry.c1 + d)) return c;
            if (d == 1) return c - ((entry.m2 & 1U) == (c & 1U));
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

TEST(utf8, utf8_casefold)
{
    for (unsigned ch = 0; ch < 0x20000; ++ch)
        EXPECT_EQ(utf8_casefold(ch), utf8_casefold_bruteforce(ch));
}

TEST(utf8, utf8_tolower)
{
    for (unsigned ch = 0; ch < 0x20000; ++ch)
        EXPECT_EQ(utf8_tolower(ch), utf8_tolower_bruteforce(ch));
}

TEST(utf8, utf8_toupper)
{
    for (unsigned ch = 0; ch < 0x20000; ++ch)
        EXPECT_EQ(utf8_toupper(ch), utf8_toupper_bruteforce(ch));
}

#if 0
TEST(utf8, bench_utf8_casefold)
{
    long long t = cco_ticks();
    int repeats = 500;
    for (int i = 0; i < repeats; i++)
        for (unsigned ch = 1; ch < 65536; ++ch)
            EXPECT_TRUE(utf8_casefold(ch));
    printf("binary=%lldus\n", (cco_ticks() - t) / repeats);

    t = cco_ticks();
    repeats = 100;
    for (int i = 0; i < repeats; i++)
        for (unsigned ch = 1; ch < 65536; ++ch)
            EXPECT_TRUE(utf8_casefold_bruteforce(ch));
    printf("bruteforce=%lldus\n", (cco_ticks() - t) / repeats);
}
#endif
