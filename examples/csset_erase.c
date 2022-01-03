#include <stdio.h>

#define i_key int
#include <stc/csset.h>

int main()
{
    c_auto (csset_int, set)
    {
        c_apply(v, csset_int_insert(&set, v),
            int, {30, 20, 80, 40, 60, 90, 10, 70, 50});
        c_foreach (k, csset_int, set)
            printf(" %d", *k.ref);
        puts("");

        int val = 64;
        csset_int_iter it;
        printf("Show values >= %d:\n", val);
        it = csset_int_lower_bound(&set, val);

        c_foreach (k, csset_int, it, csset_int_end(&set)) 
            printf(" %d", *k.ref);
        puts("");

        printf("Erase values >= %d:\n", val);
        while (it.ref != csset_int_end(&set).ref)
            it = csset_int_erase_at(&set, it);

        c_foreach (k, csset_int, set)
            printf(" %d", *k.ref);
        puts("");

        val = 40;
        printf("Erase values < %d:\n", val);
        it = csset_int_lower_bound(&set, val);
        csset_int_erase_range(&set, csset_int_begin(&set), it);

        c_foreach (k, csset_int, set)
            printf(" %d", *k.ref);
        puts("");
    }
}
