#include <stc/cstr.h>
#include <stc/algo/ccoro.h>
#include <errno.h>

// Read file line by line using coroutines:

struct file_nextline {
    const char* filename;
    int ccoro_state;
    FILE* fp;
    cstr line;
};

cstr file_nextline(struct file_nextline* U)
{
    ccoro_execute(U,
        U->fp = fopen(U->filename, "r");
        U->line = cstr_NULL;

        while (cstr_getline(&U->line, U->fp))
            ccoro_yield (cstr_clone(U->line));

        ccoro_final: // ccoro_final is needed to support ccoro_stop.
            printf("finish\n");
            cstr_drop(&U->line);
            fclose(U->fp);
    );
    return cstr_NULL;
}


int main(void) {
    struct file_nextline z = {__FILE__};
    int n = 0;
    do {
        c_with (cstr line = file_nextline(&z), ccoro_alive(&z), cstr_drop(&line)) {
            printf("%3d %s\n", ++n, cstr_str(&line));

            // stop after 15 lines:
            if (n == 15) file_nextline(ccoro_stop(&z));
        }
    } while (ccoro_alive(&z));
}
