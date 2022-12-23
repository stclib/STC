// Example of intrusive list by using the node API and typesafe c_CONTAINER_OF().

#include <stdio.h> 

#define i_type List1
#define i_val int
#define i_extern // implement List1_sort()
#include <stc/clist.h> 

#define i_type List2
#define i_val List1_node
#define i_opt c_no_cmp // no elem. comparison
#include <stc/clist.h> 

int main()
{
    c_AUTO (List2, list2)
    {
        List1 list1 = List1_init(); // should not be destroyed, list2 will destroy shared nodes.

        c_FORLIST (i, int, {6, 9, 3, 1, 7, 4, 5, 2, 8})
            List2_push_back(&list2, (List1_node){NULL, *i.ref});

        c_FOREACH (i, List2, list2)
            List1_push_node_back(&list1, c_CONTAINER_OF(&i.ref->value, List1_node, value));

        printf("list1:");
        c_FOREACH (i, List1, list1) printf(" %d", *i.ref);
        printf("\nlist2:");
        c_FOREACH (i, List2, list2) printf(" %d", i.ref->value);

        printf("\nsort list1");
        List1_sort(&list1);

        printf("\nlist1:");
        c_FOREACH (i, List1, list1) printf(" %d", *i.ref);
        printf("\nlist2:");
        c_FOREACH (i, List2, list2) printf(" %d", i.ref->value);

        printf("\nremove 5 from both lists in O(1) time");
        List1_iter it1 = List1_find(&list1, 5);
        if (it1.ref) {
            List1_unlink_node_after(&list1, it1.prev);
            free(List2_unlink_node_after(&list2, c_CONTAINER_OF(it1.prev, List2_node, value)));
        }
        printf("\nlist1:");
        c_FOREACH (i, List1, list1) printf(" %d", *i.ref);
        printf("\nlist2:");
        c_FOREACH (i, List2, list2) printf(" %d", i.ref->value);
        puts("");
    }
}
