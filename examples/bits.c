#include <stdio.h>
#include <stc/cbits.h>

int main() 
{
    c_autovar (cbits set = cbits_with_size(23, true), cbits_drop(&set)) {
        printf("count %" PRIuMAX ", %" PRIuMAX "\n", cbits_count(set), set.size);
        cbits s1 = cbits_new("1110100110111");
        char buf[256];
        cbits_to_str(s1, buf, 0, -1);
        printf("buf: %s: %" PRIuMAX "\n", buf, cbits_count(s1));
        cbits_drop(&s1);

        cbits_reset(&set, 9);
        cbits_resize(&set, 43, false);
        c_autobuf (str, char, set.size + 1)
            printf(" str: %s\n", cbits_to_str(set, str, 0, -1));

        printf("%4" PRIuMAX ": ", set.size);
        c_forrange (i, set.size)
            printf("%d", cbits_test(set, i));
        puts("");

        cbits_set(&set, 28);
        cbits_resize(&set, 77, true);
        cbits_resize(&set, 93, false);
        cbits_resize(&set, 102, true);
        cbits_set_value(&set, 99, false);
        printf("%4" PRIuMAX ": ", set.size);
        c_forrange (i, set.size)
            printf("%d", cbits_test(set, i));

        puts("\nIterate:");
        printf("%4" PRIuMAX ": ", set.size);
        c_forrange (i, set.size)
            printf("%d", cbits_test(set, i));
        puts("");

        c_autovar (cbits s2 = cbits_clone(set), cbits_drop(&s2)) {
            cbits_flip_all(&s2);
            cbits_set(&s2, 16);
            cbits_set(&s2, 17);
            cbits_set(&s2, 18);
            printf(" new: ");
            c_forrange (i, s2.size)
                printf("%d", cbits_test(s2, i));
            puts("");

            printf(" xor: ");
            cbits_xor(&set, s2);
            c_forrange (i, set.size)
                printf("%d", cbits_test(set, i));
            puts("");

            cbits_set_all(&set, false);
            printf("%4" PRIuMAX ": ", set.size);
            c_forrange (i, set.size)
                printf("%d", cbits_test(set, i));
            puts("");
        }
    }
}
