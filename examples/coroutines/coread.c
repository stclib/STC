#include <stdlib.h>
#include <stc/cstr.h>
#include <stc/coroutine.h>

// Read file line by line using coroutines:

struct file_read {
    cco_base base;
    const char* filename;
    FILE* fp;
    cstr line;
};
int file_read(struct file_read* o)
{
    cco_async (o) {
        o->fp = fopen(o->filename, "r");
        if (o->fp == NULL)
            cco_exit();
        o->line = cstr_init();
        cco_await( !cstr_getline(&o->line, o->fp) );

        cco_finalize:
        puts("finish");
        cstr_drop(&o->line);
        if (o->fp) fclose(o->fp);
    }
    return 0;
}

int main(void)
{
    struct file_read reader = {.filename=__FILE__};
    int n = 0;
    while (file_read(&reader))
    {
        printf("%3d %s\n", ++n, cstr_str(&reader.line));
        if (n == 10) cco_stop(&reader);
    }
}
