// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include "stc/coroutine.h"

#define i_type Tasks, cco_task*
#define i_keydrop(x) { puts("free task"); free(*x); }
#define i_no_clone
#include "stc/queue.h"

cco_task_struct (Scheduler) {
    Scheduler_state cco;
    cco_task* pulled;
    Tasks tasks;
};

cco_task_struct (TaskA) {
    TaskA_state cco;
    bool dummy;
};

int Scheduler(struct Scheduler* sc, cco_runtime* rt) {
    cco_routine (sc) {
        while (!Tasks_is_empty(&sc->tasks)) {
            sc->pulled = Tasks_pull(&sc->tasks);

            cco_await_task(sc->pulled, rt, CCO_YIELD);

            if (rt->result == CCO_YIELD) {
                Tasks_push(&sc->tasks, sc->pulled);
            } else {
                Tasks_value_drop(&sc->pulled);
            }
        }

        cco_finally:
        Tasks_drop(&sc->tasks);
        puts("Task queue dropped");
    }
    return 0;
}

static int TaskA(struct TaskA* task, cco_runtime* rt) {
    (void)rt;
    cco_routine (task) {
        puts("Hello, from task A");
        cco_yield;
        puts("A is back doing work");
        cco_yield;
        puts("A is back doing more work");
        cco_yield;
        puts("A is back doing even more work");

        cco_finally:
        puts("A done");
    }
    return 0;
}

static int TaskB(struct cco_task* task, cco_runtime* rt) {
    (void)rt;
    cco_routine (task) {
        puts("Hello, from task B");
        cco_yield;
        puts("B is back doing work");
        cco_yield;
        puts("B is back doing more work");

        cco_finally:
        puts("B done");
    }
    return 0;
}

int main(void) {
    struct Scheduler schedule = {
        .cco={Scheduler},
        .tasks = c_make(Tasks, {
            (cco_task*)c_new(struct TaskA, {{TaskA}}),
                       c_new(struct cco_task, {{TaskB}}),
        })
    };

    cco_run_task(&schedule);
}
