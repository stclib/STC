#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (worker) {
    worker_base base;
    int id;
    cco_timer tm;
};

int worker(struct worker* o) {
    cco_async (o) {
        printf("Worker %d starting\n", o->id);

        cco_await_timer(&o->tm, 2.0 - o->id/8.0);
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
        printf("Sleeper done: %f\n", cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (everyone) {
    everyone_base base;
    cco_group wg;
};

int everyone(struct everyone* o) {
    cco_async (o) {
        struct sleeper* sleep = c_new(struct sleeper, {{sleeper}});
        cco_spawn(sleep);
        cco_yield; // suspend: starts sleeper task

        cco_reset_group(&o->wg); // not needed if initially 0.
        for (c_range32(i, 8)) { // NB: local i, do not yield or await inside loop.
            struct worker* work = c_new(struct worker, {.base={worker}, .id=i});
            cco_launch(work, &o->wg);
        }
        cco_await_group(&o->wg); // starts launched workers and await for them to finish
        puts("All workers done.");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct everyone* go = c_new(struct everyone, {{everyone}});
    cco_run_task(go);
}
