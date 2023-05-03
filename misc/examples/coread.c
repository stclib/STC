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

bool file_read(struct file_read* g)
{
    cco_begin(g)
        g->fp = fopen(g->filename, "r");
        g->line = cstr_init();

        while (cstr_getline(&g->line, g->fp))
            cco_yield(false);

        cco_final: // this label is required.
            printf("finish\n");
            cstr_drop(&g->line);
            fclose(g->fp);
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
