#include <stc/cvec.h>
#include <stc/clist.h>
#include <stc/carray.h>
#include <stc/cmap.h>
#include <stc/cstr.h>


void stringdemo1()
{
    printf("\nSTRINGDEMO1\n");
    cstr cs = cstr_from("one-nine-three-seven-five");
    printf("%s.\n", cs.str);

    cstr_insert(&cs, 3, "-two");
    printf("%s.\n", cs.str);

    cstr_erase_n(&cs, 7, 5); // -nine
    printf("%s.\n", cs.str);

    cstr_replace(&cs, cstr_find(cs, "seven"), 5, "four");
    printf("%s.\n", cs.str);

    cstr_take(&cs, cstr_from_fmt("%s *** %s", cs.str, cs.str));
    printf("%s.\n", cs.str);

    printf("find \"four\": %s\n", cs.str + cstr_find(cs, "four"));

    // reassign:
    cstr_assign(&cs, "one two three four five six seven");
    cstr_append(&cs, " eight");
    printf("append: %s\n", cs.str);

    cstr_del(&cs);
}


using_cvec(ix, int64_t); // ix is just an example tag name.

void vectordemo1()
{
    printf("\nVECTORDEMO1\n");
    cvec_ix bignums = cvec_ix_init();
    cvec_ix_reserve(&bignums, 100);
    for (size_t i = 10; i <= 100; i += 10)
        cvec_ix_push_back(&bignums, i * i);

    printf("erase - %d: %zu\n", 3, bignums.data[3]);
    cvec_ix_erase(&bignums, 3, 1); // erase index 3

    cvec_ix_pop_back(&bignums);      // erase the last
    cvec_ix_erase(&bignums, 0, 1); // erase the first

    for (size_t i = 0; i < cvec_ix_size(bignums); ++i) {
        printf("%zu: %zu\n", i, bignums.data[i]);
    }
    cvec_ix_del(&bignums);
}


using_cvec_str();

void vectordemo2()
{
    printf("\nVECTORDEMO2\n");
    cvec_str names = cvec_str_init();
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cvec_str_emplace_back(&names, "Chris");
    cstr_assign(&names.data[1], "Jane");      // replace Joe
    printf("names[1]: %s\n", names.data[1].str);

    cvec_str_sort(&names);                     // Sort the array
    c_foreach (i, cvec_str, names)
        printf("sorted: %s\n", i.ref->str);
    cvec_str_del(&names);
}

using_clist(ix, int);

void listdemo1()
{
    printf("\nLISTDEMO1\n");
    clist_ix nums = clist_ix_init(), nums2 = clist_ix_init();
    for (int i = 0; i < 10; ++i)
        clist_ix_push_back(&nums, i);
    for (int i = 100; i < 110; ++i)
        clist_ix_push_back(&nums2, i);
    c_foreach (i, clist_ix, nums)
        printf("value: %d\n", *i.ref);
    /* merge/append nums2 to nums */
    clist_ix_splice_front(&nums, &nums2);
    c_foreach (i, clist_ix, nums)
        printf("spliced: %d\n", *i.ref);

    *clist_ix_find(&nums, 100).ref *= 10;
    clist_ix_sort(&nums);                     // Sort the array
    clist_ix_remove(&nums, 105);
    clist_ix_pop_front(&nums);
    clist_ix_push_front(&nums, -99);
    c_foreach (i, clist_ix, nums)
        printf("sorted: %d\n", *i.ref);
    clist_ix_del(&nums);
}

using_cset(i, int);

void setdemo1()
{
    printf("\nSETDEMO1\n");
    cset_i nums = cset_i_init();
    cset_i_insert(&nums, 8);
    cset_i_insert(&nums, 11);

    c_foreach (i, cset_i, nums)
        printf("set: %d\n", *i.ref);
    cset_i_del(&nums);
}


using_cmap(ii, int, int);

void mapdemo1()
{
    printf("\nMAPDEMO1\n");
    cmap_ii nums = cmap_ii_init();
    cmap_ii_emplace(&nums, 8, 64);
    cmap_ii_emplace(&nums, 11, 121);
    printf("val 8: %d\n", *cmap_ii_at(&nums, 8));
    cmap_ii_del(&nums);
}


using_cmap_strkey(si, int); // Shorthand macro for the general using_cmap expansion.

void mapdemo2()
{
    printf("\nMAPDEMO2\n");
    cmap_si nums = cmap_si_init();
    cmap_si_emplace_or_assign(&nums, "Hello", 64);
    cmap_si_emplace_or_assign(&nums, "Groovy", 121);
    cmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (cmap_si_iter_t i = cmap_si_begin(&nums); i.ref != cmap_si_end(&nums).ref; cmap_si_next(&i))
        printf("long: %s: %d\n", i.ref->first.str, i.ref->second);

    // or rather use the short form:
    c_foreach (i, cmap_si, nums)
        printf("short: %s: %d\n", i.ref->first.str, i.ref->second);

    cmap_si_del(&nums);
}


using_cmap_str();

void mapdemo3()
{
    printf("\nMAPDEMO3\n");
    cmap_str table = cmap_str_init();
    cmap_str_emplace(&table, "Map", "test");
    cmap_str_emplace(&table, "Make", "my");
    cmap_str_emplace(&table, "Sunny", "day");
    cmap_str_iter_t it = cmap_str_find(&table, "Make");
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", i.ref->first.str, i.ref->second.str);
    printf("size %zu: remove: Make: %s\n", cmap_str_size(table), it.ref->second.str);
    //cmap_str_erase(&table, "Make");
    cmap_str_erase_at(&table, it);

    printf("size %zu\n", cmap_str_size(table));
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", i.ref->first.str, i.ref->second.str);
    cmap_str_del(&table); // frees key and value cstrs, and hash table.
}


using_carray3(f, float);

void arraydemo1()
{
    printf("\nARRAYDEMO1\n");
    carray3f a3 = carray3f_init(30, 20, 10, 0.0f);
    *carray3f_at(&a3, 5, 4, 3) = 10.2f;    // a3[5][4][3]
    carray2f a2 = carray3f_at1(&a3, 5);    // sub-array reference: a2 = a3[5]
    carray1f a1 = carray3f_at2(&a3, 5, 4); // sub-array reference: a1 = a3[5][4]

    printf("a3: %zu: (%zu, %zu, %zu) = %zu\n", sizeof(a3), carray3f_xdim(a3), carray3f_ydim(a3), carray3f_zdim(a3), carray3f_size(a3));
    printf("a2: %zu: (%zu, %zu) = %zu\n", sizeof(a2), carray2f_xdim(a2), carray2f_ydim(a2), carray2f_size(a2));

    printf("%f\n", a1.data[3]);                 // lookup a1[3] (=10.2f)
    printf("%f\n", *carray2f_at(&a2, 4, 3));    // lookup a2[4][3] (=10.2f)
    printf("%f\n", *carray3f_at(&a3, 5, 4, 3)); // lookup a3[5][4][3] (=10.2f)

    c_foreach (i, carray3f, a3)
        *i.ref = 1.0f;
    printf("%f\n", *carray3f_at(&a3, 29, 19, 9));

    carray2f_del(&a2); // does nothing, since it is a sub-array.
    carray3f_del(&a3); // also invalidates a2.
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
