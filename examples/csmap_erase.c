// map_erase.c
// From C++ example: https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-16
#include <stc/cstr.h>
#include <stdio.h>

#define i_key int
#define i_val_str
#define i_type mymap
#include <stc/csmap.h>

void printmap(mymap m)
{
    c_foreach (elem, mymap, m)
        printf(" [%d, %s]", elem.ref->first, cstr_str(&elem.ref->second));
    printf("\nsize() == %" PRIuMAX "\n\n", mymap_size(m));
}

int main()
{
    c_auto (mymap, m1)
    {
        // Fill in some data to test with, one at a time
        mymap_insert(&m1, 1, cstr_new("A"));
        mymap_insert(&m1, 2, cstr_new("B"));
        mymap_insert(&m1, 3, cstr_new("C"));
        mymap_insert(&m1, 4, cstr_new("D"));
        mymap_insert(&m1, 5, cstr_new("E"));

        puts("Starting data of map m1 is:");
        printmap(m1);
        // The 1st member function removes an element at a given position
        mymap_erase_at(&m1, mymap_advance(mymap_begin(&m1), 1));
        puts("After the 2nd element is deleted, the map m1 is:");
        printmap(m1);
    }

    c_auto (mymap, m2)
    {
        // Fill in some data to test with, one at a time, using c_apply()
        c_apply(v, mymap_emplace(&m2, c_pair(v)), mymap_raw, {
            {10, "Bob"},
            {11, "Rob"},
            {12, "Robert"},
            {13, "Bert"},
            {14, "Bobby"}
        });

        puts("Starting data of map m2 is:");
        printmap(m2);
        mymap_iter it1 = mymap_advance(mymap_begin(&m2), 1);
        mymap_iter it2 = mymap_find(&m2, mymap_back(&m2)->first);
        // The 2nd member function removes elements
        // in the range [First, Last)
        mymap_erase_range(&m2, it1, it2);
        puts("After the middle elements are deleted, the map m2 is:");
        printmap(m2);
    }

    c_auto (mymap, m3)
    {
        // Fill in some data to test with, one at a time, using emplace
        mymap_emplace(&m3, 1, "red");
        mymap_emplace(&m3, 2, "yellow");
        mymap_emplace(&m3, 3, "blue");
        mymap_emplace(&m3, 4, "green");
        mymap_emplace(&m3, 5, "orange");
        mymap_emplace(&m3, 6, "purple");
        mymap_emplace(&m3, 7, "pink");

        puts("Starting data of map m3 is:");
        printmap(m3);
        // The 3rd member function removes elements with a given Key
        size_t count = mymap_erase(&m3, 2);
        // The 3rd member function also returns the number of elements removed
        printf("The number of elements removed from m3 is: %" PRIuMAX "\n", count);
        puts("After the element with a key of 2 is deleted, the map m3 is:");
        printmap(m3);
    }
}
