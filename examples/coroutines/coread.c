#include <stdlib.h>
#include <errno.h>
#include <stc/cstr.h>
#include <stc/coroutine.h>

// Read file line by line using coroutines:

struct file_read {
    const char* filename;
    FILE* fp;
    cstr line;
    cco_base base;
};

int file_read(struct file_read* o)
{
    cco_async (o) {
        o->fp = fopen(o->filename, "r");
        if (o->fp == NULL)
            cco_exit();
        o->line = (cstr){0};
        cco_await( !cstr_getline(&o->line, o->fp) );

        cco_finalize:
        cstr_drop(&o->line);
        if (o->fp) fclose(o->fp);
        puts("finish");
    }
    return 0;
}

int main(void)
{
    struct file_read reader = {.filename=__FILE__};
    int n = 0;
    cco_run_coroutine(file_read(&reader))
    {
        printf("%3d %s\n", ++n, cstr_str(&reader.line));
        //if (n == 10) cco_stop(&reader);
    }
}
