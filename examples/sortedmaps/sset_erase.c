#include <stdio.h>

#define T ISet, int
#include <stc/sortedset.h>

int main(void)
{
    ISet set = c_make(ISet, {30, 20, 80, 40, 60, 90, 10, 70, 50});

    for (c_each(k, ISet, set))
        printf(" %d", *k.ref);
    puts("");

    int val = 64;
    ISet_iter it;
    printf("Show values >= %d:\n", val);
    it = ISet_lower_bound(&set, val);

    for (c_each(k, ISet, it, ISet_end(&set)))
        printf(" %d", *k.ref);
    puts("");

    printf("Erase values >= %d:\n", val);
    while (it.ref)
        it = ISet_erase_at(&set, it);

    for (c_each(k, ISet, set))
        printf(" %d", *k.ref);
    puts("");

    val = 40;
    printf("Erase values < %d:\n", val);
    it = ISet_lower_bound(&set, val);
    ISet_erase_range(&set, ISet_begin(&set), it);

    for (c_each(k, ISet, set))
        printf(" %d", *k.ref);
    puts("");

    ISet_drop(&set);
}
