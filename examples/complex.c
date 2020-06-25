#include "../stc/cstring.h"
#include "../stc/chash.h"
#include "../stc/clist.h"
#include "../stc/carray.h"

void check_destroy(float* v) {printf("destroy %g\n", *v);}

declare_CArray(f, float, check_destroy); // normally omit the last argument - float type need no destroy.
declare_CList(t2, CArray2_f, carray2_f_destroy, c_noCompare);
declare_CHash(il, MAP, int, CList_t2, clist_t2_destroy);
declare_CHash_string(sm, MAP, CHash_il, chash_il_destroy);

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 5, entry = 42;
    CHash_sm theMap = chash_init;
    {
        // Construct.
        CArray2_f table = carray2_f_make(ydim, xdim, 0.f);
        printf("table: (%zu, %zu)\n", carray2_ydim(table), carray2_xdim(table));
        CList_t2 tableList = clist_init;
        CHash_il listMap = chash_init;
        
        // Put in some data.
        carray2_f_data(table, y)[x] = 3.1415927; // table[x][y]
        clist_t2_pushBack(&tableList, table);
        chash_il_put(&listMap, entry, tableList);
        chash_sm_put(&theMap, "First", listMap);
    }

    // Access the data entry
    CArray2_f table = clist_back(chash_il_get(&chash_sm_get(&theMap, "First")->value, entry)->value);
    printf("value (%d, %d) is: %f\n", y, x, carray2_f_value(table, y, x));

    chash_sm_destroy(&theMap); // free up the whole shebang!
}