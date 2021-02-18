// erasing from clist
#include <stc/clist.h>
#include <stdio.h>

using_clist(i, int);

int main ()
{
    c_init (clist_i, L, {10, 20, 30, 40, 50});
    clist_i_iter_t end;
                                                 // 10 20 30 40 50
    clist_i_iter_t it = clist_i_begin(&L);       // ^
    it = clist_i_erase_after(&L, it);            // 10 30 40 50
                                                 //    ^
    end = clist_i_end(&L);                       //
    it = clist_i_erase_range_after(&L, it, end); // 10 30
                                                 //       ^

    printf("mylist contains:");
    c_foreach (x, clist_i, L) printf(" %d", *x.ref);
    puts("");

    clist_i_del(&L);
}