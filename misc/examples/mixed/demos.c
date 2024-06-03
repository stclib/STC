#define i_implement
#include "stc/cstr.h"

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

#define i_TYPE vec_ll, long long
#include "stc/vec.h"

void vectordemo1(void)
{
    vec_ll bignums = vec_ll_with_capacity(100);
    vec_ll_reserve(&bignums, 100);
    for (int i = 10; i <= 100; i += 10)
        vec_ll_push(&bignums, i * i);

    printf("erase - %d: %lld\n", 3, bignums.data[3]);
    vec_ll_erase_n(&bignums, 3, 1); // erase index 3

    vec_ll_pop(&bignums);           // erase the last
    vec_ll_erase_n(&bignums, 0, 1); // erase the first

    for (int i = 0; i < vec_ll_size(&bignums); ++i) {
        printf("%d: %lld\n", i, bignums.data[i]);
    }

    vec_ll_drop(&bignums);
}

#define i_key_str
#define i_use_cmp
#include "stc/vec.h"

void vectordemo2(void)
{
    vec_str names = {0};
    vec_str_emplace_back(&names, "Mary");
    vec_str_emplace_back(&names, "Joe");
    vec_str_emplace_back(&names, "Chris");
    cstr_assign(&names.data[1], "Jane"); // replace Joe
    printf("names[1]: %s\n", cstr_str(&names.data[1]));

    vec_str_sort(&names);               // Sort the array

    c_foreach (i, vec_str, names)
        printf("sorted: %s\n", cstr_str(i.ref));

    vec_str_drop(&names);
}

#define i_key int
#define i_use_cmp
#include "stc/list.h"

void listdemo1(void)
{
    list_int nums = {0}, nums2 = {0};
    for (int i = 0; i < 10; ++i)
        list_int_push_back(&nums, i);
    for (int i = 100; i < 110; ++i)
        list_int_push_back(&nums2, i);

    /* splice nums2 to front of nums */
    list_int_splice(&nums, list_int_begin(&nums), &nums2);
    c_foreach (i, list_int, nums)
        printf("spliced: %d\n", *i.ref);
    puts("");

    *list_int_find(&nums, 104).ref += 50;
    list_int_remove(&nums, 103);
    list_int_iter it = list_int_begin(&nums);
    list_int_erase_range(&nums, list_int_advance(it, 5), list_int_advance(it, 15));
    list_int_pop_front(&nums);
    list_int_push_back(&nums, -99);
    list_int_sort(&nums);

    c_foreach (i, list_int, nums)
        printf("sorted: %d\n", *i.ref);

    c_drop(list_int, &nums, &nums2);
}

#define i_key int
#include "stc/hset.h"

void setdemo1(void)
{
    hset_int nums = {0};
    hset_int_insert(&nums, 8);
    hset_int_insert(&nums, 11);

    c_foreach (i, hset_int, nums)
        printf("set: %d\n", *i.ref);
    hset_int_drop(&nums);
}

#define i_TYPE hmap_ii,int,int
#include "stc/hmap.h"

void mapdemo1(void)
{
    hmap_ii nums = {0};
    hmap_ii_insert(&nums, 8, 64);
    hmap_ii_insert(&nums, 11, 121);
    printf("val 8: %d\n", *hmap_ii_at(&nums, 8));
    hmap_ii_drop(&nums);
}

#define i_type hmap_si
#define i_key_str
#define i_val int
#include "stc/hmap.h"

void mapdemo2(void)
{
    hmap_si nums = {0};
    hmap_si_emplace_or_assign(&nums, "Hello", 64);
    hmap_si_emplace_or_assign(&nums, "Groovy", 121);
    hmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (hmap_si_iter i = hmap_si_begin(&nums); i.ref; hmap_si_next(&i))
        printf("long: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    // or rather use the short form:
    c_foreach (i, hmap_si, nums)
        printf("short: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    hmap_si_drop(&nums);
}

#define i_key_str
#define i_val_str
#include "stc/hmap.h"

void mapdemo3(void)
{
    hmap_str table = {0};
    hmap_str_emplace(&table, "Map", "test");
    hmap_str_emplace(&table, "Make", "my");
    hmap_str_emplace(&table, "Sunny", "day");
    hmap_str_iter it = hmap_str_find(&table, "Make");
    c_foreach (i, hmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    printf("size %" c_ZI ": remove: Make: %s\n", hmap_str_size(&table), cstr_str(&it.ref->second));
    //hmap_str_erase(&table, "Make");
    hmap_str_erase_at(&table, it);

    printf("size %" c_ZI "\n", hmap_str_size(&table));
    c_foreach (i, hmap_str, table)
        printf("entry: %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));

    hmap_str_drop(&table); // frees key and value cstrs, and hash table.
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
