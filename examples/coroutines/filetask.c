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


int file_read(struct file_read* co, cco_fiber* fb)
{
    cco_async (co) {
        (void)fb;
        co->fp = fopen(co->path, "r");
        co->line = cstr_init();

        while (true) {
            // emulate async io: await 1ms per line
            cco_await_timer(&co->tm, 0.001);

            if (!cstr_getline(&co->line, co->fp))
                break;
            cco_yield;
        }
    }

    fclose(co->fp);
    cstr_drop(&co->line);
    puts("done file_read");
    return 0;
}


cco_task_struct (count_line) {
    count_line_base base;
    cstr path;
    struct file_read reader;
    int lineCount;
};


int count_line(struct count_line* co, cco_fiber* fb)
{
    cco_async (co) {
        co->reader.base.func = file_read;
        co->reader.path = cstr_str(&co->path);

        while (true) {
            // await for next CCO_YIELD (or CCO_DONE) in file_read()
            cco_await_task(&co->reader, fb, CCO_YIELD);
            if (fb->result == CCO_DONE) break;
            co->lineCount += 1;
            cco_yield;
        }
    }

    cstr_drop(&co->path);
    puts("done count_line");
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
