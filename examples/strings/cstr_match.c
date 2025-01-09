#include "stc/cstr.h"
#include "stc/csview.h"
#include <stdio.h>

int main(void)
{
    cstr ss = cstr_lit("The quick brown fox jumps over the lazy dog.JPG");

    isize pos = cstr_find_at(&ss, 0, "brown");
    printf("%" c_ZI " [%s]\n", pos, pos == c_NPOS ? "<NULL>" : cstr_str(&ss) + pos);
    printf("equals: %d\n", cstr_equals(&ss, "The quick brown fox jumps over the lazy dog.JPG"));
    printf("contains: %d\n", cstr_contains(&ss, "umps ove"));
    printf("starts_with: %d\n", cstr_starts_with(&ss, "The quick brown"));
    printf("ends_with: %d\n", cstr_ends_with(&ss, ".jpg"));
    printf("ends_with: %d\n", cstr_ends_with(&ss, ".JPG"));

    cstr s1 = cstr_lit("hell😀 w😀rl🐨");
    csview ch1 = cstr_u8_at(&s1, 7).chr;
    csview ch2 = cstr_u8_at(&s1, 10).chr;
    printf("%s\nsize: %" c_ZI ", %" c_ZI "\n", cstr_str(&s1), cstr_u8_size(&s1), cstr_size(&s1));
    printf("ch1: " c_svfmt "\n", c_svarg(ch1));
    printf("ch2: " c_svfmt "\n", c_svarg(ch2));

    c_drop(cstr, &ss, &s1);
}
