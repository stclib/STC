// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>

#define i_type cco_tasks
#define i_key cco_task*
#define i_keydrop(x) { puts("free task"); free(*x); }
#define i_no_clone
#include <stc/cqueue.h>

typedef struct {
    cco_tasks tasks;
} cco_scheduler;

void cco_scheduler_drop(cco_scheduler* sched) {
    cco_tasks_drop(&sched->tasks);
}

int cco_scheduler_run(cco_scheduler* sched) {
    while (!cco_tasks_empty(&sched->tasks)) {
        cco_task* task = cco_tasks_pull(&sched->tasks);
        if (cco_resume_task(task, NULL))
            cco_tasks_push(&sched->tasks, task);
        else
            cco_tasks_value_drop(&task);
    }
    return 0;
}

static int taskA(cco_task* task, cco_runtime* rt) {
    cco_routine(task) {
        puts("Hello, from task A");
        cco_yield();
        puts("A is back doing work");
        cco_yield();
        puts("A is back doing more work");
        cco_yield();
        puts("A is back doing even more work");
    }
    return 0;
}

static int taskB(cco_task* task, cco_runtime* rt) {
    cco_routine(task) {
        puts("Hello, from task B");
        cco_yield();
        puts("B is back doing work");
        cco_yield();
        puts("B is back doing more work");
    }
    return 0;
}

void Use(void) {
    cco_scheduler sched = {.tasks = c_init(cco_tasks, {
        c_new(cco_task, {.cco_func=taskA}), 
        c_new(cco_task, {.cco_func=taskB}),
    })};

    cco_scheduler_run(&sched);
    cco_scheduler_drop(&sched);
}

int main(void)
{
    Use();
}
