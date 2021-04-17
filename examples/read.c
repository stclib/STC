#include <errno.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

using_cvec_str();

cvec_str read_file(const char* name) {
    cvec_str vec = cvec_str_init();
    for (FILE* f = fopen(name, "r"); f; fclose(f), f=NULL) {
        cstr line = cstr_init();
        while (cstr_getline(&line, f))
            cvec_str_emplace_back(&vec, line.str);
        cstr_del(&line);
    }
    return vec;
}

int main() {
    cvec_str vec = read_file("read.c");
    if (errno) printf("errno: %d\n", errno);

    int n = 0;
    c_foreach (i, cvec_str, vec)
        printf("%5d: %s\n", ++n, i.ref->str);

    cvec_str_del(&vec);
}