// map_erase.c
// From C++ example: https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-16
#include <stc/csmap.h>
#include <stc/cstr.h>
#include <stdio.h>

using_csmap_strval(my, int);

void printmap(csmap_my map)
{
    c_foreach (e, csmap_my, map)
        printf(" [%d, %s]", e.ref->first, e.ref->second.str);
    printf("\nsize() == %zu\n\n", csmap_my_size(map));
}

int main()
{
    c_forauto (csmap_my, m1)
    {
        // Fill in some data to test with, one at a time
        csmap_my_insert(&m1, 1, cstr_lit("A"));
        csmap_my_insert(&m1, 2, cstr_lit("B"));
        csmap_my_insert(&m1, 3, cstr_lit("C"));
        csmap_my_insert(&m1, 4, cstr_lit("D"));
        csmap_my_insert(&m1, 5, cstr_lit("E"));

        puts("Starting data of map m1 is:");
        printmap(m1);
        // The 1st member function removes an element at a given position
        csmap_my_erase_at(&m1, csmap_my_fwd(csmap_my_begin(&m1), 1));
        puts("After the 2nd element is deleted, the map m1 is:");
        printmap(m1);
    }

    c_forauto (csmap_my, m2)
    {
        // Fill in some data to test with, one at a time, using c_emplace()
        c_emplace(csmap_my, m2, {
            {10, "Bob"},
            {11, "Rob"},
            {12, "Robert"},
            {13, "Bert"},
            {14, "Bobby"}
        });

        puts("Starting data of map m2 is:");
        printmap(m2);
        csmap_my_iter_t it1 = csmap_my_fwd(csmap_my_begin(&m2), 1);
        csmap_my_iter_t it2 = csmap_my_find(&m2, csmap_my_back(&m2)->first);
        // The 2nd member function removes elements
        // in the range [First, Last)
        csmap_my_erase_range(&m2, it1, it2);
        puts("After the middle elements are deleted, the map m2 is:");
        printmap(m2);
    }

    c_forauto (csmap_my, m3)
    {
        // Fill in some data to test with, one at a time, using emplace
        csmap_my_emplace(&m3, 1, "red");
        csmap_my_emplace(&m3, 2, "yellow");
        csmap_my_emplace(&m3, 3, "blue");
        csmap_my_emplace(&m3, 4, "green");
        csmap_my_emplace(&m3, 5, "orange");
        csmap_my_emplace(&m3, 6, "purple");
        csmap_my_emplace(&m3, 7, "pink");

        puts("Starting data of map m3 is:");
        printmap(m3);
        // The 3rd member function removes elements with a given Key
        size_t count = csmap_my_erase(&m3, 2);
        // The 3rd member function also returns the number of elements removed
        printf("The number of elements removed from m3 is: %zu\n", count);
        puts("After the element with a key of 2 is deleted, the map m3 is:");
        printmap(m3);
    }
}
