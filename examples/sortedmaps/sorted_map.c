// https://iq.opengenus.org/containers-cpp-stl/

#include <stdio.h>
#define T Mymap, int, int
#include <stc/sortedmap.h>

int main(void)
{
    Mymap gquiz1 = {0}, gquiz2 = {0};
    c_defer(
        Mymap_drop(&gquiz1),
        Mymap_drop(&gquiz2)
    ){
        // insert elements in random order
        Mymap_insert(&gquiz1, 2, 30);
        Mymap_insert(&gquiz1, 4, 20);
        Mymap_insert(&gquiz1, 7, 10);
        Mymap_insert(&gquiz1, 5, 50);
        Mymap_insert(&gquiz1, 3, 60);
        Mymap_insert(&gquiz1, 1, 40);
        Mymap_insert(&gquiz1, 6, 50);

        // printing map gquiz1
        printf("\nThe map gquiz1 is :\n\tKEY\tELEMENT\n");
        for (c_each_kv(k, v, Mymap, gquiz1))
            printf("\t%d\t%d\n", *k, *v);
        printf("\n");

        // assigning the elements from gquiz1 to gquiz2
        for (c_each_kv(k, v, Mymap, gquiz1))
            Mymap_insert(&gquiz2, *k, *v);

        // print all elements of the map gquiz2
        printf("\nThe map gquiz2 is :\n\tKEY\tELEMENT\n");
        for (c_each_kv(k, v, Mymap, gquiz2))
            printf("\t%d\t%d\n", *k, *v);
        printf("\n");

        // remove all elements up to element with key=3 in gquiz2
        printf("\ngquiz2 after removal of elements less than key=3 :\n");
        printf("\tKEY\tELEMENT\n");
        Mymap_erase_range(&gquiz2, Mymap_begin(&gquiz2),
                                   Mymap_find(&gquiz2, 3));
        for (c_each_kv(k, v, Mymap, gquiz2))
            printf("\t%d\t%d\n", *k, *v);
        printf("\n");

        // remove all elements with key = 4
        int num = Mymap_erase(&gquiz2, 4);
        printf("\ngquiz2.erase(4) : %d removed\n", num);
        printf("\tKEY\tELEMENT\n");
        for (c_each_kv(k, v, Mymap, gquiz2))
            printf("\t%d\t%d\n", *k, *v);
        printf("\n");

        // lower bound and upper bound for map gquiz1 key = 5
        printf("gquiz1.lower_bound(5) : ");
        printf("\tKEY = %d\t", Mymap_lower_bound(&gquiz1, 5).ref->first);
        printf("\tELEMENT = %d\n", Mymap_lower_bound(&gquiz1, 5).ref->second);
        printf("gquiz1.upper_bound(5) : ");
        printf("\tKEY = %d\t", Mymap_lower_bound(&gquiz1, 5+1).ref->first);
        printf("\tELEMENT = %d\n", Mymap_lower_bound(&gquiz1, 5+1).ref->second);
    }
}
