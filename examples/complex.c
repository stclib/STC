#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void check_del(float* v) {printf("destroy %g\n", *v);}

using_carray(f, float, check_del, c_default_clone); // normally omit the last argument - float type need no destroy.
using_clist(a, carray2f, c_no_compare, carray2f_del);
using_cmap(l, int, clist_a, clist_a_del, clist_a_clone);
using_cmap_strkey(s, cmap_l, cmap_l_del, cmap_l_clone);

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 5, tableKey = 42;
    const char* strKey = "first";
    cmap_l listMap = cmap_inits;

    cmap_s myMap = cmap_inits;

    // Construct.
    carray2f arr_a = carray2f_init(ydim, xdim, 0.f);
    printf("arr_a: (%zu, %zu)\n", carray2f_ydim(arr_a), carray2f_xdim(arr_a));

    clist_a tableList = clist_inits;
    // Put in some data.
    *carray2f_at(&arr_a, y, x) = 3.1415927f; // aa[y][x]
    clist_a_push_back(&tableList, arr_a);
    cmap_l_put(&listMap, tableKey, tableList);
    cmap_s_put(&myMap, strKey, listMap);

    // Access the data entry
    carray2f arr_b = *clist_a_back(&cmap_l_find(&cmap_s_find(&myMap, strKey)->second, tableKey)->second);
    printf("value (%d, %d) is: %f\n", y, x, *carray2f_at(&arr_b, y, x));

    cmap_s_del(&myMap); // free up everything!
}
