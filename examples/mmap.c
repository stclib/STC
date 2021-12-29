// This implements the multimap c++ example found at:
// https://en.cppreference.com/w/cpp/container/multimap/insert

// Map of int => clist_str. Note the negation of c_default_cmp
#define i_val_str
#include <stc/clist.h>

#define i_type Multimap
#define i_key int
#define i_val_bind clist_str
#define i_cmp -c_default_cmp
#include <stc/csmap.h>

void print(const Multimap mmap)
{
    c_foreach (e, Multimap, mmap) {
        c_foreach (s, clist_str, e.ref->second)
            printf("{%d,%s} ", e.ref->first, s.ref->str);
    }
    puts("");
}

void insert(Multimap* mmap, int key, const char* str)
{
    clist_str *list = &Multimap_insert(mmap, key, clist_str_init()).ref->second;
    clist_str_emplace_back(list, str);
}

int main()
{
    c_auto (Multimap, mmap)
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
        Multimap_erase(&mmap, 5);
        print(mmap);

        // find and erase a specific entry
        clist_str_iter pos;
        c_foreach (e, Multimap, mmap)
            if ((pos = clist_str_find(&e.ref->second, "bar")).ref != clist_str_end(&e.ref->second).ref) {
                clist_str_erase_at(&e.ref->second, pos);
                break;
            }
        print(mmap);
    }
}
