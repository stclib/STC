// This implements the std::map insert c++ example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-19

#define i_TYPE smap_ii, int, int
#include "stc/smap.h"

#define i_TYPE vec_ii, smap_ii_value
#include "stc/vec.h"

#define i_implement
#include "stc/cstr.h"

#define i_type smap_istr // Map of int => cstr
#define i_key int
#define i_val_cstr
#include "stc/smap.h"

void print_ii(smap_ii map) {
    c_foreach (e, smap_ii, map)
        printf("(%d, %d) ", e.ref->first, e.ref->second);
    puts("");
}

void print_istr(smap_istr map) {
    c_foreach (e, smap_istr, map)
        printf("(%d, %s) ", e.ref->first, cstr_str(&e.ref->second));
    puts("");
}

int main(void)
{
    // insert single values
    smap_ii m1 = {0};
    smap_ii_insert(&m1, 1, 10);
    smap_ii_push(&m1, c_LITERAL(smap_ii_value){2, 20});

    puts("The original key and mapped values of m1 are:");
    print_ii(m1);

    // intentionally attempt a duplicate, single element
    smap_ii_result ret = smap_ii_insert(&m1, 1, 111);
    if (!ret.inserted) {
        smap_ii_value pr = *ret.ref;
        puts("Insert failed, element with key value 1 already exists.");
        printf("  The existing element is (%d, %d)\n", pr.first, pr.second);
    }
    else {
        puts("The modified key and mapped values of m1 are:");
        print_ii(m1);
    }
    puts("");

    smap_ii_insert(&m1, 3, 30);
    puts("The modified key and mapped values of m1 are:");
    print_ii(m1);
    puts("");

    // The templatized version inserting a jumbled range
    smap_ii m2 = {0};
    vec_ii v = {0};
    typedef vec_ii_value ipair;
    vec_ii_push(&v, c_LITERAL(ipair){43, 294});
    vec_ii_push(&v, c_LITERAL(ipair){41, 262});
    vec_ii_push(&v, c_LITERAL(ipair){45, 330});
    vec_ii_push(&v, c_LITERAL(ipair){42, 277});
    vec_ii_push(&v, c_LITERAL(ipair){44, 311});

    puts("Inserting the following vector data into m2:");
    c_foreach (e, vec_ii, v)
        printf("(%d, %d) ", e.ref->first, e.ref->second);
    puts("");

    c_foreach (e, vec_ii, v)
        smap_ii_insert_or_assign(&m2, e.ref->first, e.ref->second);

    puts("The modified key and mapped values of m2 are:");
    c_foreach (e, smap_ii, m2)
        printf("(%d, %d) ", e.ref->first, e.ref->second);
    puts("\n");

    // The templatized versions move-constructing elements
    smap_istr m3 = {0};
    smap_istr_value ip1 = {475, cstr_lit("blue")}, ip2 = {510, cstr_lit("green")};

    // single element
    smap_istr_insert(&m3, ip1.first, cstr_move(&ip1.second));
    puts("After the first move insertion, m3 contains:");
    print_istr(m3);

    // single element
    smap_istr_insert(&m3, ip2.first, cstr_move(&ip2.second));
    puts("After the second move insertion, m3 contains:");
    print_istr(m3);
    puts("");

    smap_ii m4 = {0};
    // Insert the elements from an initializer_list
    m4 = c_init(smap_ii, {{4, 44}, {2, 22}, {3, 33}, {1, 11}, {5, 55}});
    puts("After initializer_list insertion, m4 contains:");
    print_ii(m4);
    puts("");

    vec_ii_drop(&v);
    smap_istr_drop(&m3);
    c_drop(smap_ii, &m1, &m2, &m4);
}
