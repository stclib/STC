
#include <stdio.h>
#include <stc/chash.h>

static void test_destr(int* x) {
    printf("destroy int: %d\n", *x);
}

declare_CMap(ii, int, int, test_destr);
declare_CMap(im, int, CMap_ii, cmap_ii_destroy);

int main(void) {
    CMap_im m = cmap_init;
    CMap_ii x = cmap_init;
    cmap_ii_put(&cmap_im_put(&m, 100, x)->value, 200, 300);
    cmap_ii_put(&cmap_im_get(&m, 100)->value, 200, 400); // update
    cmap_ii_put(&cmap_im_put(&m, 110, x)->value, 200, 500);

    cmap_im_destroy(&m);
}