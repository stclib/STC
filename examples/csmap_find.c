// This implements the c++ std::map::find example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-17
#include <stc/csmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>
#include <stdio.h>

using_csmap_strval(istr, int);
using_cvec(istr, csmap_istr_rawvalue_t, c_no_compare);

void print_elem(csmap_istr_rawvalue_t p) {
    printf("(%d, %s) ", p.first, p.second);
}

#define using_print_collection(CX) \
    void print_collection_##CX(CX t) { \
        printf("%zu elements: ", CX##_size(t)); \
    \
        c_foreach (p, CX, t) { \
            print_elem(CX##_value_toraw(p.ref)); \
        } \
        puts(""); \
    }

using_print_collection(csmap_istr);
using_print_collection(cvec_istr);


void findit(csmap_istr c, csmap_istr_key_t val)
{
    printf("Trying find() on value %d\n", val);
    csmap_istr_value_t* result = csmap_istr_get(&c, val); // easier with get than find.
    if (result) {
        printf("Element found: "); print_elem(csmap_istr_value_toraw(result)); puts("");
    } else {
        puts("Element not found.");
    }
}

int main()
{
    c_forauto (csmap_istr, m1)
    c_forauto (cvec_istr, v)
    {
        c_emplace(csmap_istr, m1, { { 40, "Zr" }, { 45, "Rh" } });
        puts("The starting map m1 is (key, value):");
        print_collection_csmap_istr(m1);

        typedef cvec_istr_value_t pair;
        cvec_istr_emplace_back(&v, (pair){43, "Tc"});
        cvec_istr_emplace_back(&v, (pair){41, "Nb"});
        cvec_istr_emplace_back(&v, (pair){46, "Pd"});
        cvec_istr_emplace_back(&v, (pair){42, "Mo"});
        cvec_istr_emplace_back(&v, (pair){44, "Ru"});
        cvec_istr_emplace_back(&v, (pair){44, "Ru"}); // attempt a duplicate

        puts("Inserting the following vector data into m1:");
        print_collection_cvec_istr(v);
        
        c_foreach (i, cvec_istr, v) csmap_istr_emplace(&m1, i.ref->first, i.ref->second);

        puts("The modified map m1 is (key, value):");
        print_collection_csmap_istr(m1);
        puts("");
        findit(m1, 45);
        findit(m1, 6);
    }
}
