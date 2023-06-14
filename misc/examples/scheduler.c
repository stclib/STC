// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/calgo.h>

cco_closure(bool, Task,
    struct Scheduler* sched;
);

#define i_type Scheduler
#define i_val struct Task
#define i_no_cmp
#include <stc/cqueue.h>

static bool schedule(Scheduler* sched)
{
    struct Task task = *Scheduler_front(sched);
    Scheduler_pop(sched);
    
    if (!cco_done(&task))
        cco_resume(&task);
    
    return !Scheduler_empty(sched);
}

static bool push_task(const struct Task* task)
{
    Scheduler_push(task->sched, *task);
    return false;
}


static bool taskA(struct Task* task)
{
    cco_routine(task) {
        puts("Hello, from task A");
        cco_yield(push_task(task));
        puts("A is back doing work");
        cco_yield(push_task(task));
        puts("A is back doing more work");
        cco_yield(push_task(task));
        puts("A is back doing even more work");
    }
    return true;
}

static bool taskB(struct Task* task) 
{
    cco_routine(task) {
        puts("Hello, from task B");
        cco_yield(push_task(task));
        puts("B is back doing work");
        cco_yield(push_task(task));
        puts("B is back doing more work");
    }
    return true;
}

void Use(void)
{
    Scheduler scheduler = c_init(Scheduler, {
        {taskA, &scheduler}, 
        {taskB, &scheduler},
    });

    while (schedule(&scheduler)) {}

    Scheduler_drop(&scheduler);
}

int main()
{
    Use();
}
