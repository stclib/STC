#include <stc/csmap.h>
#include <stc/cvec.h>
#include <stc/csview.h>
#include <stdio.h>

// This implements the std::map insert c++ example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-19


using_csmap(ii, int, int);     // Map of int => int
using_csmap_strval(istr, int); // Map of int => cstr
using_cvec(ii, csmap_ii_rawvalue_t, c_no_compare);

void print_ii(csmap_ii map) {
    c_foreach (e, csmap_ii, map)
        printf("(%d, %d) ", e.ref->first, e.ref->second);
    puts("");
}
void print_istr(csmap_istr map) {
    c_foreach (e, csmap_istr, map)
        printf("(%d, %s) ", e.ref->first, e.ref->second.str);
    puts("");
}

int main()
{
    // insert single values
    c_forvar (csmap_ii m1 = csmap_ii_init(), csmap_ii_del(&m1)) {
        csmap_ii_insert(&m1, 1, 10);
        csmap_ii_insert(&m1, 2, 20);

        puts("The original key and mapped values of m1 are:");
        print_ii(m1);

        // intentionally attempt a duplicate, single element
        csmap_ii_result_t ret = csmap_ii_insert(&m1, 1, 111);
        if (!ret.inserted) {
            csmap_ii_value_t pr = *ret.ref;
            puts("Insert failed, element with key value 1 already exists.");
            printf("  The existing element is (%d, %d)\n", pr.first, pr.second);
        }
        else {
            puts("The modified key and mapped values of m1 are:");
            print_ii(m1);
        }
        puts("");

        csmap_ii_insert(&m1, 3, 30);
        puts("The modified key and mapped values of m1 are:");
        print_ii(m1);
        puts("");
    }

    // The templatized version inserting a jumbled range
    c_forvar (csmap_ii m2 = csmap_ii_init(), csmap_ii_del(&m2))
    c_forvar (cvec_ii v = cvec_ii_init(), cvec_ii_del(&v)) {
        cvec_ii_push_back(&v, (cvec_ii_value_t){43, 294});
        cvec_ii_push_back(&v, (cvec_ii_value_t){41, 262});
        cvec_ii_push_back(&v, (cvec_ii_value_t){45, 330});
        cvec_ii_push_back(&v, (cvec_ii_value_t){42, 277});
        cvec_ii_push_back(&v, (cvec_ii_value_t){44, 311});

        puts("Inserting the following vector data into m2:");
        c_foreach (e, cvec_ii, v) printf("(%d, %d) ", e.ref->first, e.ref->second);
        puts("");

        csmap_ii_emplace_items(&m2, v.data, cvec_ii_size(v));

        puts("The modified key and mapped values of m2 are:");
        c_foreach (e, cvec_ii, v) printf("(%d, %d) ", e.ref->first, e.ref->second);
        puts("\n");
    }

    // The templatized versions move-constructing elements
    c_forvar (csmap_istr m3 = csmap_istr_init(), csmap_istr_del(&m3)) {
        csmap_istr_value_t ip1 = {475, cstr_lit("blue")}, ip2 = {510, cstr_lit("green")};

        // single element
        csmap_istr_insert(&m3, ip1.first, cstr_move(&ip1.second));
        puts("After the first move insertion, m3 contains:");
        print_istr(m3);

        // single element
        csmap_istr_insert(&m3, ip2.first, cstr_move(&ip2.second));
        puts("After the second move insertion, m3 contains:");
        print_istr(m3);
        puts("");
    }

    c_forvar (csmap_ii m4 = csmap_ii_init(), csmap_ii_del(&m4)) {
        // Insert the elements from an initializer_list
        c_emplace(csmap_ii, m4, { { 4, 44 }, { 2, 22 }, { 3, 33 }, { 1, 11 }, { 5, 55 } });
        puts("After initializer_list insertion, m4 contains:");
        print_ii(m4);
        puts("");
    }
}