#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (ticktock) {
    ticktock_base base;
    cco_timer tm;
};

int ticker(struct ticktock* o) {
    cco_async (o) {
        while (1) {
            cco_await_timer(&o->tm, 1.0);
            puts("tick");
        }
        cco_finalize:
        puts("drop tick");
    }
    return 0;
}

int tocker(struct ticktock* o) {
    cco_async (o) {
        while (1) {
            cco_await_timer(&o->tm, 2.0);
            puts("tock");
        }
        cco_finalize:
        puts("drop tock");
    }
    return 0;
}

int main(void) {
    struct ticktock tick = {{ticker}}, tock = {{tocker}};
    cco_fiber* fb = c_new(cco_fiber, {0});
    cco_spawn(&tick, NULL, NULL, fb);
    cco_spawn(&tock, NULL, NULL, fb);

    cco_timer tm = cco_make_timer(5.0);
    cco_run_fiber(&fb) {
        if (cco_timer_expired(&tm)) {
            cco_stop(fb->task);
        }
    }
}
