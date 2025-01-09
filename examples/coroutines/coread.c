#include <stdlib.h>
#include <errno.h>
#include "stc/cstr.h"
#include "stc/coroutine.h"

// Read file line by line using coroutines:

struct file_read {
    const char* filename;
    FILE* fp;
    cstr line;
    cco_state cco;
};

int file_read(struct file_read* g)
{
    cco_routine (g) {
        g->fp = fopen(g->filename, "r");
        if (g->fp == NULL) cco_return;
        g->line = (cstr){0};

        cco_await( !cstr_getline(&g->line, g->fp) );

        cco_finally:
        printf("finish\n");
        cstr_drop(&g->line);
        if (g->fp) fclose(g->fp);
    }
    return 0;
}

int main(void)
{
    struct file_read g = {.filename=__FILE__};
    int n = 0;
    cco_run_coroutine(file_read(&g))
    {
        printf("%3d %s\n", ++n, cstr_str(&g.line));
        //if (n == 10) cco_stop(&g);
    }
}
