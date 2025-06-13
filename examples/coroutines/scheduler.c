// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>

#define T Tasks, cco_task*
#define i_keydrop(x) { puts("free task"); free(*x); }
#define i_no_clone
#include <stc/queue.h>

cco_task_struct (Scheduler) {
    Scheduler_state cco;
    cco_task* pulled;
    Tasks tasks;
};

int Scheduler(struct Scheduler* co, cco_fiber* fb) {
    cco_routine (co) {
        while (!Tasks_is_empty(&co->tasks)) {
            co->pulled = Tasks_pull(&co->tasks);

            cco_await_task(co->pulled, fb, CCO_YIELD);

            if (fb->result == CCO_YIELD) {
                Tasks_push(&co->tasks, co->pulled);
            } else {
                Tasks_value_drop(&co->tasks, &co->pulled);
            }
        }

        cco_cleanup:
        Tasks_drop(&co->tasks);
        puts("Task queue dropped");
    }
    return 0;
}


static int TaskA(struct cco_task* co, cco_fiber* fb) {
    (void)fb;
    cco_routine (co) {
        puts("Hello, from task A");
        cco_yield;
        puts("A is back doing work");
        cco_yield;
        puts("A is back doing more work");
        cco_yield;
        puts("A is back doing even more work");

        cco_cleanup:
        puts("A done");
    }
    return 0;
}


static int TaskB(struct cco_task* co, cco_fiber* fb) {
    (void)fb;
    cco_routine (co) {
        puts("Hello, from task B");
        cco_yield;
        puts("B is back doing work");
        cco_yield;
        puts("B is back doing more work");

        cco_cleanup:
        puts("B done");
    }
    return 0;
}

int main(void) {
    struct Scheduler schedule = {
        .cco={Scheduler},
        .tasks = c_make(Tasks, {
            c_new(cco_task, {.cco={TaskA}}),
            c_new(cco_task, {.cco={TaskB}}),
        })
    };

    cco_run_task(&schedule);
}
