// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/calgo.h>

struct Task {
    int (*fn)(struct Task*);
    int cco_state;
    struct Scheduler* sched;
};

#define i_type Scheduler
#define i_key struct Task
#include <stc/cqueue.h>

static bool schedule(Scheduler* sched)
{
    struct Task task = *Scheduler_front(sched);
    Scheduler_pop(sched);
    
    if (!cco_done(&task))
        task.fn(&task);
    
    return !Scheduler_empty(sched);
}

static int push_task(const struct Task* task)
{
    Scheduler_push(task->sched, *task);
    return CCO_YIELD;
}


static int taskA(struct Task* task)
{
    cco_routine(task) {
        puts("Hello, from task A");
        cco_yield_v(push_task(task));
        puts("A is back doing work");
        cco_yield_v(push_task(task));
        puts("A is back doing more work");
        cco_yield_v(push_task(task));
        puts("A is back doing even more work");
    }
    return 0;
}

static int taskB(struct Task* task) 
{
    cco_routine(task) {
        puts("Hello, from task B");
        cco_yield_v(push_task(task));
        puts("B is back doing work");
        cco_yield_v(push_task(task));
        puts("B is back doing more work");
    }
    return 0;
}

void Use(void)
{
    Scheduler scheduler = c_init(Scheduler, {
        {.fn=taskA, .sched=&scheduler}, 
        {.fn=taskB, .sched=&scheduler},
    });

    while (schedule(&scheduler)) {}

    Scheduler_drop(&scheduler);
}

int main(void)
{
    Use();
}
