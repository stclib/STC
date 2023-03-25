// erasing from clist
#include <stdio.h>

#define i_type IList
#define i_val int
#include <stc/clist.h>

int main ()
{
    IList L = c_make(IList, {10, 20, 30, 40, 50});

    c_foreach (x, IList, L)
        printf("%d ", *x.ref);
    puts("");
                                          // 10 20 30 40 50
    IList_iter it = IList_begin(&L);      // ^
    IList_next(&it);
    it = IList_erase_at(&L, it);          // 10 30 40 50
                                          //    ^
    IList_iter end = IList_end(&L);       //
    IList_next(&it);
    it = IList_erase_range(&L, it, end);  // 10 30
                                          //       ^
    printf("list contains:");
    c_foreach (x, IList, L)
        printf(" %d", *x.ref);
    puts("");

    IList_drop(&L);
}
