#include <stc/cstr.h>
#include <stc/coroutine.h>

#define T SVec, cstr, (c_pro_key)
#include <stc/stack.h>

// Read file line by line using coroutines:

cco_task_struct (FileRead, SVec*) {
    FileRead_base base;
    const char* filename;
    FILE* fp;
    cstr line;
};

int file_read(struct FileRead* o)
{
    cco_async (o) {
        if (!(o->fp = fopen(o->filename, "r")))
            cco_return;

        while (cstr_getline(&o->line, o->fp)) {
            SVec_push(cco_data(o), cstr_clone(o->line));
            cco_yield;
        }

        cco_finalize:
        cstr_drop(&o->line);
        if (o->fp) { fclose(o->fp); printf("done reading: %s\n", o->filename); }
        else printf("could not read: %s\n", o->filename);
    }
    return 0;
}


cco_task_struct (MainTask, SVec*) {
    MainTask_base base;
    struct FileRead reader1;
    struct FileRead reader2;
    cstr file2;
    cco_group grp; // demo, use instead of cco_grp(0).
};

int maintask(struct MainTask* o)
{
    cco_async (o) {
        o->file2 = cstr_from(__FILE__);
        cstr_replace(&o->file2, "coread.c", "waitgroup.c");
        o->reader1 = (struct FileRead){{file_read}, __FILE__};
        o->reader2 = (struct FileRead){{file_read}, cstr_str(&o->file2)};

        cco_spawn(&o->reader1, &o->grp);
        cco_spawn(&o->reader2, &o->grp);

        cco_finalize:
        cco_await_all(&o->grp);
        cstr_drop(&o->file2);
        puts("done all");
    }
    return 0;
}


int main(void)
{
    SVec output = {0};
    struct MainTask task = {{maintask}};

    cco_run_task(&task, &output);

    int n = 0;
    for (c_each(i, SVec, output)) {
        printf("%03d: %s\n", ++n, cstr_str(i.ref));
    }

    SVec_drop(&output);
}
