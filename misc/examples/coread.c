#include <stc/cstr.h>
#include <stc/algo/cco.h>
#include <errno.h>

// Read file line by line using coroutines:

cstr file_nextline(ccontext* ccx, const char* name)
{
    cco_context(ccx,
        FILE* fp;
        cstr line;
    );

    cco_routine(U,
        U->fp = fopen(name, "r");
        U->line = cstr_NULL;

        while (cstr_getline(&U->line, U->fp))
            cco_yield (cstr_clone(U->line));

        cco_finish: // cco_finish is needed to support cco_stop.
            printf("finish\n");
            cstr_drop(&U->line);
            fclose(U->fp);
    );
    return cstr_NULL;
}


int main(void) {
    ccontext z = 0;
    int n = 0;
    do {
        c_with (cstr line = file_nextline(&z, __FILE__), z, cstr_drop(&line)) {
            printf("%3d %s\n", ++n, cstr_str(&line));

            // stop after 10 lines:
            if (n == 10) file_nextline(cco_stop(&z), __FILE__);
        }
    } while (z);
}
