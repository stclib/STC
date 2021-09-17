#include <stdio.h>

#define i_tag i
#define i_val int
#include <stc/carr2.h>

#define i_val int
#include <stc/carr3.h>
#include <stdio.h>

int main()
{
    int w = 7, h = 5, d = 3;

    c_forvar (carr2_i image = carr2_i_init(w, h), carr2_i_del(&image))
    {
        int *dat = carr2_i_data(&image);
        for (int i = 0; i < carr2_i_size(image); ++i)
            dat[i] = i;

        for (int x = 0; x < image.xdim; ++x)
            for (int y = 0; y < image.ydim; ++y)
                printf(" %d", image.data[x][y]);
        puts("");

        c_foreach (i, carr2_i, image)
            printf(" %d", *i.ref);
    }
    puts("\n");

    c_forvar (carr3_int image = carr3_int_init(w, h, d), carr3_int_del(&image))
    {
        int *dat = carr3_int_data(&image);
        for (int i = 0; i < carr3_int_size(image); ++i)
            dat[i] = i;

        for (int x = 0; x < image.xdim; ++x)
            for (int y = 0; y < image.ydim; ++y)
                for (int z = 0; z < image.zdim; ++z)
                    printf(" %d", image.data[x][y][z]);
        puts("");

        c_foreach (i, carr3_int, image)
            printf(" %d", *i.ref);
    }
    puts("");
}
