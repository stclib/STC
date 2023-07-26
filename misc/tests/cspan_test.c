#include <stdio.h>
#include <stc/cspan.h>
#include "ctest.h"

using_cspan3(intspan, int);


CTEST(cspan, subdim) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    intspan3 m = cspan_md(array, 2, 2, 3);
    
    for (int i = 0; i < m.shape[0]; ++i) {
        intspan2 sub_i = cspan_submd3(&m, i);
        for (int j = 0; j < m.shape[1]; ++j) {
            intspan sub_i_j = cspan_submd2(&sub_i, j);
            for (int k = 0; k < m.shape[2]; ++k) {
               ASSERT_EQ(*cspan_at(&sub_i_j, k), *cspan_at(&m, i, j, k));
            }
        }
    }
}

CTEST(cspan, slice) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    intspan2 m1 = cspan_md(array, 3, 4);

    int sum1 = 0;
    for (int i = 0; i < m1.shape[0]; ++i) {
        for (int j = 0; j < m1.shape[1]; ++j) {
            sum1 += *cspan_at(&m1, i, j);
        }
    }

    intspan2 m2 = cspan_slice(intspan2, &m1, {c_ALL}, {2,4});

    int sum2 = 0;
    for (int i = 0; i < m2.shape[0]; ++i) {
        for (int j = 0; j < m2.shape[1]; ++j) {
            sum2 += *cspan_at(&m2, i, j);
        }
    }
    ASSERT_EQ(78, sum1);
    ASSERT_EQ(45, sum2);
}

#define i_key int
#include <stc/cstack.h>

CTEST(cspan, slice2) {
    cstack_int stack = {0};
    c_defer (cstack_int_drop(&stack))
    {
        c_forrange (i, 10*20*30)
            cstack_int_push(&stack, i);

        intspan3 ms3 = cspan_md(stack.data, 10, 20, 30);
        ms3 = cspan_slice(intspan3, &ms3, {1,4}, {3,7}, {20,24});

        int sum = 0;
        for (int i = 0; i < ms3.shape[0]; ++i) {
            for (int j = 0; j < ms3.shape[1]; ++j) {
                for (int k = 0; k < ms3.shape[2]; ++k) {
                    sum += *cspan_at(&ms3, i, j, k);
                }
            }
        }
        ASSERT_EQ(65112, sum);

        sum = 0;
        c_foreach (i, intspan3, ms3)
            sum += *i.ref;
        ASSERT_EQ(65112, sum);
    }
}


#define i_type Tiles
#define i_key intspan3
#include <stc/cstack.h>

CTEST_FIXTURE(cspan_cube) {
    cstack_int stack;
    Tiles tiles;
};

CTEST_SETUP(cspan_cube) {
    enum {TSIZE=4, CUBE=64, N=CUBE*CUBE*CUBE};

    _self->stack = cstack_int_init();
    _self->tiles = Tiles_init();

    cstack_int_reserve(&_self->stack, N);
    c_forrange (i, N)
        cstack_int_push(&_self->stack, i+1);

    intspan3 ms3 = cspan_md(_self->stack.data, CUBE, CUBE, CUBE);

    c_forrange (i, 0, ms3.shape[0], TSIZE) {
        c_forrange (j, 0, ms3.shape[1], TSIZE) {
            c_forrange (k, 0, ms3.shape[2], TSIZE) {
                intspan3 tile = cspan_slice(intspan3, &ms3, {i, i + TSIZE}, {j, j + TSIZE}, {k, k + TSIZE});
                Tiles_push(&_self->tiles, tile);
            }
        }
    }
}

// Optional teardown function for suite, called after every test in suite
CTEST_TEARDOWN(cspan_cube) {
    cstack_int_drop(&_self->stack);
    Tiles_drop(&_self->tiles);
}


CTEST_F(cspan_cube, slice3) {
    long long n = cstack_int_size(&_self->stack);
    long long sum = 0;

    // iterate each 3d tile in sequence
    c_foreach (i, Tiles, _self->tiles)
        c_foreach (t, intspan3, *i.ref)
            sum += *t.ref;

    ASSERT_EQ(n*(n + 1)/2, sum);
}
