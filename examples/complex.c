#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void check_destroy(float* v) {printf("destroy %g\n", *v);}

using_carray(f, float, check_destroy); // normally omit the last argument - float type need no destroy.
using_clist(y, carray2f, carray2f_destroy, c_no_compare);
using_cmap(g, int, clist_y, clist_y_destroy);
using_cmap_strkey(s, cmap_g, cmap_g_destroy);

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 5, tableKey = 42;
    const char* strKey = "first";
    cmap_s myMap = cmap_ini;

    { // Construct.
        carray2f table = carray2f_make(ydim, xdim, 0.f);
        printf("table: (%zu, %zu)\n", carray2_ydim(table), carray2_xdim(table));
        clist_y tableList = clist_ini;
        // Put in some data.
        cmap_g listMap = cmap_ini;

        *carray2f_at(&table, y, x) = 3.1415927f; // table[y][x]
        clist_y_push_back(&tableList, table);
        cmap_g_put(&listMap, tableKey, tableList);
        cmap_s_put(&myMap, strKey, listMap);
    }
    { // Access the data entry
        carray2f table = *clist_y_back(&cmap_g_find(&cmap_s_find(&myMap, strKey)->second, tableKey)->second);
        printf("value (%d, %d) is: %f\n", y, x, *carray2f_at(&table, y, x));
    }

    c_dtor(cmap_s, &myMap); // free up everything!
}