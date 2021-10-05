// erasing from clist
#include <stdio.h>

#define i_val int
#define i_tag i
#include <stc/clist.h>

int main ()
{
    c_autovar (clist_i L = clist_i_init(), clist_i_del(&L))
    {
        c_apply(clist_i, push_back, &L, {10, 20, 30, 40, 50});
                                                    // 10 20 30 40 50
        clist_i_iter_t it = clist_i_begin(&L);      // ^
        clist_i_next(&it);
        it = clist_i_erase_at(&L, it);              // 10 30 40 50
                                                    //    ^
        clist_i_iter_t end = clist_i_end(&L);       //
        clist_i_next(&it);
        it = clist_i_erase_range(&L, it, end);      // 10 30
                                                    //       ^
        printf("mylist contains:");
        c_foreach (x, clist_i, L) printf(" %d", *x.ref);
        puts("");
    }
}
