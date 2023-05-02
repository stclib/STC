#include <stc/cstr.h>
#include <stc/algo/coroutine.h>
#include <errno.h>

// Read file line by line using coroutines:

struct file_read {
    const char* filename;
    int cco_state;
    FILE* fp;
    cstr line;
};

bool file_read(struct file_read* U)
{
    cco_begin(U)
        U->fp = fopen(U->filename, "r");
        U->line = cstr_init();

        while (cstr_getline(&U->line, U->fp))
            cco_yield(false);

        cco_final: // this label is required.
            printf("finish\n");
            cstr_drop(&U->line);
            fclose(U->fp);
    cco_end(true);
}

int main(void)
{
    struct file_read g = {__FILE__};
    int n = 0;
    while (!file_read(&g))
    {
        printf("%3d %s\n", ++n, cstr_str(&g.line));
        //if (n == 10) cco_stop(&it);
    }
}
