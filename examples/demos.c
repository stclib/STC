#include <stc/cstr.h>

void stringdemo1()
{
    printf("\nSTRINGDEMO1\n");
    c_autovar (cstr cs = cstr_new("one-nine-three-seven-five"), cstr_drop(&cs))
    {
        printf("%s.\n", cstr_str(&cs));

        cstr_insert(&cs, 3, "-two");
        printf("%s.\n", cstr_str(&cs));

        cstr_erase_n(&cs, 7, 5); // -nine
        printf("%s.\n", cstr_str(&cs));

        cstr_replace(&cs, cstr_find(cs, "seven"), 5, "four");
        printf("%s.\n", cstr_str(&cs));

        cstr_take(&cs, cstr_from_fmt("%s *** %s", cstr_str(&cs), cstr_str(&cs)));
        printf("%s.\n", cstr_str(&cs));

        printf("find \"four\": %s\n", cstr_str(&cs) + cstr_find(cs, "four"));

        // reassign:
        cstr_assign(&cs, "one two three four five six seven");
        cstr_append(&cs, " eight");
        printf("append: %s\n", cstr_str(&cs));
    }
}

#define i_val int64_t
#define i_tag ix
#include <stc/cvec.h>

void vectordemo1()
{
    printf("\nVECTORDEMO1\n");
    c_autovar (cvec_ix bignums = cvec_ix_with_capacity(100), cvec_ix_drop(&bignums))
    {
        cvec_ix_reserve(&bignums, 100);
        for (size_t i = 10; i <= 100; i += 10)
            cvec_ix_push_back(&bignums, i * i);

        printf("erase - %d: %" PRIuMAX "\n", 3, bignums.data[3]);
        cvec_ix_erase_n(&bignums, 3, 1); // erase index 3

        cvec_ix_pop_back(&bignums);      // erase the last
        cvec_ix_erase_n(&bignums, 0, 1); // erase the first

        for (size_t i = 0; i < cvec_ix_size(bignums); ++i) {
            printf("%" PRIuMAX ": %" PRIuMAX "\n", i, bignums.data[i]);
        }
    }
}

#define i_val_str
#include <stc/cvec.h>

void vectordemo2()
{
    printf("\nVECTORDEMO2\n");
    c_auto (cvec_str, names) {
        cvec_str_emplace_back(&names, "Mary");
        cvec_str_emplace_back(&names, "Joe");
        cvec_str_emplace_back(&names, "Chris");
        cstr_assign(&names.data[1], "Jane");      // replace Joe
        printf("names[1]: %s\n", cstr_str(&names.data[1]));

        cvec_str_sort(&names);                     // Sort the array
        c_foreach (i, cvec_str, names)
            printf("sorted: %s\n", cstr_str(i.ref));
    }
}

#define i_val int
#define i_tag ix
#include <stc/clist.h>

void listdemo1()
{
    printf("\nLISTDEMO1\n");
    c_auto (clist_ix, nums, nums2)
    {
        for (int i = 0; i < 10; ++i)
            clist_ix_push_back(&nums, i);
        for (int i = 100; i < 110; ++i)
            clist_ix_push_back(&nums2, i);

        /* splice nums2 to front of nums */
        clist_ix_splice(&nums, clist_ix_begin(&nums), &nums2);
        c_foreach (i, clist_ix, nums)
            printf("spliced: %d\n", *i.ref);
        puts("");

        *clist_ix_find(&nums, 104).ref += 50;
        clist_ix_remove(&nums, 103);
        clist_ix_iter it = clist_ix_begin(&nums);
        clist_ix_erase_range(&nums, clist_ix_advance(it, 5), clist_ix_advance(it, 15));
        clist_ix_pop_front(&nums);
        clist_ix_push_back(&nums, -99);
        clist_ix_sort(&nums);

        c_foreach (i, clist_ix, nums)
            printf("sorted: %d\n", *i.ref);
    }
}

#define i_key int
#define i_tag i
#include <stc/cset.h>

void setdemo1()
{
    printf("\nSETDEMO1\n");
    cset_i nums = cset_i_init();
    cset_i_insert(&nums, 8);
    cset_i_insert(&nums, 11);

    c_foreach (i, cset_i, nums)
        printf("set: %d\n", *i.ref);
    cset_i_drop(&nums);
}

#define i_key int
#define i_val int
#define i_tag ii
#include <stc/cmap.h>

void mapdemo1()
{
    printf("\nMAPDEMO1\n");
    cmap_ii nums = cmap_ii_init();
    cmap_ii_insert(&nums, 8, 64);
    cmap_ii_insert(&nums, 11, 121);
    printf("val 8: %d\n", *cmap_ii_at(&nums, 8));
    cmap_ii_drop(&nums);
}

#define i_key_str
#define i_val int
#define i_tag si
#include <stc/cmap.h>

void mapdemo2()
{
    printf("\nMAPDEMO2\n");
    c_auto (cmap_si, nums)
    {
        cmap_si_emplace_or_assign(&nums, "Hello", 64);
        cmap_si_emplace_or_assign(&nums, "Groovy", 121);
        cmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

        // iterate the map:
        for (cmap_si_iter i = cmap_si_begin(&nums); i.ref != cmap_si_end(&nums).ref; cmap_si_next(&i))
            printf("long: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

        // or rather use the short form:
        c_foreach (i, cmap_si, nums)
            printf("short: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);
    }
}

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

void mapdemo3()
{
    printf("\nMAPDEMO3\n");
    cmap_str table = cmap_str_init();
    cmap_str_emplace(&table, "Map", "test");
    cmap_str_emplace(&table, "Make", "my");
    cmap_str_emplace(&table, "Sunny", "day");
    cmap_str_iter it = cmap_str_find(&table, "Make");
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    printf("size %" PRIuMAX ": remove: Make: %s\n", cmap_str_size(table), cstr_str(&it.ref->second));
    //cmap_str_erase(&table, "Make");
    cmap_str_erase_at(&table, it);

    printf("size %" PRIuMAX "\n", cmap_str_size(table));
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    cmap_str_drop(&table); // frees key and value cstrs, and hash table.
}

#define i_val float
#define i_tag f
#include <stc/carr3.h>

void arraydemo1()
{
    printf("\nARRAYDEMO1\n");
    c_autovar (carr3_f arr3 = carr3_f_with_values(30, 20, 10, 0.0f), 
                             carr3_f_drop(&arr3))
    {
        arr3.data[5][4][3] = 10.2f;
        float **arr2 = arr3.data[5];
        float *arr1 = arr3.data[5][4];

        printf("arr3: %" PRIuMAX ": (%" PRIuMAX ", %" PRIuMAX ", %" PRIuMAX ") = %" PRIuMAX "\n", sizeof(arr3), 
               arr3.xdim, arr3.ydim, arr3.zdim, carr3_f_size(arr3));

        printf("%g\n", arr1[3]); // = 10.2
        printf("%g\n", arr2[4][3]); // = 10.2
        printf("%g\n", arr3.data[5][4][3]); // = 10.2

        float x = 0.0;
        c_foreach (i, carr3_f, arr3)
            *i.ref = ++x;
        printf("%g\n", arr3.data[29][19][9]); // = 6000
    }
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
