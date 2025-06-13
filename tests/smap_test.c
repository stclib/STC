#include "ctest.h"
#include <stc/cstr.h>

// map_erase.c
// From C++ example: https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-16
#define T Mymap, int, cstr, (c_use_eq | c_valpro)
#include <stc/sortedmap.h>

TEST(sortedmap, erase)
{
    Mymap m1 = {0};
    Mymap res = {0};

    // Fill in some data to test with, one at a time
    Mymap_insert(&m1, 1, cstr_lit("A"));
    Mymap_insert(&m1, 2, cstr_lit("B"));
    Mymap_insert(&m1, 3, cstr_lit("C"));
    Mymap_insert(&m1, 4, cstr_lit("D"));
    Mymap_insert(&m1, 5, cstr_lit("E"));

    // Starting data of map m1 is:
    Mymap_take(&res, c_make(Mymap, {{1, "A"}, {2, "B"}, {3, "C"}, {4, "D"}, {5, "E"}}));
    EXPECT_TRUE(Mymap_eq(&res, &m1));
    // The 1st member function removes an element at a given position
    Mymap_erase_at(&m1, Mymap_advance(Mymap_begin(&m1), 1));
    // After the 2nd element is deleted, the map m1 is:
    Mymap_take(&res, c_make(Mymap, {{1, "A"}, {3, "C"}, {4, "D"}, {5, "E"}, }));
    EXPECT_TRUE(Mymap_eq(&res, &m1));

    // Fill in some data to test with
    Mymap m2 = c_make(Mymap, {
        {10, "Bob"},
        {11, "Rob"},
        {12, "Robert"},
        {13, "Bert"},
        {14, "Bobby"},
    });

    // Starting data of map m2 is:
    Mymap_iter it1 = Mymap_advance(Mymap_begin(&m2), 1);
    Mymap_iter it2 = Mymap_find(&m2, Mymap_back(&m2)->first);

    // The 2nd member function removes elements
    // in the range [First, Last)
    Mymap_erase_range(&m2, it1, it2);
    // After the middle elements are deleted, the map m2 is:
    Mymap_take(&res, c_make(Mymap, {{10, "Bob"}, {14, "Bobby"}, }));
    EXPECT_TRUE(Mymap_eq(&res, &m2));

    Mymap m3 = {0};

    // Fill in some data to test with, one at a time, using emplace
    Mymap_emplace(&m3, 1, "red");
    Mymap_emplace(&m3, 2, "yellow");
    Mymap_emplace(&m3, 3, "blue");
    Mymap_emplace(&m3, 4, "green");
    Mymap_emplace(&m3, 5, "orange");
    Mymap_emplace(&m3, 6, "purple");
    Mymap_emplace(&m3, 7, "pink");

    // Starting data of map m3 is:
    Mymap_take(&res, c_make(Mymap, {{1, "red"}, {2, "yellow"}, {3, "blue"}, {4, "green"}, {5, "orange"}, {6, "purple"}, {7, "pink"}, }));
    EXPECT_TRUE(Mymap_eq(&res, &m3));
    // The 3rd member function removes elements with a given Key
    int count = Mymap_erase(&m3, 2);
    // The 3rd member function also returns the number of elements removed:
    EXPECT_EQ(1, count);
    // After the element with a key of 2 is deleted, the map m3 is:
    Mymap_take(&res, c_make(Mymap, {{1, "red"}, {3, "blue"}, {4, "green"}, {5, "orange"}, {6, "purple"}, {7, "pink"}}));
    EXPECT_TRUE(Mymap_eq(&res, &m3));

    c_drop(Mymap, &m1, &m2, &m3, &res);
}


// map_insert.c
// This implements the std::map insert c++ example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-19

#define T Intmap, int, int, (c_use_eq)
#include <stc/sortedmap.h>

#define T Pairvec, Intmap_raw
#include <stc/vec.h>

TEST(sortedmap, insert)
{
    // insert single values
    Intmap m1 = {0};
    Intmap res = {0};

    Intmap_insert(&m1, 1, 10);
    Intmap_push(&m1, (Intmap_value){2, 20});

    // The original key and mapped values of m1 are:
    Intmap_take(&res, c_make(Intmap, {{1, 10}, {2, 20}}));
    EXPECT_TRUE(Intmap_eq(&res, &m1));

    // intentionally attempt a duplicate, single element
    Intmap_result ret = Intmap_insert(&m1, 1, 111);
    EXPECT_FALSE(ret.inserted);

    Intmap_insert(&m1, 3, 30);
    // The modified key and mapped values of m1 are:
    Intmap_take(&res, c_make(Intmap, {{1, 10}, {2, 20}, {3, 30}}));
    EXPECT_TRUE(Intmap_eq(&res, &m1));

    // The templatized version inserting a jumbled range
    Intmap m2 = {0};
    Pairvec v = {0};
    Pairvec_push(&v, (Pairvec_value){43, 294});
    Pairvec_push(&v, (Pairvec_value){41, 262});
    Pairvec_push(&v, (Pairvec_value){45, 330});
    Pairvec_push(&v, (Pairvec_value){42, 277});
    Pairvec_push(&v, (Pairvec_value){44, 311});

    // Inserting the following vector data into m2:
    Intmap_put_n(&m2, v.data, Pairvec_size(&v));

    // The modified key and mapped values of m2 are:
    Intmap_take(&res, c_make(Intmap, {{41, 262}, {42, 277}, {43, 294}, {44, 311}, {45, 330}}));
    EXPECT_TRUE(Intmap_eq(&res, &m2));

    // The templatized versions move-constructing elements
    Mymap m3 = {0};
    Mymap res3 = {0};

    Mymap_value ip1 = {475, cstr_lit("blue")}, ip2 = {510, cstr_lit("green")};

    // single element
    Mymap_insert(&m3, ip1.first, cstr_move(&ip1.second));
    // After the first move insertion, m3 contains:
    Mymap_take(&res3, c_make(Mymap, {{475, "blue"}}));
    EXPECT_TRUE(Mymap_eq(&res3, &m3));

    // single element
    Mymap_insert(&m3, ip2.first, cstr_move(&ip2.second));
    // After the second move insertion, m3 contains:
    Mymap_take(&res3, c_make(Mymap, {{475, "blue"}, {510, "green"}}));
    EXPECT_TRUE(Mymap_eq(&res3, &m3));

    c_drop(Intmap, &m1, &m2, &res);
    c_drop(Mymap, &m3, &res3);
    c_drop(Pairvec, &v);
}
