// This implements the multimap c++ example found at:
// https://en.cppreference.com/w/cpp/container/multimap/insert

// Multi-map entries
#include <stc/cstr.h>
#define i_pro_key cstr
#include <stc/list.h>

// Map of int => list_cstr.
// `c_class_val` binds list_cstr_clone() and list_cstr_drop()
#define T Mmap, int, list_cstr, (c_class_val)
#define i_cmp -c_default_cmp // like std::greater<int>
#include <stc/sortedmap.h>

void print(const char* lbl, const Mmap mmap)
{
    printf("%s ", lbl);
    for (c_each_ref(e, Mmap, mmap)) {
        for (c_each_ref(s, list_cstr, e->second))
            printf("{%d,%s} ", e->first, cstr_str(s));
    }
    puts("");
}

void insert(Mmap* mmap, int key, const char* str)
{
    list_cstr *list = &Mmap_insert(mmap, key, list_cstr_init()).ref->second;
    list_cstr_emplace(list, str);
}

int main(void)
{
    Mmap mmap = {0};

    // list-initialize
    struct pair {int a; const char* b;};
    for (c_items(i, struct pair, {{2, "foo"}, {2, "bar"}, {3, "baz"}, {1, "abc"}, {5, "def"}}))
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
    for (c_items(it, struct pair, {{5, "one"}, {5, "two"}}))
        insert(&mmap, it.ref->a, it.ref->b);
    print("#5", mmap);

    // erase all entries with key 5. NOT IN ORIGINAL EXAMPLE.
    Mmap_erase(&mmap, 5);
    print("+5", mmap);

    // insert using a pair of iterators
    Mmap_clear(&mmap);
    struct pair il[] = {{1, "ä"}, {2, "ё"}, {2, "ö"}, {3, "ü"}};
    for (c_range(i, c_countof(il)))
        insert(&mmap, il[i].a, il[i].b);
    print("#6", mmap);

    Mmap_drop(&mmap);
}
