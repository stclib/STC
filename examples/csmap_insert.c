#include <stc/cstr.h>
#include <stdio.h>

// This implements the std::map insert c++ example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-19

#define i_key int
#define i_val int
#define i_tag ii  // Map of int => int
#include <stc/csmap.h>

#define i_key int
#define i_val_str
#define i_tag istr // Map of int => cstr
#include <stc/csmap.h>

#define i_val csmap_ii_raw
#define i_opt c_no_cmp
#define i_tag ii
#include <stc/cvec.h>

void print_ii(csmap_ii map) {
    c_foreach (e, csmap_ii, map)
        printf("(%d, %d) ", e.ref->first, e.ref->second);
    puts("");
}

void print_istr(csmap_istr map) {
    c_foreach (e, csmap_istr, map)
        printf("(%d, %s) ", e.ref->first, cstr_str(&e.ref->second));
    puts("");
}

int main()
{
    // insert single values
    c_auto (csmap_ii, m1) {
        csmap_ii_insert(&m1, 1, 10);
        csmap_ii_insert(&m1, 2, 20);

        puts("The original key and mapped values of m1 are:");
        print_ii(m1);

        // intentionally attempt a duplicate, single element
        csmap_ii_result ret = csmap_ii_insert(&m1, 1, 111);
        if (!ret.inserted) {
            csmap_ii_value pr = *ret.ref;
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
    c_auto (csmap_ii, m2)
    c_auto (cvec_ii, v) {
        typedef cvec_ii_value ipair;
        cvec_ii_push_back(&v, (ipair){43, 294});
        cvec_ii_push_back(&v, (ipair){41, 262});
        cvec_ii_push_back(&v, (ipair){45, 330});
        cvec_ii_push_back(&v, (ipair){42, 277});
        cvec_ii_push_back(&v, (ipair){44, 311});

        puts("Inserting the following vector data into m2:");
        c_foreach (e, cvec_ii, v)
            printf("(%d, %d) ", e.ref->first, e.ref->second);
        puts("");

        c_foreach (e, cvec_ii, v) 
            csmap_ii_insert_or_assign(&m2, e.ref->first, e.ref->second);

        puts("The modified key and mapped values of m2 are:");
        c_foreach (e, csmap_ii, m2)
            printf("(%d, %d) ", e.ref->first, e.ref->second);
        puts("\n");
    }

    // The templatized versions move-constructing elements
    c_auto (csmap_istr, m3) {
        csmap_istr_value ip1 = {475, cstr_new("blue")}, ip2 = {510, cstr_new("green")};

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

    c_auto (csmap_ii, m4) {
        // Insert the elements from an initializer_list
        c_apply(v, csmap_ii_insert(&m4, c_pair(v)), csmap_ii_raw, {
            { 4, 44 }, { 2, 22 }, { 3, 33 }, { 1, 11 }, { 5, 55 }
        });
        puts("After initializer_list insertion, m4 contains:");
        print_ii(m4);
        puts("");
    }
}
