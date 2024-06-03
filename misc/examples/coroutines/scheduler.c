// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include "stc/coroutine.h"

#define i_TYPE Tasks, struct cco_task*
#define i_keydrop(x) { puts("free task"); free(*x); }
#define i_no_clone
#include "stc/queue.h"

typedef struct {
    Tasks tasks;
} Scheduler;

void Scheduler_drop(Scheduler* sched) {
    Tasks_drop(&sched->tasks);
}

int Scheduler_run(Scheduler* sched) {
    while (!Tasks_is_empty(&sched->tasks)) {
        struct cco_task* task = Tasks_pull(&sched->tasks);
        if (cco_resume_task(task, NULL))
            Tasks_push(&sched->tasks, task);
        else
            Tasks_value_drop(&task);
    }
    return 0;
}

static int taskA(struct cco_task* task, cco_runtime* rt) {
    (void)rt;
    cco_scope(task) {
        puts("Hello, from task A");
        cco_yield;
        puts("A is back doing work");
        cco_yield;
        puts("A is back doing more work");
        cco_yield;
        puts("A is back doing even more work");
    }
    return 0;
}

static int taskB(struct cco_task* task, cco_runtime* rt) {
    (void)rt;
    cco_scope(task) {
        puts("Hello, from task B");
        cco_yield;
        puts("B is back doing work");
        cco_yield;
        puts("B is back doing more work");
    }
    return 0;
}

void Use(void) {
    Scheduler sched = {.tasks = c_init(Tasks, {
        c_new(struct cco_task, {.cco_func=taskA}),
        c_new(struct cco_task, {.cco_func=taskB}),
    })};

    Scheduler_run(&sched);
    Scheduler_drop(&sched);
}

int main(void)
{
    Use();
}
