#define i_implement
#include <stc/cstr.h>

void stringdemo1(void)
{
    cstr cs = cstr_lit("one-nine-three-seven-five");
    printf("%s.\n", cstr_str(&cs));

    cstr_insert(&cs, 3, "-two");
    printf("%s.\n", cstr_str(&cs));

    cstr_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cstr_str(&cs));

    cstr_replace(&cs, "seven", "four", 1);
    printf("%s.\n", cstr_str(&cs));

    cstr_take(&cs, cstr_from_fmt("%s *** %s", cstr_str(&cs), cstr_str(&cs)));
    printf("%s.\n", cstr_str(&cs));

    printf("find \"four\": %s\n", cstr_str(&cs) + cstr_find(&cs, "four"));

    // reassign:
    cstr_assign(&cs, "one two three four five six seven");
    cstr_append(&cs, " eight");
    printf("append: %s\n", cstr_str(&cs));

    cstr_drop(&cs);
}

#define i_key long long
#define i_tag ix
#include <stc/cvec.h>

void vectordemo1(void)
{
    cvec_ix bignums = cvec_ix_with_capacity(100);
    cvec_ix_reserve(&bignums, 100);
    for (int i = 10; i <= 100; i += 10)
        cvec_ix_push(&bignums, i * i);

    printf("erase - %d: %lld\n", 3, bignums.data[3]);
    cvec_ix_erase_n(&bignums, 3, 1); // erase index 3

    cvec_ix_pop(&bignums);           // erase the last
    cvec_ix_erase_n(&bignums, 0, 1); // erase the first

    for (int i = 0; i < cvec_ix_size(&bignums); ++i) {
        printf("%d: %lld\n", i, bignums.data[i]);
    }

    cvec_ix_drop(&bignums);
}

#define i_key_str
#define i_use_cmp
#include <stc/cvec.h>

void vectordemo2(void)
{
    cvec_str names = {0};
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cvec_str_emplace_back(&names, "Chris");
    cstr_assign(&names.data[1], "Jane"); // replace Joe
    printf("names[1]: %s\n", cstr_str(&names.data[1]));

    cvec_str_sort(&names);               // Sort the array
    
    c_foreach (i, cvec_str, names)
        printf("sorted: %s\n", cstr_str(i.ref));

    cvec_str_drop(&names);
}

#define i_key int
#define i_tag ix
#define i_use_cmp
#include <stc/clist.h>

void listdemo1(void)
{
    clist_ix nums = {0}, nums2 = {0};
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

    c_drop(clist_ix, &nums, &nums2);
}

#define i_key int
#define i_tag i
#include <stc/cset.h>

void setdemo1(void)
{
    cset_i nums = {0};
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

void mapdemo1(void)
{
    cmap_ii nums = {0};
    cmap_ii_insert(&nums, 8, 64);
    cmap_ii_insert(&nums, 11, 121);
    printf("val 8: %d\n", *cmap_ii_at(&nums, 8));
    cmap_ii_drop(&nums);
}

#define i_key_str
#define i_val int
#define i_tag si
#include <stc/cmap.h>

void mapdemo2(void)
{
    cmap_si nums = {0};
    cmap_si_emplace_or_assign(&nums, "Hello", 64);
    cmap_si_emplace_or_assign(&nums, "Groovy", 121);
    cmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (cmap_si_iter i = cmap_si_begin(&nums); i.ref; cmap_si_next(&i))
        printf("long: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    // or rather use the short form:
    c_foreach (i, cmap_si, nums)
        printf("short: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    cmap_si_drop(&nums);
}

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

void mapdemo3(void)
{
    cmap_str table = {0};
    cmap_str_emplace(&table, "Map", "test");
    cmap_str_emplace(&table, "Make", "my");
    cmap_str_emplace(&table, "Sunny", "day");
    cmap_str_iter it = cmap_str_find(&table, "Make");
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    printf("size %" c_ZI ": remove: Make: %s\n", cmap_str_size(&table), cstr_str(&it.ref->second));
    //cmap_str_erase(&table, "Make");
    cmap_str_erase_at(&table, it);

    printf("size %" c_ZI "\n", cmap_str_size(&table));
    c_foreach (i, cmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    
    cmap_str_drop(&table); // frees key and value cstrs, and hash table.
}

int main(void)
{
    printf("\nSTRINGDEMO1\n"); stringdemo1();
    printf("\nVECTORDEMO1\n"); vectordemo1();
    printf("\nVECTORDEMO2\n"); vectordemo2();
    printf("\nLISTDEMO1\n"); listdemo1();
    printf("\nSETDEMO1\n"); setdemo1();
    printf("\nMAPDEMO1\n"); mapdemo1();
    printf("\nMAPDEMO2\n"); mapdemo2();
    printf("\nMAPDEMO3\n"); mapdemo3();
}
