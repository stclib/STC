// Example of clist using the node API.

#include <stdio.h> 

#define i_type List
#define i_val int
#include <stc/clist.h> 

void printList(List list) {
    printf("list:");
    c_foreach (i, List, list)
        printf(" %d", *i.ref);
    puts("");
}

int main() {
    List list = {0};
    c_forlist (i, int, {6, 9, 3, 1, 7, 4, 5, 2, 8})
        List_push_back_node(&list, c_new(List_node, {0, *i.ref}));

    printList(list);

    puts("Sort list");
    List_sort(&list);
    printList(list);

    puts("Remove nodes from list");
    while (!List_empty(&list))
        c_free(List_unlink_after_node(&list, list.last));

    printList(list);
}
