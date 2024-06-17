// This implements the c++ std::map::find example at:
// https://docs.microsoft.com/en-us/cpp/standard-library/map-class?view=msvc-160#example-17
#define i_implement
#include "stc/cstr.h"

#define i_type smap_istr
#define i_key int
#define i_val_cstr
#include "stc/smap.h"

#define i_TYPE vec_istr, smap_istr_raw
#include "stc/vec.h"

void print_elem(smap_istr_raw p) {
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

using_print_collection(smap_istr)
using_print_collection(vec_istr)

void findit(smap_istr c, smap_istr_key val)
{
    printf("Trying find() on value %d\n", val);
    smap_istr_iter result = smap_istr_find(&c, val); // prefer contains() or get()
    if (result.ref) {
        printf("Element found: "); print_elem(smap_istr_value_toraw(result.ref)); puts("");
    } else {
        puts("Element not found.");
    }
}

int main(void)
{
    smap_istr m1 = c_init(smap_istr, {{40, "Zr"}, {45, "Rh"}});
    vec_istr v = {0};

    puts("The starting map m1 is (key, value):");
    print_collection_smap_istr(&m1);

    typedef vec_istr_value pair;
    vec_istr_push(&v, c_LITERAL(pair){43, "Tc"});
    vec_istr_push(&v, c_LITERAL(pair){41, "Nb"});
    vec_istr_push(&v, c_LITERAL(pair){46, "Pd"});
    vec_istr_push(&v, c_LITERAL(pair){42, "Mo"});
    vec_istr_push(&v, c_LITERAL(pair){44, "Ru"});
    vec_istr_push(&v, c_LITERAL(pair){44, "Ru"}); // attempt a duplicate

    puts("Inserting the following vector data into m1:");
    print_collection_vec_istr(&v);

    c_foreach (i, vec_istr, vec_istr_begin(&v), vec_istr_end(&v))
        smap_istr_emplace(&m1, i.ref->first, i.ref->second);

    puts("The modified map m1 is (key, value):");
    print_collection_smap_istr(&m1);
    puts("");
    findit(m1, 45);
    findit(m1, 6);

    smap_istr_drop(&m1);
    vec_istr_drop(&v);
}
