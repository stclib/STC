#include <stdio.h>
#include <stc/cbitset.h>

int main() {
    cbitset_t set = cbitset_make(23, true);
    printf("count %zu, %zu\n", cbitset_count(set), set.size);
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
    cbitset_set_to(&set, 99, false);
    printf("%4zu: ", set.size);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_t s2 = cbitset_from(set);
    cbitset_flip_all(&s2);
    cbitset_set(&s2, 16);
    cbitset_set(&s2, 17);
    cbitset_set(&s2, 18);    
    printf(" new: ");
    for (int i=0; i<s2.size; ++i)
        printf("%d", cbitset_test(s2, i));
    puts("");

    printf(" xor: ");
    cbitset_set_xor(&set, s2);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_set_all(&set, false);
    printf("%4zu: ", set.size);
    for (int i=0; i<set.size; ++i)
        printf("%d", cbitset_test(set, i));
    puts("");
    
    cbitset_destroy(&s2);
    cbitset_destroy(&set);
}