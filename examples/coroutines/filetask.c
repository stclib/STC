// https://github.com/lewissbaker/cppcoro#taskt

#include <time.h>
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/coroutine.h>

cco_task_struct (file_read) {
    file_read_base base;
    const char* path;
    cstr line;
    FILE* fp;
    cco_timer tm;
};


int file_read(struct file_read* o)
{
    cco_async (o) {
        o->fp = fopen(o->path, "r");
        if (!o->fp)
            goto fail;
        o->line = cstr_init();

        while (true) {
            // emulate async io: await 1ms per line
            cco_await_timer(&o->tm, 0.001);

            if (!cstr_getline(&o->line, o->fp))
                break;
            cco_yield;
        }

        cco_finalize:
        fclose(o->fp);
        cstr_drop(&o->line);
        puts("done file_read");
        break;

        fail:
        printf("error: couldn't open file '%s'.\n", o->path);
    }
    return 0;
}


cco_task_struct (count_line) {
    count_line_base base;
    cstr path;
    struct file_read reader;
    int lineCount;
};


int count_line(struct count_line* o)
{
    cco_async (o) {
        o->reader.base.func = file_read;
        o->reader.path = cstr_str(&o->path);

        while (true) {
            // await for next CCO_YIELD (or CCO_DONE) in file_read()
            cco_await_task(&o->reader, CCO_YIELD);
            if (cco_status() == CCO_DONE) break;
            o->lineCount += 1;
            cco_yield;
        }

        cco_finalize:
        cstr_drop(&o->path);
        puts("done count_line");
    }
    return 0;
}


int main(void)
{
    // Creates a new task
    struct count_line countTask = {
        .base = {count_line},
        .path = cstr_from(__FILE__),
    };

    // Execute coroutine as top-level blocking
    int loop = 0;

    cco_run_task(&countTask) { ++loop; }

    printf("line count = %d\n", countTask.lineCount);
    printf("exec count = %d\n", loop);
}
