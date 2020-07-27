#include <stc/cvec.h>
#include <stc/clist.h>
#include <stc/carray.h>
#include <stc/cmap.h>
#include <stc/cstr.h>


void stringdemo1()
{
    printf("\nSTRINGDEMO1\n");
    cstr_t cs = cstr_make("one-nine-three-seven-five");
    printf("%s.\n", cs.str);

    cstr_insert(&cs, 3, "-two");
    printf("%s.\n", cs.str);

    cstr_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cs.str);

    cstr_replace(&cs, 0, "seven", "four");
    printf("%s.\n", cs.str);
    cstr_take(&cs, cstr_from("%s *** %s", cs.str, cs.str));
    printf("%s.\n", cs.str);

    printf("find: %s\n", cs.str + cstr_find(cs, 0, "four"));

    // reassign:
    cstr_assign(&cs, "one two three four five six seven");
    cstr_append(&cs, " eight");
    printf("append: %s\n", cs.str);

    cstr_destroy(&cs);
}


declare_cvec(ix, int64_t); // ix is just an example tag name.

void vectordemo1()
{
    printf("\nVECTORDEMO1\n");
    cvec_ix bignums = cvec_init; // = (cvec_ix) cvec_init; if initializing after declaration.
    cvec_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<=100; ++i)
        cvec_ix_pushBack(&bignums, i * i * i);

    printf("erase - %d: %zu\n", 100, bignums.data[100]);
    cvec_ix_popBack(&bignums); // erase the last

    for (size_t i = 0; i < cvec_size(bignums); ++i) {
        if (i >= 90) printf("%zu: %zu\n", i, bignums.data[i]);
    }
    cvec_ix_destroy(&bignums);
}



declare_cvec(cs, cstr_t, cstr_destroy, cstr_compare); // supply inline destructor of values

void vectordemo2()
{
    printf("\nVECTORDEMO2\n");
    cvec_cs names = cvec_init;
    cvec_cs_pushBack(&names, cstr_make("Mary"));
    cvec_cs_pushBack(&names, cstr_make("Joe"));
    cvec_cs_pushBack(&names, cstr_make("Chris"));
    cstr_assign(&names.data[1], "Jane");      // replace Joe
    printf("names[1]: %s\n", names.data[1].str);

    cvec_cs_sort(&names);                     // Sort the array
    c_foreach (i, cvec_cs, names)
        printf("sorted: %s\n", i.item->str);
    cvec_cs_destroy(&names);
}

declare_clist(ix, int); 

void listdemo1()
{
    printf("\nLISTDEMO1\n");
    clist_ix nums = clist_init, nums2 = clist_init;
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

declare_cset(i, int);

void setdemo1()
{
    printf("\nSETDEMO1\n");
    cset_i nums = cset_init;
    cset_i_put(&nums, 8);
    cset_i_put(&nums, 11);

    c_foreach (i, cset_i, nums)
        printf("set: %d\n", i.item->key);
    cset_i_destroy(&nums);
}


declare_cmap(ii, int, int);

void mapdemo1()
{
    printf("\nMAPDEMO1\n");
    cmap_ii nums = cmap_init;
    cmap_ii_put(&nums, 8, 64);
    cmap_ii_put(&nums, 11, 121);

    printf("get 8: %d\n", cmap_ii_find(&nums, 8)->value);
    cmap_ii_destroy(&nums);
}


declare_cmap_str(si, int); // Shorthand macro for the general declare_cmap expansion.

void mapdemo2()
{
    printf("\nMAPDEMO2\n");
    cmap_si nums = cmap_init;
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


declare_cmap_str(ss, cstr_t, cstr_destroy); 

void mapdemo3()
{
    printf("\nMAPDEMO3\n");
    cmap_ss table = cmap_init;
    cmap_ss_put(&table, "Map", cstr_make("test"));
    cmap_ss_put(&table, "Make", cstr_make("my"));
    cmap_ss_put(&table, "Sunny", cstr_make("day"));
    cmapentry_ss *e = cmap_ss_find(&table, "Make");
    c_foreach (i, cmap_ss, table)
        printf("entry: %s: %s\n", i.item->key.str, i.item->value.str);
    printf("size %zu: remove: Make: %s\n", cmap_size(table), e->value.str);
    cmap_ss_erase(&table, "Make");
    //cmap_ss_eraseEntry(&table, e);

    printf("size %zu\n", cmap_size(table));
    c_foreach (i, cmap_ss, table)
        printf("entry: %s: %s\n", i.item->key.str, i.item->value.str);
    cmap_ss_destroy(&table); // frees key and value CStrs, and hash table (CVec).
}


declare_carray(f, float);

void arraydemo1()
{
    printf("\nARRAYDEMO1\n");
    carray3f a3 = carray3f_make(30, 20, 10, 0.f);
    carray3f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    carray2f a2 = carray3f_at(a3, 5);     // sub-array reference (no data copy).

    printf("a3: %zu: (%zu, %zu, %zu) = %zu\n", sizeof(a3), carray3_xdim(a3), carray3_ydim(a3), carray3_zdim(a3), carray3_size(a3));
    printf("a2: %zu: (%zu, %zu) = %zu\n", sizeof(a2), carray2_xdim(a2), carray2_ydim(a2), carray2_size(a2));

    printf("%f\n", carray2f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3f_at2(a3, 5, 4).data[3]);

    carray2f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3f_destroy(&a3); // also invalidates a2.
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
