#include "stc/cstring.h"
#include "stc/cvector.h"
#include "stc/clist.h"
#include "stc/cmap.h"


void stringdemo1()
{
    printf("STRINGDEMO1\n");
    CString cs = cstring_make("one-nine-three-seven-five");
    printf("%s.\n", cs.str);

    cstring_insert(&cs, 3, "-two");
    printf("%s.\n", cs.str);

    cstring_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cs.str);

    cstring_replace(&cs, 0, "seven", "four");
    printf("%s.\n", cs.str);
    cstring_take(&cs, cstring_makeFmt("%s *** %s", cs.str, cs.str));
    printf("%s.\n", cs.str);

    printf("find: %s\n", cs.str + cstring_find(cs, 0, "four"));

    // reassign:
    cstring_assign(&cs, "one two three four five six seven");
    cstring_append(&cs, " eight");
    printf("append: %s\n", cs.str);

    cstring_destroy(&cs);
}


declare_CVector(ix, int64_t); // ix is just an example tag name.

void vectordemo1()
{
    printf("VECTORDEMO1\n");
    CVector_ix bignums = cvector_init; // = (CVector_ix) cvector_init; if initializing after declaration.
    cvector_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<100; ++i)
        cvector_ix_pushBack(&bignums, i * i * i);
    cvector_ix_popBack(&bignums); // erase the last

    uint64_t value;
    for (size_t i = 0; i < cvector_size(bignums); ++i)
        value = bignums.data[i];
    cvector_ix_destroy(&bignums);
}



declare_CVector(cs, CString, cstring_destroy, cstring_compare); // supply inline destructor of values

void vectordemo2()
{
    printf("VECTORDEMO2\n");
    CVector_cs names = cvector_init;
    cvector_cs_pushBack(&names, cstring_make("Mary"));
    cvector_cs_pushBack(&names, cstring_make("Joe"));
    cvector_cs_pushBack(&names, cstring_make("Chris"));
    cstring_assign(&names.data[1], "Jane");      // replace Joe
    printf("names[1]: %s\n", names.data[1].str);

    cvector_cs_sort(&names);                     // Sort the array
    c_foreach (i, cvector_cs, names)
        printf("sorted: %s\n", i.item->str);
    cvector_cs_destroy(&names);
}

declare_CList(ix, int); 

void listdemo1()
{
    printf("LISTDEMO1\n");
    CList_ix nums = clist_init;
    clist_ix_pushBack(&nums, 123);
    clist_ix_pushBack(&nums, 231);
    clist_ix_pushBack(&nums, 444);
    clist_ix_pushBack(&nums, 321);
    *clist_ix_find(&nums, 231) = 1000;
    c_foreach (i, clist_ix, nums)
        printf("value: %d\n", i.item->value);    
    clist_ix_sort(&nums);                     // Sort the array
    clist_ix_remove(&nums, 123);
    c_foreach (i, clist_ix, nums)
        printf("sorted: %d\n", i.item->value);

    clist_ix_destroy(&nums);
}


declare_CMap(ii, int, int);

void mapdemo1()
{
    printf("MAPDEMO1\n");
    CMap_ii nums = cmap_init;
    cmap_ii_put(&nums, 8, 64);
    cmap_ii_put(&nums, 11, 121);

    printf("%d\n", cmap_ii_get(&nums, 8)->value);
    cmap_ii_destroy(&nums);
}



declare_CMap_stringkey(si, int); // Shorthand macro for the general declare_CMap expansion.

void mapdemo2()
{
    printf("MAPDEMO2\n");
    CMap_si nums = cmap_init;
    cmap_si_put(&nums, "Hello", 64);
    cmap_si_put(&nums, "Groovy", 121);
    cmap_si_put(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (cmap_si_iter_t i = cmap_si_begin(&nums); i.item; i = cmap_si_next(i))
        printf("long: %s: %d\n", i.item->key.str, i.item->value);

    // or rather use the short form:
    c_foreach (i, cmap_si, nums)
        printf("short: %s: %d\n", i.item->key.str, i.item->value);

    cmap_si_destroy(&nums);
}


declare_CMap_stringkey(ss, CString, cstring_destroy); 

void mapdemo3()
{
    printf("MAPDEMO3\n");
    CMap_ss table = cmap_init;
    cmap_ss_put(&table, "Map", cstring_make("test"));
    cmap_ss_put(&table, "Make", cstring_make("my"));
    cmap_ss_put(&table, "Sunny", cstring_make("day"));
    printf("remove: Make: %s\n", cmap_ss_get(&table, "Make")->value.str);
    cmap_ss_erase(&table, "Make");

    printf("size %llu\n", cmap_size(table));
    c_foreach (i, cmap_ss, table)
        printf("key: %s\n", i.item->key.str);
    cmap_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}



int main()
{
    stringdemo1();
    vectordemo1();
    vectordemo2();
    listdemo1();
    mapdemo1();
    mapdemo2();
    mapdemo3();
}
