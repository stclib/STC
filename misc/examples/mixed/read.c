#define i_implement
#include "stc/cstr.h"

#define i_key_cstr
#include "stc/vec.h"
#include <errno.h>

vec_cstr read_file(const char* name)
{
    vec_cstr vec = {0};
    c_scoped (FILE* f = fopen(name, "r"), f != NULL, fclose(f))
        c_scoped (cstr line = {0}, cstr_drop(&line))
            while (cstr_getline(&line, f))
                vec_cstr_push(&vec, cstr_clone(line));
    return vec;
}

int main(void)
{
    int n = 0;
    c_scoped (vec_cstr vec = read_file(__FILE__), vec_cstr_drop(&vec))
        c_foreach (i, vec_cstr, vec)
            printf("%5d: %s\n", ++n, cstr_str(i.ref));

    if (errno)
        printf("error: read_file(" __FILE__ "). errno: %d\n", errno);
}
