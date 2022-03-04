// This implements the c++ std::map::find example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-17
#include <stc/cstr.h>

#define i_key int
#define i_val_str
#define i_tag istr
#include <stc/csmap.h>

#define i_val csmap_istr_raw
#define i_opt c_no_cmp
#define i_tag istr
#include <stc/cvec.h>

void print_elem(csmap_istr_raw p) {
    printf("(%d, %s) ", p.first, p.second);
}

#define using_print_collection(CX) \
    void print_collection_##CX(CX t) { \
        printf("%" PRIuMAX " elements: ", CX##_size(t)); \
    \
        c_foreach (p, CX, t) { \
            print_elem(CX##_value_toraw(p.ref)); \
        } \
        puts(""); \
    }

using_print_collection(csmap_istr)
using_print_collection(cvec_istr)

void findit(csmap_istr c, csmap_istr_key val)
{
    printf("Trying find() on value %d\n", val);
    csmap_istr_iter result = csmap_istr_find(&c, val); // prefer contains() or get()
    if (result.ref != csmap_istr_end(&c).ref) {
        printf("Element found: "); print_elem(csmap_istr_value_toraw(result.ref)); puts("");
    } else {
        puts("Element not found.");
    }
}

int main()
{
    c_auto (csmap_istr, m1)
    c_auto (cvec_istr, v)
    {
        c_apply(v, csmap_istr_emplace(&m1, c_pair(v)), csmap_istr_raw, 
            {{40, "Zr"}, {45, "Rh"}});
        puts("The starting map m1 is (key, value):");
        print_collection_csmap_istr(m1);

        typedef cvec_istr_value pair;
        cvec_istr_push_back(&v, (pair){43, "Tc"});
        cvec_istr_push_back(&v, (pair){41, "Nb"});
        cvec_istr_push_back(&v, (pair){46, "Pd"});
        cvec_istr_push_back(&v, (pair){42, "Mo"});
        cvec_istr_push_back(&v, (pair){44, "Ru"});
        cvec_istr_push_back(&v, (pair){44, "Ru"}); // attempt a duplicate

        puts("Inserting the following vector data into m1:");
        print_collection_cvec_istr(v);

        c_foreach (i, cvec_istr, cvec_istr_begin(&v), cvec_istr_end(&v))
            csmap_istr_emplace(&m1, c_pair(*i.ref));

        puts("The modified map m1 is (key, value):");
        print_collection_csmap_istr(m1);
        puts("");
        findit(m1, 45);
        findit(m1, 6);
    }
}
