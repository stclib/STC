#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (Task1) {
    Task1_base base;
    int val;
    cco_channel_t(int) ch;
    cco_timer tmr;
};

cco_task_struct (Task2) {
    Task2_base base;
    struct Task1* task1;
    int val;
    cco_timer tmr;
};

int task1(struct Task1* o) {
    cco_async (o) {
        cco_await_get(&o->ch, &o->val);
        printf("task1 got %d\n", o->val);

        cco_await_timer(&o->tmr, 0.5);
        cco_await_put(&o->ch, o->val*12);
    }
    return 0;
}

int task2(struct Task2* o) {
    cco_async (o) {
        cco_await_timer(&o->tmr, 0.4);
        cco_await_put(&o->task1->ch, 42);

        cco_await_get(&o->task1->ch, &o->val);
        printf("task2 got %d\n", o->val);
    }
    return 0; 
}

int main(void)
{
    cco_fiber* f = c_new(cco_fiber, {0});
    struct Task1 t1 = {{task1}};
    struct Task2 t2 = {{task2}, &t1};

    cco_spawn(&t1, NULL, NULL, f);
    cco_spawn(&t2, NULL, NULL, f);
    cco_run_fiber(&f);
}