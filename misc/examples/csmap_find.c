// This implements the c++ std::map::find example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-17
#define i_implement
#include <stc/cstr.h>

#define i_key int
#define i_val_str
#define i_tag istr
#include <stc/csmap.h>

#define i_key csmap_istr_raw
#define i_tag istr
#include <stc/cvec.h>

void print_elem(csmap_istr_raw p) {
    printf("(%d, %s) ", p.first, p.second);
}

#define using_print_collection(CX) \
    void print_collection_##CX(const CX* t) { \
        printf("%" c_ZI " elements: ", CX##_size(t)); \
    \
        c_foreach (p, CX, *t) { \
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
    if (result.ref) {
        printf("Element found: "); print_elem(csmap_istr_value_toraw(result.ref)); puts("");
    } else {
        puts("Element not found.");
    }
}

int main(void)
{
    csmap_istr m1 = c_init(csmap_istr, {{40, "Zr"}, {45, "Rh"}});
    cvec_istr v = {0};

    puts("The starting map m1 is (key, value):");
    print_collection_csmap_istr(&m1);

    typedef cvec_istr_value pair;
    cvec_istr_push(&v, c_LITERAL(pair){43, "Tc"});
    cvec_istr_push(&v, c_LITERAL(pair){41, "Nb"});
    cvec_istr_push(&v, c_LITERAL(pair){46, "Pd"});
    cvec_istr_push(&v, c_LITERAL(pair){42, "Mo"});
    cvec_istr_push(&v, c_LITERAL(pair){44, "Ru"});
    cvec_istr_push(&v, c_LITERAL(pair){44, "Ru"}); // attempt a duplicate

    puts("Inserting the following vector data into m1:");
    print_collection_cvec_istr(&v);

    c_foreach (i, cvec_istr, cvec_istr_begin(&v), cvec_istr_end(&v))
        csmap_istr_emplace(&m1, i.ref->first, i.ref->second);

    puts("The modified map m1 is (key, value):");
    print_collection_csmap_istr(&m1);
    puts("");
    findit(m1, 45);
    findit(m1, 6);

    csmap_istr_drop(&m1);
    cvec_istr_drop(&v);
}
