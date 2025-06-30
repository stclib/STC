#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (worker) {
    worker_base base;
    int id;
    int *wg; // waitgroup pointer
    cco_timer tm;
};

int worker(struct worker* co, cco_fiber* const fb) {
    cco_async (co) {
        printf("Worker %d starting\n", co->id); (void)fb;

        cco_await_timer(&co->tm, 2.0 - co->id/8.0);
        printf("Worker %d done: %f\n", co->id, cco_timer_elapsed(&co->tm));
    }

    *co->wg -= 1;
    c_free_n(co, 1);
    return 0;
}


cco_task_struct (sleeper) {
    sleeper_base base;
    cco_timer tm;
};

int sleeper(struct sleeper* co, cco_fiber* const fb) {
    cco_async (co) {
        printf("Sleeper starting\n"); (void)fb;

        cco_await_timer(&co->tm, 3.0);
        printf("Sleeper done: %f\n", cco_timer_elapsed(&co->tm));
    }

    c_free_n(co, 1);
    return 0;
}


cco_task_struct (everyone) {
    everyone_base base;
    int wg; // waitgroup
};

int everyone(struct everyone* co, cco_fiber* const fb) {
    cco_async (co) {
        struct sleeper* sleep = c_new(struct sleeper, {{sleeper}});
        cco_spawn(sleep, fb);
        cco_yield; // suspend: starts sleeper task

        cco_fiber* f2 = fb;
        for (c_range32(i, 8)) { // nb! local i, do not yield or await inside loop.
            co->wg += 1;
            struct worker* work = c_new(struct worker, {.base={worker}, .id=i, .wg=&co->wg});
            f2 = cco_spawn(work, f2); // optionally assign result to f2 to make tasks start in given order.
        }

        cco_await(co->wg == 0); // suspend: starts workers and wait for all of them to finish
        puts("All workers done.");
    }

    c_free_n(co, 1);
    return 0;
}


int main(void)
{
    struct everyone* go = c_new(struct everyone, {{everyone}});
    cco_run_task(go);
}
