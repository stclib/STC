#include <stc/cstr.h>
#include <stdio.h>

int main()
{
    c_forvar (cstr ss = cstr_lit("The quick brown fox jumps over the lazy dog.JPG"), cstr_del(&ss)) {
        size_t pos = cstr_find_n(ss, "brown", 0, 5);
        printf("%zu [%s]\n", pos, pos == cstr_npos ? "<NULL>" : &ss.str[pos]);
        printf("iequals: %d\n", cstr_iequals(ss, "the quick brown fox jumps over the lazy dog.jpg"));
        printf("icontains: %d\n", cstr_icontains(ss, "uMPS Ove"));
        printf("ibegins_with: %d\n", cstr_ibegins_with(ss, "The Quick Brown"));
        printf("iends_with: %d\n", cstr_iends_with(ss, ".Jpg"));
        printf("ends_with: %d\n", cstr_ends_with(ss, ".JPG"));
    }
}