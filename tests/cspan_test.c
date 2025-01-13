#include <stdio.h>
#include "stc/cspan.h"
#include "ctest.h"

using_cspan3_with_eq(Span, int, c_default_eq);

TEST(cspan, subdim) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Span3 m = cspan_md(array, 2, 2, 3);

    for (c_range(i, m.shape[0])) {
        Span2 sub_i = cspan_submd3(&m, i);
        for (c_range(j, m.shape[1])) {
            Span sub_i_j = cspan_submd2(&sub_i, j);
            for (c_range(k, m.shape[2])) {
               EXPECT_EQ(*cspan_at(&sub_i_j, k), *cspan_at(&m, i, j, k));
            }
        }
    }
}


TEST(cspan, slice) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Span2 m1 = cspan_md(array, 3, 4);

    int sum1 = 0;
    for (c_range(i, m1.shape[0])) {
        for (c_range(j, m1.shape[1])) {
            sum1 += *cspan_at(&m1, i, j);
        }
    }

    Span2 m2 = cspan_slice(Span2, &m1, {c_ALL}, {2,4});

    int sum2 = 0;
    for (c_range(i, m2.shape[0])) {
        for (c_range(j, m2.shape[1])) {
            sum2 += *cspan_at(&m2, i, j);
        }
    }
    EXPECT_EQ(78, sum1);
    EXPECT_EQ(45, sum2);
}


#define i_type Stack, int
#include "stc/stack.h"

TEST(cspan, slice2) {
    c_with (Stack stack = {0}, Stack_drop(&stack))
    {
        for (c_range32(i, 10 * 20 * 30))
            Stack_push(&stack, i);

        Span3 ms3 = cspan_md(stack.data, 10, 20, 30);
        ms3 = cspan_slice(Span3, &ms3, {1,4}, {3,7}, {20,24});

        int sum = 0;
        for (c_range(i, ms3.shape[0])) {
            for (c_range(j, ms3.shape[1])) {
                for (c_range(k, ms3.shape[2])) {
                    sum += *cspan_at(&ms3, i, j, k);
                }
            }
        }
        EXPECT_EQ(65112, sum);

        sum = 0;
        for (c_each(i, Span3, ms3))
            sum += *i.ref;
        EXPECT_EQ(65112, sum);
    }
}


TEST(cspan, equality) {
    Span base = c_make(Span, {
        3, 1, 2, 3, 1,
        2, 4, 5, 6, 2,
        1, 7, 8, 9, 3,
        1, 3, 1, 2, 3,
        2, 2, 4, 5, 6,
        3, 1, 7, 8, 9,
    });
    Span2 base2 = cspan_md(base.data, 6, 5);

    Span test = c_make(Span, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
    });
    Span2 test2 = cspan_md(test.data, 3, 3);

    //puts(""); cspan_print(Span2, cspan_slice(Span2, &base2, {0, 3}, {1, 4}), "%d");

    // Test every 3x3 subtile in base2 against the test2 tile.
    for (c_range(y, base2.shape[0] - 3 + 1)) {
        for (c_range(x, base2.shape[1] - 3 + 1)) {
            bool expect_eq = (y == 0 && x == 1) || (y == 3 && x == 2);
            EXPECT_EQ(expect_eq, Span2_equals(test2, cspan_slice(Span2, &base2, {y, y+3}, {x, x+3})));
        }
    }

    // Check that the two 3x4 tiles are equal.
    EXPECT_TRUE(Span2_equals(cspan_slice(Span2, &base2, {0, 3}, {0, 4}),
                             cspan_slice(Span2, &base2, {3, 6}, {1, 5})));
}


#define i_type Tiles, Span3
#include "stc/stack.h"

TEST_FIXTURE(cspan_cube) {
    Stack stack;
    Tiles tiles;
};

TEST_SETUP(cspan_cube, fixt) {
    enum {TSIZE = 4, CUBE = 64, N = CUBE*CUBE*CUBE};

    fixt->stack = Stack_init();
    fixt->tiles = Tiles_init();

    Stack_reserve(&fixt->stack, N);
    for (c_range32(i, N))
        Stack_push(&fixt->stack, i+1);

    Span3 ms3 = cspan_md(fixt->stack.data, CUBE, CUBE, CUBE);

    for (c_range(i, 0, ms3.shape[0], TSIZE)) {
        for (c_range(j, 0, ms3.shape[1], TSIZE)) {
            for (c_range(k, 0, ms3.shape[2], TSIZE)) {
                Span3 tile = cspan_slice(Span3, &ms3, {i, i + TSIZE}, {j, j + TSIZE}, {k, k + TSIZE});
                Tiles_push(&fixt->tiles, tile);
            }
        }
    }
}

// Optional teardown function for suite, called after every test in suite
TEST_TEARDOWN(cspan_cube, fixt) {
    Stack_drop(&fixt->stack);
    Tiles_drop(&fixt->tiles);
}


TEST_F(cspan_cube, slice3, fixt) {
    int64 n = Stack_size(&fixt->stack);
    int64 sum = 0;

    // iterate each 3d tile in sequence
    for (c_each(tile, Tiles, fixt->tiles))
        for (c_each(i, Span3, *tile.ref))
            sum += *i.ref;

    EXPECT_EQ(sum, n*(n + 1)/2);
}
