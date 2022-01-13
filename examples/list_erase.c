// erasing from clist
#include <stdio.h>

#define i_val int
#include <stc/clist.h>

int main ()
{
    c_auto (clist_int, L)
    {
        c_apply(i, clist_int_push_back(&L, i), int, {10, 20, 30, 40, 50});
        c_foreach (x, clist_int, L)
            printf("%d ", *x.ref);
        puts("");
                                                    // 10 20 30 40 50
        clist_int_iter it = clist_int_begin(&L);    // ^
        clist_int_next(&it);
        it = clist_int_erase_at(&L, it);            // 10 30 40 50
                                                    //    ^
        clist_int_iter end = clist_int_end(&L);     //
        clist_int_next(&it);
        it = clist_int_erase_range(&L, it, end);    // 10 30
                                                    //       ^
        printf("list contains:");
        c_foreach (x, clist_int, L)
            printf(" %d", *x.ref);
        puts("");
    }
}
