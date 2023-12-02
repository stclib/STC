#include <stdio.h>

#define i_key int
#define i_tag i
#include "stc/list.h"

void print_ilist(const char* s, list_i list)
{
    printf("%s", s);
    c_foreach (i, list_i, list) {
        printf(" %d", *i.ref);
    }
    puts("");
}

int main(void)
{
    list_i list1 = c_init(list_i, {1, 2, 3, 4, 5});
    list_i list2 = c_init(list_i, {10, 20, 30, 40, 50});

    print_ilist("list1:", list1);
    print_ilist("list2:", list2);

    list_i_iter it = list_i_advance(list_i_begin(&list1), 2);
    it = list_i_splice(&list1, it, &list2);

    puts("After splice");
    print_ilist("list1:", list1);
    print_ilist("list2:", list2);

    list_i_splice_range(&list2, list_i_begin(&list2), &list1, it, list_i_end(&list1));

    puts("After splice_range");
    print_ilist("list1:", list1);
    print_ilist("list2:", list2);

    c_drop(list_i, &list1, &list2);
}
