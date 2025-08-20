// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>

#define T Tasks, cco_task*
#define i_keydrop(x) { puts("free task"); free(*x); }
#define i_no_clone
#include <stc/queue.h>

cco_task_struct (Scheduler) {
    Scheduler_base base;
    cco_task* pulled;
    Tasks tasks;
};

int Scheduler(struct Scheduler* o) {
    cco_async (o) {
        while (!Tasks_is_empty(&o->tasks)) {
            o->pulled = Tasks_pull(&o->tasks);

            int result = cco_resume(o->pulled);

            if (result == CCO_YIELD) {
                Tasks_push(&o->tasks, o->pulled);
            } else {
                Tasks_value_drop(&o->tasks, &o->pulled);
            }
        }

        cco_finalize:
        Tasks_drop(&o->tasks);
        puts("Task queue dropped");
    }
    return 0;
}


static int TaskA(struct cco_task* o) {
    cco_async (o) {
        puts("Hello, from task A");
        cco_yield;
        puts("A is back doing work");
        cco_yield;
        puts("A is back doing more work");
        cco_yield;
        puts("A is back doing even more work");

        cco_finalize:
        puts("A done");
    }

    return 0;
}


static int TaskB(struct cco_task* o) {
    cco_async (o) {
        puts("Hello, from task B");
        cco_yield;
        puts("B is back doing work");
        cco_yield;
        puts("B is back doing more work");

        cco_finalize:
        puts("B done");
    }
    return 0;
}

int main(void) {
    struct Scheduler schedule = {
        .base={Scheduler},
        .tasks = c_make(Tasks, {
            c_new(cco_task, {.base={TaskA}}),
            c_new(cco_task, {.base={TaskB}}),
        })
    };

    cco_run_task(&schedule);
}
