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

        cco_await_timer(&o->tm, 1.0 + o->id/8.0);

        cco_drop:
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

        cco_drop:
        printf("Sleeper done: %f\n", cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (everyone) {
    everyone_base base;
    struct sleeper* sleep;
    cco_group wg;
};

int everyone(struct everyone* o) {
    cco_async (o) {
        o->sleep = c_new(struct sleeper, {{sleeper}});
        cco_spawn(o->sleep);
        cco_yield; // suspend: starts sleeper task

        cco_reset_group(&o->wg);
        for (c_range32(i, 8)) { // NB: local i, do not yield or await inside loop.
            struct worker* work = c_new(struct worker, {.base={worker}, .id=i});
            cco_launch(work, &o->wg);
        }
        cco_yield;

        puts("Start");
        //cco_cancel_fiber(o->sleep);
        //cco_cancel_fiber(o);
        puts("End");

        cco_drop:
        puts("Await group");
        cco_await_any(&o->wg); // await for any (one) launched worker to finish
        puts("1 worker done.");
        cco_await_n(&o->wg, 3); // await for 3 workers to finish
        puts("3 more workers done.");
        cco_await_cancel(&o->wg); // cancel and await for remaining workers to finish
        puts("All workers done.");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct everyone* go = c_new(struct everyone, {{everyone}});
    cco_run_task(go);
    puts("Everyone done");
}
