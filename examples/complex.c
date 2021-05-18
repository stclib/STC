#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void check_del(float* v) {printf("destroy %g\n", *v);}

using_carray2(f, float, check_del, c_default_fromraw);
using_clist(arr, carray2f, c_no_compare, carray2f_del, c_no_clone);
using_cmap(lst, int, clist_arr, c_default_equals, c_default_hash, clist_arr_del, c_no_clone);
using_cmap_strkey(map, cmap_lst, cmap_lst_del, c_no_clone);
// c++:
// using array2f = std::array<std::array<float, 6>, 4>>;
// using map_lst = std::unordered_map<int, std::forward_list<array2f>>;
// using map_map = std::unordered_map<std::string, map_lst>;

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 3, tableKey = 42;
    const char* strKey = "first";

    cmap_map myMap = cmap_map_init();
    cmap_lst listMap = cmap_lst_init();
    clist_arr tableList = clist_arr_init();
    carray2f arr2 = carray2f_with_values(xdim, ydim, 1.f);

    printf("arr2 size: %zu x %zu\n", arr2.xdim, arr2.ydim);

    // Put in some data in 2D array
    arr2.data[x][y] = 3.1415927f;
    clist_arr_push_back(&tableList, arr2);
    cmap_lst_insert(&listMap, tableKey, tableList);
    cmap_map_insert(&myMap, cstr_from(strKey), listMap);

    // Access the data entry
    cmap_lst* mapl = &cmap_map_find(&myMap, strKey).ref->second;
    clist_arr* lsta = &cmap_lst_find(mapl, tableKey).ref->second;
    carray2f arr = *clist_arr_back(lsta);

    printf("value (%d, %d) is: %f\n", x, y, arr.data[x][y]);

    arr2.data[x][y] = 1.41421356f; // change the value in array
    cmap_map_del(&myMap); // free up everything!
}
