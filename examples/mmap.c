#include <stc/csmap.h>
#include <stc/clist.h>
#include <stc/cstr.h>

// This implements the multimap c++ example found at:
// https://en.cppreference.com/w/cpp/container/multimap/insert

// Map of int => clist_str. Note the negation of c_default_compare
using_clist_str();
using_csmap(m, int, clist_str, -c_default_compare, clist_str_del);

void print(const csmap_m mmap)
{
    c_foreach (e, csmap_m, mmap) {
        c_foreach (s, clist_str, e.ref->second)
            printf("{%d,%s} ", e.ref->first, s.ref->str);
    }
    puts("");
}

void insert(csmap_m* mmap, int key, const char* str)
{
    clist_str *list = &csmap_m_insert(mmap, key, clist_str_init()).ref->second;
    clist_str_emplace_back(list, str);
}

int main()
{
    c_forvar (csmap_m mmap = csmap_m_init(), csmap_m_del(&mmap))
    {
        // list-initialize
        struct {int i; const char* s;} vals[] = {{2, "foo"}, {2, "bar"}, {3, "baz"}, {1, "abc"}, {5, "def"}};
        c_forrange (i, c_arraylen(vals)) insert(&mmap, vals[i].i, vals[i].s);

        // insert using value_type
        insert(&mmap, 5, "pqr");
        print(mmap);

        // insert using make_pair
        insert(&mmap, 6, "uvw");
        print(mmap);

        insert(&mmap, 7, "xyz");
        print(mmap);

        // insert using initialization_list
        insert(&mmap, 5, "one");
        insert(&mmap, 5, "two");
        print(mmap);

        // erase all entries with key 5
        csmap_m_erase(&mmap, 5);
        print(mmap);

        // find and erase a specific entry
        clist_str_iter_t pos;
        c_foreach (e, csmap_m, mmap)
            if ((pos = clist_str_find(&e.ref->second, "bar")).ref) {
                clist_str_erase(&e.ref->second, pos);
                break;
            }
        print(mmap);
    }
}
