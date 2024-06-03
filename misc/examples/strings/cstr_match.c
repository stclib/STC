#define i_implement
#include "stc/cstr.h"
#include "stc/csview.h"
#include <stdio.h>

int main(void)
{
    cstr ss = cstr_from("The quick brown fox jumps over the lazy dog.JPG");

    intptr_t pos = cstr_find_at(&ss, 0, "brown");
    printf("%" c_ZI " [%s]\n", pos, pos == c_NPOS ? "<NULL>" : cstr_str(&ss) + pos);
    printf("equals: %d\n", cstr_equals(&ss, "The quick brown fox jumps over the lazy dog.JPG"));
    printf("contains: %d\n", cstr_contains(&ss, "umps ove"));
    printf("starts_with: %d\n", cstr_starts_with(&ss, "The quick brown"));
    printf("ends_with: %d\n", cstr_ends_with(&ss, ".jpg"));
    printf("ends_with: %d\n", cstr_ends_with(&ss, ".JPG"));

    cstr s1 = cstr_lit("hell😀 w😀rl🐨");
    csview ch1 = cstr_u8_chr(&s1, 7);
    csview ch2 = cstr_u8_chr(&s1, 10);
    printf("%s\nsize: %" c_ZI ", %" c_ZI "\n", cstr_str(&s1), cstr_u8_size(&s1), cstr_size(&s1));
    printf("ch1: %.*s\n", c_SVARG(ch1));
    printf("ch2: %.*s\n", c_SVARG(ch2));

    c_drop(cstr, &ss, &s1);
}
