// Example of intrusive/shared list-nodes by using the node API.

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
        printf("  inner:");
        c_foreach (i, Inner, inner)
            printf(" %d", *i.ref);

        printf("\n  outer:");
        c_foreach (i, Outer, outer)
            printf(" %d", i.ref->value);
        puts("");
}

int main()
{
    c_auto (Outer, outer)
    {
        Inner inner = Inner_init(); // do not destroy, outer will destroy the shared nodes.

        c_forlist (i, int, {6, 9, 3, 1, 7, 4, 5, 2, 8}) {
            Inner_node* node = Outer_push_back(&outer, (Inner_node){0});
            node->value = *i.ref;
        }

        c_foreach (i, Outer, outer)
            Inner_push_node_back(&inner, i.ref);

        printLists(inner, outer);

        puts("Sort inner");
        Inner_sort(&inner);

        printLists(inner, outer);

        puts("Remove odd numbers from inner list");

        c_foreach (i, Inner, inner)
            if (*i.ref & 1)
                Inner_unlink_node_after(&inner, i.prev);

        printLists(inner, outer);
    }
}
