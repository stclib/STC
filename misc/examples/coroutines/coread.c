#define i_implement
#include "stc/cstr.h"
#include "stc/coroutine.h"
#include <errno.h>

// Read file line by line using coroutines:

struct file_read {
    const char* filename;
    int cco_state;
    FILE* fp;
    cstr line;
};

int file_read(struct file_read* g)
{
    cco_scope(g) {
        g->fp = fopen(g->filename, "r");
        if (!g->fp) cco_return;
        g->line = (cstr){0};

        cco_await( !cstr_getline(&g->line, g->fp) );

        cco_final:
        printf("finish\n");
        cstr_drop(&g->line);
        if (g->fp) fclose(g->fp);
    }
    return 0;
}

int main(void)
{
    struct file_read g = {__FILE__};
    int n = 0;
    cco_run_coroutine(file_read(&g))
    {
        printf("%3d %s\n", ++n, cstr_str(&g.line));
        //if (n == 10) cco_stop(&g);
    }
}
