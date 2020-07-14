#include "../stc/cvector.h"
#include "../stc/clist.h"
#include "../stc/carray.h"
#include "../stc/chash.h"
#include "../stc/cstring.h"


void stringdemo1()
{
    printf("\nSTRINGDEMO1\n");
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
    printf("\nVECTORDEMO1\n");
    CVector_ix bignums = cvector_init; // = (CVector_ix) cvector_init; if initializing after declaration.
    cvector_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<=100; ++i)
        cvector_ix_pushBack(&bignums, i * i * i);

    printf("erase - %d: %zu\n", 100, bignums.data[100]);
    cvector_ix_popBack(&bignums); // erase the last

    for (size_t i = 0; i < cvector_size(bignums); ++i) {
        if (i >= 90) printf("%zu: %zu\n", i, bignums.data[i]);
    }
    cvector_ix_destroy(&bignums);
}



declare_CVector(cs, CString, cstring_destroy, cstring_compare); // supply inline destructor of values

void vectordemo2()
{
    printf("\nVECTORDEMO2\n");
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
    printf("\nLISTDEMO1\n");
    CList_ix nums = clist_init, nums2 = clist_init;
    for (int i = 0; i < 10; ++i)
        clist_ix_pushBack(&nums, i);
    for (int i = 100; i < 110; ++i)
        clist_ix_pushBack(&nums2, i);
    c_foreach (i, clist_ix, nums)
        printf("value: %d\n", i.item->value);
    /* merge/append nums2 to nums */
    clist_ix_spliceAfter(&nums, clist_ix_last(&nums), &nums2);
    c_foreach (i, clist_ix, nums)
        printf("spliced: %d\n", i.item->value);

    *clist_ix_find(&nums, 100) *= 10;
    clist_ix_sort(&nums);                     // Sort the array
    clist_ix_remove(&nums, 105);
    clist_ix_popFront(&nums);
    clist_ix_pushFront(&nums, -99);
    c_foreach (i, clist_ix, nums)
        printf("sorted: %d\n", i.item->value);
    clist_ix_destroy(&nums);
}

declare_CHash_set(i, int); // alias: declare_CHash(i, int);

void setdemo1()
{
    printf("\nSETDEMO1\n");
    CHash_i nums = chash_init;
    chash_i_put(&nums, 8);
    chash_i_put(&nums, 11);

    c_foreach (i, chash_i, nums)
        printf("set: %d\n", i.item->key);
    chash_i_destroy(&nums);
}


declare_CHash(ii, int, int);

void mapdemo1()
{
    printf("\nMAPDEMO1\n");
    CHash_ii nums = chash_init;
    chash_ii_put(&nums, 8, 64);
    chash_ii_put(&nums, 11, 121);

    printf("get 8: %d\n", chash_ii_get(&nums, 8)->value);
    chash_ii_destroy(&nums);
}


declare_CHash_string(si, int); // Shorthand macro for the general declare_CHash expansion.

void mapdemo2()
{
    printf("\nMAPDEMO2\n");
    CHash_si nums = chash_init;
    chash_si_put(&nums, "Hello", 64);
    chash_si_put(&nums, "Groovy", 121);
    chash_si_put(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (chash_si_iter_t i = chash_si_begin(&nums); i.item; i = chash_si_next(i))
        printf("long: %s: %d\n", i.item->key.str, i.item->value);

    // or rather use the short form:
    c_foreach (i, chash_si, nums)
        printf("short: %s: %d\n", i.item->key.str, i.item->value);

    chash_si_destroy(&nums);
}


declare_CHash_string(ss, CString, cstring_destroy); 

void mapdemo3()
{
    printf("\nMAPDEMO3\n");
    CHash_ss table = chash_init;
    chash_ss_put(&table, "Map", cstring_make("test"));
    chash_ss_put(&table, "Make", cstring_make("my"));
    chash_ss_put(&table, "Sunny", cstring_make("day"));
    printf("remove: Make: %s\n", chash_ss_get(&table, "Make")->value.str);
    chash_ss_erase(&table, "Make");

    printf("size %zu\n", chash_size(table));
    c_foreach (i, chash_ss, table)
        printf("key: %s\n", i.item->key.str);
    chash_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}



declare_CArray(f, float);

void arraydemo1()
{
    printf("\nARRAYDEMO1\n");
    CArray3_f a3 = carray3_f_make(30, 20, 10, 0.f);
    carray3_f_data(a3, 5, 4)[3] = 10.2f;         // a3[5][4][3]
    CArray2_f a2 = carray3_f_at(a3, 5);          // sub-array reference (no data copy).

    printf("a3: %zu: (%zu, %zu, %zu) = %zu\n", sizeof(a3), carray3_xdim(a3), carray3_ydim(a3), carray3_zdim(a3), carray3_size(a3));
    printf("a2: %zu: (%zu, %zu) = %zu\n", sizeof(a2), carray2_xdim(a2), carray2_ydim(a2), carray2_size(a2));

    printf("%f\n", carray2_f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2_f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2_f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3_f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3_f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3_f_at2(a3, 5, 4).data[3]);

    carray2_f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3_f_destroy(&a3); // also invalidates a2.
}



int main()
{
    stringdemo1();
    vectordemo1();
    vectordemo2();
    listdemo1();
    setdemo1();
    mapdemo1();
    mapdemo2();
    mapdemo3();
    arraydemo1();    
}
