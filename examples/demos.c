#include <stc/cvec.h>
#include <stc/clist.h>
#include <stc/carray.h>
#include <stc/cmap.h>
#include <stc/cfmt.h>


void stringdemo1()
{
    c_print(1, "\nSTRINGDEMO1\n");
    cstr_t cs = cstr_from("one-nine-three-seven-five");
    c_print(1, "{}.\n", cs.str);

    cstr_insert(&cs, 3, "-two");
    c_print(1, "{}.\n", cs.str);

    cstr_erase(&cs, 7, 5); // -nine
    c_print(1, "{}.\n", cs.str);

    cstr_replace(&cs, cstr_find(&cs, "seven"), 5, "four");
    c_print(1, "{}.\n", cs.str);

    cstr_take(&cs, cstr_from_fmt("%s *** %s", cs.str, cs.str));
    c_print(1, "{}.\n", cs.str);

    c_print(1, "find \"four\": {}\n", cs.str + cstr_find(&cs, "four"));

    // reassign:
    cstr_assign(&cs, "one two three four five six seven");
    cstr_append(&cs, " eight");
    c_print(1, "append: {}\n", cs.str);

    cstr_del(&cs);
}


using_cvec(ix, int64_t); // ix is just an example tag name.

void vectordemo1()
{
    c_print(1, "\nVECTORDEMO1\n");
    cvec_ix bignums = cvec__init; // = (cvec_ix) cvec__init; if initializing after declaration.
    cvec_ix_reserve(&bignums, 100);
    c_forrange (i, 101)
        cvec_ix_push_back(&bignums, i * i * i);

    c_print(1, "erase - {}: {}\n", 100, bignums.data[100]);
    cvec_ix_pop_back(&bignums); // erase the last

    c_forrange (i, cvec_size(bignums)) {
        if (i >= 90) c_print(1, "{}: {}\n", i, bignums.data[i]);
    }
    cvec_ix_del(&bignums);
}



using_cvec_str();

void vectordemo2()
{
    c_print(1, "\nVECTORDEMO2\n");
    cvec_str names = cvec__init;
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cvec_str_emplace_back(&names, "Chris");
    cstr_assign(&names.data[1], "Jane");      // replace Joe
    c_print(1, "names[1]: {}\n", names.data[1].str);

    cvec_str_sort(&names);                     // Sort the array
    c_foreach (i, cvec_str, names)
        c_print(1, "sorted: {}\n", i.val->str);
    cvec_str_del(&names);
}

using_clist(ix, int);

void listdemo1()
{
    c_print(1, "\nLISTDEMO1\n");
    clist_ix nums = clist__init, nums2 = clist__init;
    c_forrange (i, 10)
        clist_ix_push_back(&nums, i);
    c_forrange (i, int, 100, 110)
        clist_ix_push_back(&nums2, i);
    c_foreach (i, clist_ix, nums)
        c_print(1, "value: {}\n", *i.val);
    /* merge/append nums2 to nums */
    clist_ix_splice_front(&nums, &nums2);
    c_foreach (i, clist_ix, nums)
        c_print(1, "spliced: {}\n", *i.val);

    *clist_ix_find(&nums, 100).val *= 10;
    clist_ix_sort(&nums);                     // Sort the array
    clist_ix_remove(&nums, 105);
    clist_ix_pop_front(&nums);
    clist_ix_push_front(&nums, -99);
    c_foreach (i, clist_ix, nums)
        c_print(1, "sorted: {}\n", *i.val);
    clist_ix_del(&nums);
}

using_cset(i, int);

void setdemo1()
{
    c_print(1, "\nSETDEMO1\n");
    cset_i nums = cset__init;
    cset_i_insert(&nums, 8);
    cset_i_insert(&nums, 11);

    c_foreach (i, cset_i, nums)
        c_print(1, "set: {}\n", *i.val);
    cset_i_del(&nums);
}


using_cmap(ii, int, int);

void mapdemo1()
{
    c_print(1, "\nMAPDEMO1\n");
    cmap_ii nums = cmap__init;
    cmap_ii_put(&nums, 8, 64);
    cmap_ii_put(&nums, 11, 121);
    c_print(1, "val 8: {}\n", *cmap_ii_at(&nums, 8));
    cmap_ii_del(&nums);
}


using_cmap_strkey(si, int); // Shorthand macro for the general using_cmap expansion.

void mapdemo2()
{
    c_print(1, "\nMAPDEMO2\n");
    cmap_si nums = cmap__init;
    cmap_si_put(&nums, "Hello", 64);
    cmap_si_put(&nums, "Groovy", 121);
    cmap_si_put(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (cmap_si_iter_t i = cmap_si_begin(&nums); i.val != cmap_si_end(&nums).val; cmap_si_next(&i))
        c_print(1, "long: {}: {}\n", i.val->first.str, i.val->second);

    // or rather use the short form:
    c_foreach (i, cmap_si, nums)
        c_print(1, "short: {}: {}\n", i.val->first.str, i.val->second);

    cmap_si_del(&nums);
}


using_cmap_str();

void mapdemo3()
{
    c_print(1, "\nMAPDEMO3\n");
    cmap_str table = cmap__init;
    cmap_str_put(&table, "Map", "test");
    cmap_str_put(&table, "Make", "my");
    cmap_str_put(&table, "Sunny", "day");
    cmap_str_value_t *e = cmap_str_find(&table, "Make");
    c_foreach (i, cmap_str, table)
        c_print(1, "entry: {}: {}\n", i.val->first.str, i.val->second.str);
    c_print(1, "size {}: remove: Make: {}\n", cmap_size(table), e->second.str);
    cmap_str_erase(&table, "Make");

    c_print(1, "size {}\n", cmap_size(table));
    c_foreach (i, cmap_str, table)
        c_print(1, "entry: {}: {}\n", i.val->first.str, i.val->second.str);
    cmap_str_del(&table); // frees key and value cstrs, and hash table.
}


using_carray(f, float);

void arraydemo1()
{
    c_print(1, "\nARRAYDEMO1\n");
    carray3f a3 = carray3f_init(30, 20, 10, 0.0f);
    *carray3f_at(&a3, 5, 4, 3) = 10.2f;    // a3[5][4][3]
    carray2f a2 = carray3f_at1(&a3, 5);    // sub-array reference: a2 = a3[5]
    carray1f a1 = carray3f_at2(&a3, 5, 4); // sub-array reference: a1 = a3[5][4]

    c_print(1, "a3: {}: ({}, {}, {}) = {}\n", sizeof(a3), carray3_xdim(a3), carray3_ydim(a3), carray3_zdim(a3), carray3_size(a3));
    c_print(1, "a2: {}: ({}, {}) = {}\n", sizeof(a2), carray2_xdim(a2), carray2_ydim(a2), carray2_size(a2));

    c_print(1, "{}\n", a1.data[3]);                 // lookup a1[3] (=10.2f)
    c_print(1, "{}\n", *carray2f_at(&a2, 4, 3));    // lookup a2[4][3] (=10.2f)
    c_print(1, "{}\n", *carray3f_at(&a3, 5, 4, 3)); // lookup a3[5][4][3] (=10.2f)

    c_foreach (i, carray3f, a3)
        *i.val = 1.0f;
    c_print(1, "{:.1f}\n", *carray3f_at(&a3, 29, 19, 9));

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
