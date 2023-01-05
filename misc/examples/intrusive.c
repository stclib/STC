// Example of intrusive list by using the node API.

#include <stdio.h> 

#define i_type Inner
#define i_val int
#define i_extern // implement Inner_sort()
#include <stc/clist.h> 

#define i_type Outer
#define i_val Inner_node
#define i_opt c_no_cmp // no elem. comparison
#include <stc/clist.h>

void printLists(Inner inner, Outer outer) {
        printf("inner:");
        c_FOREACH (i, Inner, inner)
            printf(" %d", *i.ref);

        printf("\nouter:");
        c_FOREACH (i, Outer, outer)
            printf(" %d", i.ref->value);
        puts("");
}

int main()
{
    c_AUTO (Outer, outer)
    {
        Inner inner = Inner_init(); // do not destroy, outer will destroy the shared nodes.

        c_FORLIST (i, int, {6, 9, 3, 1, 7, 4, 5, 2, 8})
            Outer_push_back(&outer, (Inner_node){NULL, *i.ref});

        c_FOREACH (i, Outer, outer)
            Inner_push_node_back(&inner, Inner_get_node(&i.ref->value));

        printLists(inner, outer);

        puts("sort inner");
        Inner_sort(&inner);

        printLists(inner, outer);

        puts("remove 5 from both lists in O(1) time");
        Inner_iter it1 = Inner_find(&inner, 5);

        if (it1.ref) {
            Inner_unlink_node_after(&inner, it1.prev);
            free(Outer_unlink_node_after(&outer, Outer_get_node(it1.prev)));
        }

        printLists(inner, outer);
    }
}
