#include <stdio.h>

#define i_val int
#include <stc/carr2.h>

#define i_val int
#include <stc/carr3.h>

int main()
{
    int w = 7, h = 5, d = 3;

    c_autovar (carr2_int image = carr2_int_init(w, h), carr2_int_del(&image))
    {
        int *dat = carr2_int_data(&image);
        for (size_t i = 0; i < carr2_int_size(image); ++i)
            dat[i] = i;

        for (size_t x = 0; x < image.xdim; ++x)
            for (size_t y = 0; y < image.ydim; ++y)
                printf(" %d", image.data[x][y]);
        puts("");

        c_foreach (i, carr2_int, image)
            printf(" %d", *i.ref);
    }
    puts("\n");

    c_autovar (carr3_int image = carr3_int_init(w, h, d), carr3_int_del(&image))
    {
        int *dat = carr3_int_data(&image);
        for (size_t i = 0; i < carr3_int_size(image); ++i)
            dat[i] = i;

        for (size_t x = 0; x < image.xdim; ++x)
            for (size_t y = 0; y < image.ydim; ++y)
                for (size_t z = 0; z < image.zdim; ++z)
                    printf(" %d", image.data[x][y][z]);
        puts("");

        c_foreach (i, carr3_int, image)
            printf(" %d", *i.ref);
    }
    puts("");
}
