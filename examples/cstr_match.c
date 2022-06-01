#include <stc/cstr.h>
#include <stc/csview.h>
#include <stdio.h>

int main()
{
    c_autovar (cstr ss = cstr_new("The quick brown fox jumps over the lazy dog.JPG"), cstr_drop(&ss)) {
        size_t pos = cstr_find_from(ss, 0, "brown");
        printf("%" PRIuMAX " [%s]\n", pos, pos == cstr_npos ? "<NULL>" : cstr_str(&ss) + pos);
        printf("equals: %d\n", cstr_equals(ss, "The quick brown fox jumps over the lazy dog.JPG"));
        printf("contains: %d\n", cstr_contains(ss, "umps ove"));
        printf("starts_with: %d\n", cstr_starts_with(ss, "The quick brown"));
        printf("ends_with: %d\n", cstr_ends_with(ss, ".jpg"));
        printf("ends_with: %d\n", cstr_ends_with(ss, ".JPG"));

        cstr s1 = cstr_new("hell😀 w😀rl🐨");
        csview ch1 = cstr_at(&s1, 10);
        csview ch2 = cstr_at_u8(&s1, 10);
        printf("ch1: %" c_PRIsv "\n", c_ARGsv(ch1));
        printf("ch2: %" c_PRIsv "\n", c_ARGsv(ch2));
    }
}
