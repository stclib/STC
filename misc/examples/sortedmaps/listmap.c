// This implements the multimap c++ example found at:
// https://en.cppreference.com/w/cpp/container/multimap/insert

// Multimap entries
#define i_implement
#include "stc/cstr.h"
#define i_key_cstr
#include "stc/list.h"

// Map of int => list_cstr.
#define i_type Multimap
#define i_key int
#define i_valclass list_cstr // set i_val = list_cstr, bind list_cstr_clone and list_cstr_drop
#define i_cmp -c_default_cmp // like std::greater<int>
#include "stc/smap.h"

void print(const char* lbl, const Multimap mmap)
{
    printf("%s ", lbl);
    c_foreach (e, Multimap, mmap) {
        c_foreach (s, list_cstr, e.ref->second)
            printf("{%d,%s} ", e.ref->first, cstr_str(s.ref));
    }
    puts("");
}

void insert(Multimap* mmap, int key, const char* str)
{
    list_cstr *list = &Multimap_insert(mmap, key, list_cstr_init()).ref->second;
    list_cstr_emplace_back(list, str);
}

int main(void)
{
    Multimap mmap = {0};

    // list-initialize
    struct pair {int a; const char* b;};
    c_foritems (i, struct pair, {{2, "foo"}, {2, "bar"}, {3, "baz"}, {1, "abc"}, {5, "def"}})
        insert(&mmap, i.ref->a, i.ref->b);
    print("#1", mmap);

    // insert using value_type
    insert(&mmap, 5, "pqr");
    print("#2", mmap);

    // insert using make_pair
    insert(&mmap, 6, "uvw");
    print("#3", mmap);

    insert(&mmap, 7, "xyz");
    print("#4", mmap);

    // insert using initialization_list
    c_foritems (i, struct pair, {{5, "one"}, {5, "two"}})
        insert(&mmap, i.ref->a, i.ref->b);
    print("#5", mmap);

    // FOLLOWING NOT IN ORIGINAL EXAMPLE:
    // erase all entries with key 5
    Multimap_erase(&mmap, 5);
    print("+5", mmap);

    Multimap_drop(&mmap);
}
