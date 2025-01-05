#include <stdio.h>
#include "stc/cstr.h"
#include "ctest.h"

#define i_type hmap_ii, int, int
#include "stc/hmap.h"

TEST(hmap, mapdemo1)
{
    hmap_ii nums = {0};
    hmap_ii_insert(&nums, 8, 64);
    hmap_ii_insert(&nums, 11, 121);
    EXPECT_EQ(64, *hmap_ii_at(&nums, 8));
    hmap_ii_drop(&nums);
}

#define i_type hmap_si
#define i_keypro cstr
#define i_val int
#include "stc/hmap.h"

TEST(hmap, mapdemo2)
{
    hmap_si nums = {0};
    hmap_si_clear(&nums);
    hmap_si_emplace_or_assign(&nums, "Hello", 64);
    hmap_si_emplace_or_assign(&nums, "Groovy", 121);
    hmap_si_emplace_or_assign(&nums, "Groovy", 200); // overwrite previous

    EXPECT_EQ(200, *hmap_si_at(&nums, "Groovy"));
    EXPECT_EQ(64, *hmap_si_at(&nums, "Hello"));

    hmap_si_drop(&nums);
}


#define i_keypro cstr
#define i_valpro cstr
#include "stc/hmap.h"

TEST(hmap, mapdemo3)
{
    hmap_cstr map = {0};
    hmap_cstr_emplace(&map, "Map", "test");
    hmap_cstr_emplace(&map, "Make", "my");
    hmap_cstr_emplace(&map, "Hello", "world");
    hmap_cstr_emplace(&map, "Sunny", "day");

    hmap_cstr_iter it = hmap_cstr_find(&map, "Make");
    hmap_cstr_erase_at(&map, it);
    hmap_cstr_erase(&map, "Hello");

    hmap_cstr res1 = c_make(hmap_cstr, {{"Map", ""}, {"Sunny", ""}});
    hmap_cstr res2 = c_make(hmap_cstr, {{"Sunny", ""}, {"Map", ""}});

    EXPECT_TRUE(hmap_cstr_eq(&res1, &map));
    EXPECT_TRUE(hmap_cstr_eq(&res2, &map));

    c_drop(hmap_cstr, &map, &res1, &res2);
}
