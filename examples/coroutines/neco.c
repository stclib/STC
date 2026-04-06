#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (TickTock) {
    TickTock_base base;
    cco_timer tm;
};


cco_task_struct (Maintask) {
    Maintask_base base;
    cco_timer tm;
    struct TickTock tick, tock;
};


int ticker(struct TickTock* o) {
    cco_async (o) {
        for (;;) {
            cco_await_timer(&o->tm, 0.1);
            puts("tick");
        }
        cco_finalize:
        puts("drop tick");
    }
    return 0;
}

int tocker(struct TickTock* o) {
    cco_async (o) {
        for (;;) {
            cco_await_timer(&o->tm, 0.15);
            puts("TOCK");
        }
        cco_finalize:
        puts("drop TOCK");
    }
    return 0;
}

int maintask(struct Maintask* o) {
    cco_async (o) {
        o->tick = (struct TickTock){{ticker}};
        o->tock = (struct TickTock){{tocker}};
        cco_spawn(&o->tick, cco_grp(0));
        cco_spawn(&o->tock, cco_grp(0));

        cco_await_timer(&o->tm, 0.5);
        cco_throw(cco_CANCEL);
        cco_await_timer(&o->tm, 0.5);

        cco_finalize:
        cco_await_cancel_all(cco_grp(0));
        puts("done");
    }
    return 0;
}

int main(void) {
    cco_run_task(&(struct Maintask){{maintask}});
}
