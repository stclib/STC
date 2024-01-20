#include <stdio.h>
#include "stc/cspan.h"
#include "ctest.h"

using_cspan3(Span, int);


TEST(cspan, subdim) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Span3 m = cspan_md(array, 2, 2, 3);

    for (int i = 0; i < m.shape[0]; ++i) {
        Span2 sub_i = cspan_submd3(&m, i);
        for (int j = 0; j < m.shape[1]; ++j) {
            Span sub_i_j = cspan_submd2(&sub_i, j);
            for (int k = 0; k < m.shape[2]; ++k) {
               ASSERT_EQ(*cspan_at(&sub_i_j, k), *cspan_at(&m, i, j, k));
            }
        }
    }
}

TEST(cspan, slice) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Span2 m1 = cspan_md(array, 3, 4);

    int sum1 = 0;
    for (int i = 0; i < m1.shape[0]; ++i) {
        for (int j = 0; j < m1.shape[1]; ++j) {
            sum1 += *cspan_at(&m1, i, j);
        }
    }

    Span2 m2 = cspan_slice(Span2, &m1, {c_ALL}, {2,4});

    int sum2 = 0;
    for (int i = 0; i < m2.shape[0]; ++i) {
        for (int j = 0; j < m2.shape[1]; ++j) {
            sum2 += *cspan_at(&m2, i, j);
        }
    }
    EXPECT_EQ(78, sum1);
    EXPECT_EQ(45, sum2);
}

#define i_TYPE Stack,int
#include "stc/stack.h"

TEST(cspan, slice2) {
    c_scoped (Stack stack = {0}, Stack_drop(&stack))
    {
        c_forrange (i, 10 * 20 * 30)
            Stack_push(&stack, i);

        Span3 ms3 = cspan_md(stack.data, 10, 20, 30);
        ms3 = cspan_slice(Span3, &ms3, {1,4}, {3,7}, {20,24});

        int sum = 0;
        for (int i = 0; i < ms3.shape[0]; ++i) {
            for (int j = 0; j < ms3.shape[1]; ++j) {
                for (int k = 0; k < ms3.shape[2]; ++k) {
                    sum += *cspan_at(&ms3, i, j, k);
                }
            }
        }
        EXPECT_EQ(65112, sum);

        sum = 0;
        c_foreach (i, Span3, ms3)
            sum += *i.ref;
        EXPECT_EQ(65112, sum);
    }
}


#define i_TYPE Tiles,Span3
#include "stc/stack.h"

TEST_FIXTURE(cspan_cube) {
    Stack stack;
    Tiles tiles;
};

TEST_SETUP(cspan_cube) {
    enum {TSIZE=4, CUBE=64, N=CUBE*CUBE*CUBE};

    self->stack = Stack_init();
    self->tiles = Tiles_init();

    Stack_reserve(&self->stack, N);
    c_forrange (i, N)
        Stack_push(&self->stack, i+1);

    Span3 ms3 = cspan_md(self->stack.data, CUBE, CUBE, CUBE);

    c_forrange (i, 0, ms3.shape[0], TSIZE) {
        c_forrange (j, 0, ms3.shape[1], TSIZE) {
            c_forrange (k, 0, ms3.shape[2], TSIZE) {
                Span3 tile = cspan_slice(Span3, &ms3, {i, i + TSIZE}, {j, j + TSIZE}, {k, k + TSIZE});
                Tiles_push(&self->tiles, tile);
            }
        }
    }
}

// Optional teardown function for suite, called after every test in suite
TEST_TEARDOWN(cspan_cube) {
    Stack_drop(&self->stack);
    Tiles_drop(&self->tiles);
}


TEST_F(cspan_cube, slice3) {
    int64_t n = Stack_size(&self->stack);
    int64_t sum = 0;

    // iterate each 3d tile in sequence
    c_foreach (tile, Tiles, self->tiles)
        c_foreach (i, Span3, *tile.ref)
            sum += *i.ref;

    ASSERT_EQ(sum, n*(n + 1)/2);
}
