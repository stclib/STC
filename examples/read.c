#include <stc/cstr.h>
#include <errno.h>

#define i_val_str
#include <stc/cvec.h>

cvec_str read_file(const char* name)
{
    cvec_str vec = cvec_str_init();
    c_autovar (FILE* f = fopen(name, "r"), fclose(f))
        c_auto (cstr, line)
            while (cstr_getline(&line, f))
                cvec_str_emplace_back(&vec, line.str);
    return vec;
}

int main()
{
    int n = 0;
    c_autovar (cvec_str vec = read_file(__FILE__), cvec_str_del(&vec))
        c_foreach (i, cvec_str, vec)
            printf("%5d: %s\n", ++n, i.ref->str);

    if (errno)
        printf("error: read_file(" __FILE__ "). errno: %d\n", errno);
}