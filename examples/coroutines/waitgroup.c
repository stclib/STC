#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (worker) {
    worker_base base;
    int id; // parameter
    cco_timer tm;
};

int worker(struct worker* o) {
    cco_async (o) {
        printf("Worker %d starting\n", o->id);
        cco_await_timer(&o->tm, 1.0 + o->id/8.0);

        cco_finalize:
        printf("Worker %d done: %f\n", o->id, cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (sleeper) {
    sleeper_base base;
    cco_timer tm;
};

int sleeper(struct sleeper* o) {
    cco_async (o) {
        printf("Sleeper starting\n");
        cco_await_timer(&o->tm, 3.0);

        cco_finalize:
        printf("Sleeper done: %f\n", cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (maintask) {
    maintask_base base;
    struct sleeper* sleep;
    cco_group group;
};

int maintask(struct maintask* o) {
    cco_async (o) {
        o->sleep = c_new(struct sleeper, {.base={sleeper}});
        cco_spawn(o->sleep);
        cco_suspend; // allow sleep-task to start

        for (c_range32(i, 8)) { // NB: local i, do not yield or await inside loop.
            struct worker* work = c_new(struct worker, {.base={worker}, .id=i});
            cco_spawn(work, &o->group);
        }
        // Start workers:
        cco_suspend;

        cco_finalize:
        puts("Await 1");
        cco_await_n(1, &o->group); // await for 1 launched worker to finish
        puts("1 worker done.");
        cco_await_n(3, &o->group); // await for 3 more workers to finish
        puts("3 more workers done");
        cco_await_all_of(&o->group); // await for remaining workers to finish
        // cancel them instead: cco_await_cancel_all_of(&o->group);
        puts("All workers done.");
        cco_await_all(); // await sleeper.
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct maintask* go = c_new(struct maintask, {{maintask}});
    cco_run_task(go);
    puts("maintask done");
}
