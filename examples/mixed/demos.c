#include <stc/cstr.h>

void stringdemo1(void)
{
    cstr cs = cstr_lit("one-nine-three-seven-five");
    printf("%s.\n", cstr_str(&cs));

    cstr_insert(&cs, 3, "-two");
    printf("%s.\n", cstr_str(&cs));

    cstr_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cstr_str(&cs));

    cstr_replace_nfirst(&cs, "seven", "four", 1);
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

#define T Vec64, long long
#include <stc/vec.h>

void vectordemo1(void)
{
    Vec64 bignums = Vec64_with_capacity(11);

    for (int i = 10; i <= 100; i += 10)
        Vec64_push(&bignums, i * i);

    printf("erase - %d: %lld\n", 3, bignums.data[3]);
    Vec64_erase_n(&bignums, 3, 1); // erase index 3

    Vec64_pop(&bignums);           // erase the last
    Vec64_erase_n(&bignums, 0, 1); // erase the first

    for (c_range(i, Vec64_size(&bignums))) {
        printf("%d: %lld\n", (int)i, bignums.data[i]);
    }

    Vec64_drop(&bignums);
}

#define T Strvec, cstr, (c_keypro | c_use_cmp)
#include <stc/vec.h>

void vectordemo2(void)
{
    Strvec names = {0};
    Strvec_emplace_back(&names, "Mary");
    Strvec_emplace_back(&names, "Joe");
    Strvec_emplace_back(&names, "Chris");

    cstr_assign(&names.data[1], "Jane"); // replace Joe
    printf("names[1]: %s\n", cstr_str(&names.data[1]));

    Strvec_sort(&names);               // Sort the array

    for (c_each(i, Strvec, names))
        printf("sorted: %s\n", cstr_str(i.ref));

    Strvec_drop(&names);
}

#define T Intlist, int, (c_use_cmp)
#include <stc/list.h>

void listdemo1(void)
{
    Intlist nums = {0}, nums2 = {0};
    for (int i = 0; i < 10; ++i)
        Intlist_push_back(&nums, i);
    for (int i = 100; i < 110; ++i)
        Intlist_push_back(&nums2, i);

    /* splice nums2 to front of nums */
    Intlist_splice(&nums, Intlist_begin(&nums), &nums2);
    for (c_each(i, Intlist, nums))
        printf("spliced: %d\n", *i.ref);
    puts("");

    *Intlist_find(&nums, 104).ref += 50;
    Intlist_remove(&nums, 103);

    Intlist_iter it = Intlist_begin(&nums);
    Intlist_erase_range(&nums, Intlist_advance(it, 5), Intlist_advance(it, 15));
    Intlist_pop_front(&nums);
    Intlist_push_back(&nums, 99);

    Intlist_sort(&nums);

    for (c_each(i, Intlist, nums))
        printf("sorted: %d\n", *i.ref);

    c_drop(Intlist, &nums, &nums2);
}

#define T hset_int, int
#include <stc/hashset.h>

void setdemo1(void)
{
    hset_int nums = {0};
    hset_int_insert(&nums, 8);
    hset_int_insert(&nums, 11);

    for (c_each(i, hset_int, nums))
        printf("set: %d\n", *i.ref);
    hset_int_drop(&nums);
}

#define T Intmap, int, int
#include <stc/hashmap.h>

void mapdemo1(void)
{
    Intmap nums = {0};
    Intmap_insert(&nums, 8, 64);
    Intmap_insert(&nums, 11, 121);

    printf("val 8: %d\n", *Intmap_at(&nums, 8));
    Intmap_drop(&nums);
}

#define T SImap, cstr, int, (c_keypro)
#include <stc/hashmap.h>

void mapdemo2(void)
{
    SImap nums = {0};
    SImap_put(&nums, "Hello", 64);
    SImap_put(&nums, "Groovy", 121);
    SImap_put(&nums, "Groovy", 200); // overwrite previous

    // iterate the map:
    for (SImap_iter i = SImap_begin(&nums); i.ref; SImap_next(&i))
        printf("long: %s: %d\n", cstr_str(&i.ref->first), i.ref->second);

    // or rather use the short form:
    for (c_each_kv(k, v, SImap, nums))
        printf("short: %s: %d\n", cstr_str(k), *v);

    SImap_drop(&nums);
}

#define T Strmap, cstr, cstr, (c_keypro|c_valpro)
#include <stc/hashmap.h>

void mapdemo3(void)
{
    Strmap table = {0};
    Strmap_emplace(&table, "Map", "test");
    Strmap_emplace(&table, "Make", "my");
    Strmap_emplace(&table, "Sunny", "day");

    Strmap_iter it = Strmap_find(&table, "Make");
    for (c_each_kv(k, v, Strmap, table))
        printf("entry: %s: %s\n", cstr_str(k), cstr_str(v));

    printf("size %d: remove: Make: %s\n", (int)Strmap_size(&table), cstr_str(&it.ref->second));
    //Strmap_erase(&table, "Make");
    Strmap_erase_at(&table, it);

    printf("size %d\n", (int)Strmap_size(&table));
    for (c_each_kv(k, v, Strmap, table))
        printf("entry: %s: %s\n", cstr_str(k), cstr_str(v));

    Strmap_drop(&table); // frees key and value cstrs, and hash table.
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
