#include "ctest.h"
#include "stc/cstr.h"

// map_erase.c
// From C++ example: https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-16
#define i_type mymap
#define i_key int
#define i_valpro cstr
#define i_use_eq
#include "stc/smap.h"


TEST(smap, erase)
{
    mymap m1 = {0};
    mymap res = {0};

    // Fill in some data to test with, one at a time
    mymap_insert(&m1, 1, cstr_lit("A"));
    mymap_insert(&m1, 2, cstr_lit("B"));
    mymap_insert(&m1, 3, cstr_lit("C"));
    mymap_insert(&m1, 4, cstr_lit("D"));
    mymap_insert(&m1, 5, cstr_lit("E"));

    // Starting data of map m1 is:
    mymap_take(&res, c_make(mymap, {{1, "A"}, {2, "B"}, {3, "C"}, {4, "D"}, {5, "E"}}));
    EXPECT_TRUE(mymap_eq(&res, &m1));
    // The 1st member function removes an element at a given position
    mymap_erase_at(&m1, mymap_advance(mymap_begin(&m1), 1));
    // After the 2nd element is deleted, the map m1 is:
    mymap_take(&res, c_make(mymap, {{1, "A"}, {3, "C"}, {4, "D"}, {5, "E"}, }));
    EXPECT_TRUE(mymap_eq(&res, &m1));

    // Fill in some data to test with
    mymap m2 = c_make(mymap, {
        {10, "Bob"},
        {11, "Rob"},
        {12, "Robert"},
        {13, "Bert"},
        {14, "Bobby"},
    });

    // Starting data of map m2 is:
    mymap_iter it1 = mymap_advance(mymap_begin(&m2), 1);
    mymap_iter it2 = mymap_find(&m2, mymap_back(&m2)->first);

    // The 2nd member function removes elements
    // in the range [First, Last)
    mymap_erase_range(&m2, it1, it2);
    // After the middle elements are deleted, the map m2 is:
    mymap_take(&res, c_make(mymap, {{10, "Bob"}, {14, "Bobby"}, }));
    EXPECT_TRUE(mymap_eq(&res, &m2));

    mymap m3 = {0};

    // Fill in some data to test with, one at a time, using emplace
    mymap_emplace(&m3, 1, "red");
    mymap_emplace(&m3, 2, "yellow");
    mymap_emplace(&m3, 3, "blue");
    mymap_emplace(&m3, 4, "green");
    mymap_emplace(&m3, 5, "orange");
    mymap_emplace(&m3, 6, "purple");
    mymap_emplace(&m3, 7, "pink");

    // Starting data of map m3 is:
    mymap_take(&res, c_make(mymap, {{1, "red"}, {2, "yellow"}, {3, "blue"}, {4, "green"}, {5, "orange"}, {6, "purple"}, {7, "pink"}, }));
    EXPECT_TRUE(mymap_eq(&res, &m3));
    // The 3rd member function removes elements with a given Key
    int count = mymap_erase(&m3, 2);
    // The 3rd member function also returns the number of elements removed:
    EXPECT_EQ(1, count);
    // After the element with a key of 2 is deleted, the map m3 is:
    mymap_take(&res, c_make(mymap, {{1, "red"}, {3, "blue"}, {4, "green"}, {5, "orange"}, {6, "purple"}, {7, "pink"}}));
    EXPECT_TRUE(mymap_eq(&res, &m3));

    c_drop(mymap, &m1, &m2, &m3, &res);
}


// map_insert.c
// This implements the std::map insert c++ example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-19

#define i_type smap_ii, int, int
#define i_use_eq
#include "stc/smap.h"

#define i_type vec_ii, smap_ii_value
#include "stc/vec.h"

TEST(smap, insert)
{
    // insert single values
    smap_ii m1 = {0};
    smap_ii res = {0};

    smap_ii_insert(&m1, 1, 10);
    smap_ii_push(&m1, (smap_ii_value){2, 20});

    // The original key and mapped values of m1 are:
    smap_ii_take(&res, c_make(smap_ii, {{1, 10}, {2, 20}}));
    EXPECT_TRUE(smap_ii_eq(&res, &m1));

    // intentionally attempt a duplicate, single element
    smap_ii_result ret = smap_ii_insert(&m1, 1, 111);
    EXPECT_FALSE(ret.inserted);

    smap_ii_insert(&m1, 3, 30);
    // The modified key and mapped values of m1 are:
    smap_ii_take(&res, c_make(smap_ii, {{1, 10}, {2, 20}, {3, 30}}));
    EXPECT_TRUE(smap_ii_eq(&res, &m1));

    // The templatized version inserting a jumbled range
    smap_ii m2 = {0};
    vec_ii v = {0};
    typedef vec_ii_value ipair;
    vec_ii_push(&v, (ipair){43, 294});
    vec_ii_push(&v, (ipair){41, 262});
    vec_ii_push(&v, (ipair){45, 330});
    vec_ii_push(&v, (ipair){42, 277});
    vec_ii_push(&v, (ipair){44, 311});

    // Inserting the following vector data into m2:
    for (c_each(e, vec_ii, v))
        smap_ii_insert_or_assign(&m2, e.ref->first, e.ref->second);

    // The modified key and mapped values of m2 are:
    smap_ii_take(&res, c_make(smap_ii, {{41, 262}, {42, 277}, {43, 294}, {44, 311}, {45, 330}}));
    EXPECT_TRUE(smap_ii_eq(&res, &m2));

    // The templatized versions move-constructing elements
    mymap m3 = {0};
    mymap res3 = {0};

    mymap_value ip1 = {475, cstr_lit("blue")}, ip2 = {510, cstr_lit("green")};

    // single element
    mymap_insert(&m3, ip1.first, cstr_move(&ip1.second));
    // After the first move insertion, m3 contains:
    mymap_take(&res3, c_make(mymap, {{475, "blue"}}));
    EXPECT_TRUE(mymap_eq(&res3, &m3));

    // single element
    mymap_insert(&m3, ip2.first, cstr_move(&ip2.second));
    // After the second move insertion, m3 contains:
    mymap_take(&res3, c_make(mymap, {{475, "blue"}, {510, "green"}}));
    EXPECT_TRUE(mymap_eq(&res3, &m3));

    c_drop(smap_ii, &m1, &m2, &res);
    c_drop(mymap, &m3, &res3);
    c_drop(vec_ii, &v);
}
