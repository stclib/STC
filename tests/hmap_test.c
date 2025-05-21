#include <stdio.h>
#include <stc/cstr.h>
#include "ctest.h"

#define T Intmap, int, int
#include <stc/hashmap.h>

TEST(hashmap, mapdemo1)
{
    Intmap nums = {0};
    Intmap_insert(&nums, 8, 64);
    Intmap_insert(&nums, 11, 121);
    EXPECT_EQ(64, *Intmap_at(&nums, 8));
    Intmap_drop(&nums);
}

#define T SImap, cstr, int, (c_keypro)
#include <stc/hashmap.h>

TEST(hashmap, mapdemo2)
{
    SImap nums = {0};
    SImap_clear(&nums);
    SImap_emplace_or_assign(&nums, "Hello", 64);
    SImap_emplace_or_assign(&nums, "Groovy", 121);
    SImap_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    EXPECT_EQ(200, *SImap_at(&nums, "Groovy"));
    EXPECT_EQ(64, *SImap_at(&nums, "Hello"));

    SImap_drop(&nums);
}


#define T Strmap, cstr, cstr, (c_keypro | c_valpro)
#include <stc/hashmap.h>

TEST(hashmap, mapdemo3)
{
    Strmap map = {0};
    Strmap_emplace(&map, "Map", "test");
    Strmap_emplace(&map, "Make", "my");
    Strmap_emplace(&map, "Hello", "world");
    Strmap_emplace(&map, "Sunny", "day");

    Strmap_iter it = Strmap_find(&map, "Make");
    Strmap_erase_at(&map, it);
    Strmap_erase(&map, "Hello");

    Strmap res1 = c_make(Strmap, {{"Map", ""}, {"Sunny", ""}});
    Strmap res2 = c_make(Strmap, {{"Sunny", ""}, {"Map", ""}});

    EXPECT_TRUE(Strmap_eq(&res1, &map));
    EXPECT_TRUE(Strmap_eq(&res2, &map));

    c_drop(Strmap, &map, &res1, &res2);
}
