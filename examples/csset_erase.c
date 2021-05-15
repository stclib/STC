#include <stc/csset.h>
#include <stdio.h>

using_csset(i, int);

int main()
{
    c_var (csset_i, set, {30, 20, 80, 40, 60, 90, 10, 70, 50});
    c_foreach (k, csset_i, set) printf(" %d", *k.ref); puts("");

    int val = 64;
    csset_i_iter_t it;
    printf("Show values >= %d:\n", val);
    it = csset_i_lower_bound(&set, val);
    c_foreach (k, csset_i, it, csset_i_end(&set)) printf(" %d", *k.ref); puts("");

    printf("Erase values >= %d:\n", val);
    while (it.ref) it = csset_i_erase_at(&set, it);
    c_foreach (k, csset_i, set) printf(" %d", *k.ref); puts("");

    val = 40;
    printf("Erase values < %d:\n", val);
    it = csset_i_lower_bound(&set, val);
    csset_i_erase_range(&set, csset_i_begin(&set), it);
    c_foreach (k, csset_i, set) printf(" %d", *k.ref); puts("");

    csset_i_del(&set);
}

