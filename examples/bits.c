#include <stdio.h>
#include <stc/cbitset.h>

int main() {
    cbitset_t set = cbitset_with_size(23, true);
    printf("count %zu, %zu\n", cbitset_count(set), set.size);

    cbitset_reset(&set, 9);
    cbitset_resize(&set, 43, false);
    cstr_t str = cbitset_to_str(set);
    printf(" str: %s\n", str.str);
    cstr_del(&str);

    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_set(&set, 28);
    cbitset_resize(&set, 77, true);
    cbitset_resize(&set, 93, false);
    cbitset_resize(&set, 102, true);
    cbitset_set_value(&set, 99, false);
    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbitset_test(set, i));

    puts("\nIterator:");
    printf("%4zu: ", set.size);
    c_foreach (i, cbitset, set)
        printf("%d", cbitset_itval(i));
    puts("");        

    cbitset_t s2 = cbitset_clone(set);
    cbitset_flip_all(&s2);
    cbitset_set(&s2, 16);
    cbitset_set(&s2, 17);
    cbitset_set(&s2, 18);    
    printf(" new: ");
    c_forrange (i, int, s2.size)
        printf("%d", cbitset_test(s2, i));
    puts("");

    printf(" xor: ");
    cbitset_xor_with(&set, s2);
    c_forrange (i, int, set.size)
        printf("%d", cbitset_test(set, i));
    puts("");

    cbitset_set_all(&set, false);
    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbitset_test(set, i));
    puts("");
    
    cbitset_del(&s2);
    cbitset_del(&set);
}