#include <stdio.h>
#include <stc/cbits.h>

int main() {
    cbits_t set = cbits_with_size(23, true);
    printf("count %zu, %zu\n", cbits_count(set), set.size);

    cbits_reset(&set, 9);
    cbits_resize(&set, 43, false);
    c_withbuffer (str, char, set.size + 1)
        printf(" str: %s\n", cbits_to_str(set, str, 0, -1));

    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbits_test(set, i));
    puts("");

    cbits_set(&set, 28);
    cbits_resize(&set, 77, true);
    cbits_resize(&set, 93, false);
    cbits_resize(&set, 102, true);
    cbits_set_value(&set, 99, false);
    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbits_test(set, i));

    puts("\nIterator:");
    printf("%4zu: ", set.size);
    c_foreach (i, cbits, set)
        printf("%d", cbits_itval(i));
    puts("");

    cbits_t s2 = cbits_clone(set);
    cbits_flip_all(&s2);
    cbits_set(&s2, 16);
    cbits_set(&s2, 17);
    cbits_set(&s2, 18);
    printf(" new: ");
    c_forrange (i, int, s2.size)
        printf("%d", cbits_test(s2, i));
    puts("");

    printf(" xor: ");
    cbits_xor_with(&set, s2);
    c_forrange (i, int, set.size)
        printf("%d", cbits_test(set, i));
    puts("");

    cbits_set_all(&set, false);
    printf("%4zu: ", set.size);
    c_forrange (i, int, set.size)
        printf("%d", cbits_test(set, i));
    puts("");

    cbits_del(&s2);
    cbits_del(&set);
}