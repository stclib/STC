#include <stdio.h>
#include <stc/coroutine.h>

typedef cco_channel_t(int) intchan_t;

cco_task_struct (Task1) {
    Task1_base base;
    int val;
    intchan_t ch;
    cco_timer tmr;
};

cco_task_struct (Task2) {
    Task2_base base;
    intchan_t *ch_ref;
    int val;
    cco_timer tmr;
};

int task1(struct Task1* o) {
    cco_async (o) {
        cco_start_timer(&o->tmr, 0.0);
        cco_await_get(&o->ch, &o->val);
        printf("task1 got %d, %f sec\n", o->val, cco_timer_elapsed(&o->tmr));

        cco_await(cco_timer_elapsed(&o->tmr) > 1.0);
        cco_await_put(&o->ch, o->val*12);
    }
    return 0;
}

int task2(struct Task2* o) {
    cco_async (o) {
        cco_start_timer(&o->tmr, 0.0);
        cco_await(cco_timer_elapsed(&o->tmr) > 0.4);
        cco_await_put(o->ch_ref, 42);

        cco_await_get(o->ch_ref, &o->val);
        printf("task2 got %d, %f sec\n", o->val, cco_timer_elapsed(&o->tmr));
    }
    return 0;
}

int main(void)
{
    cco_fiber* f = c_new(cco_fiber, {0});
    struct Task1 t1 = {{task1}};
    struct Task2 t2 = {{task2}, &t1.ch};

    cco_spawn(&t1, NULL, NULL, f);
    cco_spawn(&t2, NULL, NULL, f);
    cco_run_fiber(&f);
}
