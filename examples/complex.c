#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void check_destroy(float* v) {printf("destroy %g\n", *v);}

declare_carray(f, float, check_destroy); // normally omit the last argument - float type need no destroy.
declare_clist(t2, carray2f, carray2f_destroy, c_no_compare);
declare_cmap(il, int, clist_t2, clist_t2_destroy);
declare_cmap_str(sm, cmap_il, cmap_il_destroy);

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 5, tableKey = 42;
    const char* strKey = "first";
    cmap_sm theMap = cmap_init;

    { // Construct.
        carray2f table = carray2f_make(ydim, xdim, 0.f);
        printf("table: (%zu, %zu)\n", carray2_ydim(table), carray2_xdim(table));
        clist_t2 tableList = clist_init;
        // Put in some data.
        cmap_il listMap = cmap_init;
        
        *carray2f_at(&table, y, x) = 3.1415927; // table[y][x]
        clist_t2_push_back(&tableList, table);
        cmap_il_put(&listMap, tableKey, tableList);
        cmap_sm_put(&theMap, strKey, listMap);
    }
    { // Access the data entry
        carray2f table = clist_back(cmap_il_find(&cmap_sm_find(&theMap, strKey)->value, tableKey)->value);
        printf("value (%d, %d) is: %f\n", y, x, *carray2f_at(&table, y, x));
    }

    cmap_sm_destroy(&theMap); // free up the whole shebang!
}