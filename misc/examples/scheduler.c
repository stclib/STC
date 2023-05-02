// https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/calgo.h>

struct Scheduler;
struct Task {
    bool (*resume)(struct Task*);
    struct Scheduler* sched;
    int cco_state;
};

#define i_type Scheduler
#define i_val struct Task
#define i_no_cmp
#include <stc/clist.h>

static bool schedule(Scheduler* sched)
{
    struct Task task = *Scheduler_front(sched);
    Scheduler_pop_front(sched);
    
    if (cco_alive(&task))
        task.resume(&task);
    
    return !Scheduler_empty(sched);
}

static bool resume_task(const struct Task* task)
{
    Scheduler_push_back(task->sched, *task);
    return false;
}


static bool taskA(struct Task* task)
{
    cco_begin(task);
        puts("Hello, from task A");
        cco_yield(resume_task(task));
        puts("A is back doing work");
        cco_yield(resume_task(task));
        puts("A is back doing more work");
        cco_yield(resume_task(task));
        puts("A is back doing even more work");
        cco_final:
    cco_end(true);
}

static bool taskB(struct Task* task) 
{
    cco_begin(task);
        puts("Hello, from task B");
        cco_yield(resume_task(task));
        puts("B is back doing work");
        cco_yield(resume_task(task));
        puts("B is back doing more work");
        cco_final:
    cco_end(true);
}

void Use(void)
{
    Scheduler scheduler = c_make(Scheduler, {
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
