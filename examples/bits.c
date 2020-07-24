#include <stdio.h>
#include <stc/cbitset.h>

int main() {
    CBitset set = cbitset_make(23, true);
    cbitset_reset(&set, 9);
    cbitset_resize(&set, 43, false);
    printf("%4zu: ", set.size);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_set(&set, 28);
    cbitset_resize(&set, 77, true);
    cbitset_resize(&set, 93, false);
    cbitset_resize(&set, 102, true);
    cbitset_setTo(&set, 99, false);
    printf("%4zu: ", set.size);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");

    CBitset s2 = cbitset_from(set);
    cbitset_flipAll(&s2);
    cbitset_set(&s2, 16);
    cbitset_set(&s2, 17);
    cbitset_set(&s2, 18);    
    printf(" new: ");
    for (int i=0; i<s2.size; ++i)
        printf("%d", cbitset_test(s2, i));
    puts("");

    printf(" xor: ");
    cbitset_setXor(&set, s2);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_setAll(&set, false);
    printf("%4zu: ", set.size);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");
    
    cbitset_destroy(&s2);
    cbitset_destroy(&set);
}