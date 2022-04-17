// This implements the multimap c++ example found at:
// https://en.cppreference.com/w/cpp/container/multimap/insert

// Multimap entries
#define i_val_str
#include <stc/clist.h>

// Map of int => clist_str.
#define i_type Multimap
#define i_key int
#define i_val_bind clist_str // uses clist_str as i_val and binds clist_str_clone, clist_str_drop
#define i_cmp -c_default_cmp // like std::greater<int>
#include <stc/csmap.h>

void print(const char* lbl, const Multimap mmap)
{
    printf("%s ", lbl);
    c_foreach (e, Multimap, mmap) {
        c_foreach (s, clist_str, e.ref->second)
            printf("{%d,%s} ", e.ref->first, cstr_str(s.ref));
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
        struct { int first; const char* second; } vals[] =
            {{2, "foo"}, {2, "bar"}, {3, "baz"}, {1, "abc"}, {5, "def"}};
        c_forrange (i, c_arraylen(vals)) insert(&mmap, c_pair(vals[i]));
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
        insert(&mmap, 5, "one");
        insert(&mmap, 5, "two");
        print("#5", mmap);

        // FOLLOWING NOT IN ORIGINAL EXAMPLE:
        
        // erase all entries with key 5
        Multimap_erase(&mmap, 5);
        print("+6", mmap);

        // find and erase first entry containing "bar"
        clist_str_iter pos;
        c_foreach (e, Multimap, mmap) {
            if ((pos = clist_str_find(&e.ref->second, "bar")).ref != clist_str_end(&e.ref->second).ref) {
                clist_str_erase_at(&e.ref->second, pos);
                break;
            }
        }
        print("+7", mmap);
    }
}
