#include <stdio.h>

#define i_type IList, int
#include "stc/list.h"

void print_IList(const char* s, IList list)
{
    printf("%s", s);
    for (c_each(i, IList, list)) {
        printf(" %d", *i.ref);
    }
    puts("");
}

int main(void)
{
    IList list1 = c_make(IList, {1, 2, 3, 4, 5});
    IList list2 = c_make(IList, {10, 20, 30, 40, 50});

    print_IList("list1:", list1);
    print_IList("list2:", list2);

    IList_iter it = IList_advance(IList_begin(&list1), 2);
    it = IList_splice(&list1, it, &list2);

    puts("After splice");
    print_IList("list1:", list1);
    print_IList("list2:", list2);

    IList_splice_range(&list2, IList_begin(&list2), &list1, it, IList_end(&list1));

    puts("After splice_range");
    print_IList("list1:", list1);
    print_IList("list2:", list2);

    c_drop(IList, &list1, &list2);
}
