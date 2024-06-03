// https://github.com/lewissbaker/cppcoro#taskt

#include <time.h>
#include <stdio.h>
#define i_static
#include "stc/cstr.h"
#include "stc/coroutine.h"

cco_task_struct(file_read,
    const char* path;
    cstr line;
    FILE* fp;
    cco_timer tm;
);


int file_read(struct file_read* co, cco_runtime* rt)
{
    cco_scope (co) {
        (void)rt;
        co->fp = fopen(co->path, "r");
        co->line = cstr_init();

        while (true) {
            // emulate async io: await 10ms per line
            cco_await_timer(&co->tm, 0.010);

            if (!cstr_getline(&co->line, co->fp))
                break;
            cco_yield;
        }

        cco_final:
        fclose(co->fp);
        cstr_drop(&co->line);
        puts("done file_read");
    }
    return 0;
}


cco_task_struct(count_line,
    cstr path;
    struct file_read reader;
    int lineCount;
);


int count_line(struct count_line* co, cco_runtime* rt)
{
    cco_scope (co) {
        co->reader.cco_func = file_read;
        co->reader.path = cstr_str(&co->path);

        while (true) {
            // await for next CCO_YIELD (or CCO_DONE) in file_read()
            cco_await_task(&co->reader, rt, CCO_YIELD);
            if (rt->result == CCO_DONE) break;
            co->lineCount += 1;
            cco_yield;
        }

        cco_final:
        cstr_drop(&co->path);
        puts("done count_line");
    }
    return 0;
}


int main(void)
{
    // Creates a new task
    struct count_line countTask = {
        .cco_func = count_line,
        .path = cstr_from(__FILE__),
    };

    // Execute coroutine as top-level blocking
    int loop = 0;
    cco_run_task(&countTask) { ++loop; }

    printf("line count = %d\n", countTask.lineCount);
    printf("exec count = %d\n", loop);
}
