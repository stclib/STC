#include <stdio.h>

#define i_key int
#include "stc/sset.h"

int main(void)
{
    sset_int set = c_make(sset_int, {30, 20, 80, 40, 60, 90, 10, 70, 50});

    for (c_each(k, sset_int, set))
        printf(" %d", *k.ref);
    puts("");

    int val = 64;
    sset_int_iter it;
    printf("Show values >= %d:\n", val);
    it = sset_int_lower_bound(&set, val);

    for (c_each(k, sset_int, it, sset_int_end(&set)))
        printf(" %d", *k.ref);
    puts("");

    printf("Erase values >= %d:\n", val);
    while (it.ref)
        it = sset_int_erase_at(&set, it);

    for (c_each(k, sset_int, set))
        printf(" %d", *k.ref);
    puts("");

    val = 40;
    printf("Erase values < %d:\n", val);
    it = sset_int_lower_bound(&set, val);
    sset_int_erase_range(&set, sset_int_begin(&set), it);

    for (c_each(k, sset_int, set))
        printf(" %d", *k.ref);
    puts("");

    sset_int_drop(&set);
}
