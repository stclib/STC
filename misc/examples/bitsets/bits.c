#include <stdio.h>
#include <stc/cbits.h>

int main(void)
{
    cbits set = cbits_with_size(23, true);
    cbits s2;
    c_defer(
        cbits_drop(&set),
        cbits_drop(&s2)
    ){
        printf("count %lld, %lld\n", cbits_count(&set), cbits_size(&set));
        cbits s1 = cbits_from("1110100110111");
        char buf[256];
        cbits_to_str(&s1, buf, 0, 255);
        printf("buf: %s: %lld\n", buf, cbits_count(&s1));
        cbits_drop(&s1);

        cbits_reset(&set, 9);
        cbits_resize(&set, 43, false);
        printf(" str: %s\n", cbits_to_str(&set, buf, 0, 255));

        printf("%4lld: ", cbits_size(&set));
        c_forrange (i, cbits_size(&set))
            printf("%d", cbits_test(&set, i));
        puts("");

        cbits_set(&set, 28);
        cbits_resize(&set, 77, true);
        cbits_resize(&set, 93, false);
        cbits_resize(&set, 102, true);
        cbits_set_value(&set, 99, false);
        printf("%4lld: ", cbits_size(&set));
        c_forrange (i, cbits_size(&set))
            printf("%d", cbits_test(&set, i));

        puts("\nIterate:");
        printf("%4lld: ", cbits_size(&set));
        c_forrange (i, cbits_size(&set))
            printf("%d", cbits_test(&set, i));
        puts("");

        // Make a clone
        s2 = cbits_clone(set);
        cbits_flip_all(&s2);
        cbits_set(&s2, 16);
        cbits_set(&s2, 17);
        cbits_set(&s2, 18);
        printf(" new: ");
        c_forrange (i, cbits_size(&s2))
            printf("%d", cbits_test(&s2, i));
        puts("");

        printf(" xor: ");
        cbits_xor(&set, &s2);
        c_forrange (i, cbits_size(&set))
            printf("%d", cbits_test(&set, i));
        puts("");

        cbits_set_all(&set, false);
        printf("%4lld: ", cbits_size(&set));
        c_forrange (i, cbits_size(&set))
            printf("%d", cbits_test(&set, i));
        puts("");
    }
}
