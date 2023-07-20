#define i_implement
#include <stc/cstr.h>
#include <stc/algo/raii.h>
#define i_key_str
#include <stc/cvec.h>
#include <errno.h>

cvec_str read_file(const char* name)
{
    cvec_str vec = {0};
    c_with (FILE* f = fopen(name, "r"), fclose(f))
        c_with (cstr line = {0}, cstr_drop(&line))
            while (cstr_getline(&line, f))
                cvec_str_push(&vec, cstr_clone(line));
    return vec;
}

int main(void)
{
    int n = 0;
    c_with (cvec_str vec = read_file(__FILE__), cvec_str_drop(&vec))
        c_foreach (i, cvec_str, vec)
            printf("%5d: %s\n", ++n, cstr_str(i.ref));

    if (errno)
        printf("error: read_file(" __FILE__ "). errno: %d\n", errno);
}
