
#include <stdio.h>
#include <stc/cmap.h>

static void test_destr(int* x) {
    printf("destroy int: %d\n", *x);
}

declare_CMap(ii, int, int, test_destr);
declare_CMap(im, int, CMap_ii, cmap_ii_destroy);

int main(void) {
    CMap_im m = cmap_init;
    CMap_ii ini = cmap_init;
    cmap_ii_put(&cmap_im_at(&m, 100, ini)->value, 2000, 200);
    cmap_ii_put(&cmap_im_at(&m, 100, ini)->value, 2001, 201);
    cmap_ii_put(&cmap_im_at(&m, 100, ini)->value, 2000, 400); // update
    cmap_ii_put(&cmap_im_at(&m, 110, ini)->value, 2000, 500);
    cmap_ii_put(&cmap_im_at(&m, 120, ini)->value, 3000, 600);

    c_foreach (i, cmap_im, m)
        c_foreach (j, cmap_ii, i.item->value)
            printf("%d: %d - %d\n", i.item->key, j.item->key, j.item->value);

    cmap_im_destroy(&m);
}