// Example of list using the node API.

#include <stdio.h>

#define i_TYPE List,int
#define i_use_cmp
#include "stc/list.h"

void printList(List list) {
    printf("list:");
    c_foreach (i, List, list)
        printf(" %d", *i.ref);
    puts("");
}

int main(void) {
    List list = {0};
    c_foritems (i, int, {6, 9, 3, 1, 7, 4, 5, 2, 8})
        List_push_back_node(&list, c_new(List_node, {.value=*i.ref}));

    printList(list);

    puts("Sort list");
    List_sort(&list);
    printList(list);

    puts("Remove nodes from list");
    while (!List_is_empty(&list))
        free(List_unlink_after_node(&list, list.last));

    printList(list);
}
