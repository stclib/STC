#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (worker) {
    worker_base base;
    int id;
    int *count;
    cco_timer tm;
};

int worker(struct worker* co, cco_fiber* fb) {
    cco_async (co) {
        printf("Worker %d starting\n", co->id);
        cco_await_timer(&co->tm, 1.0 + co->id / 8.0);
        printf("Worker %d done: %f\n", co->id, cco_timer_elapsed(&co->tm));
    }

	*co->count -= 1;
    free(co); (void)fb;
    return 0;
}


cco_task_struct (chiller) {
    chiller_base base;
    cco_timer tm;
};

int chiller(struct chiller* co, cco_fiber* fb) {
    cco_async (co) {
        printf("Chiller starting\n");
        cco_await_timer(&co->tm, 3.0);
        printf("Chiller done: %g\n", cco_timer_elapsed(&co->tm));
    }

    free(co); (void)fb;
    return 0;
}

cco_task_struct (all_together) {
    all_together_base base;
    int wgroup;
};

int all_together(struct all_together* co, cco_fiber* fb) {
    cco_async (co) {
        struct chiller* chill = c_new(struct chiller, {{chiller}});
        cco_spawn(chill, fb);

        for (c_range32(i, 8)) {
            co->wgroup += 1;
            struct worker* work = c_new(struct worker, {.base={worker}, .id=i, .count=&co->wgroup});
            cco_spawn(work, fb);
        }
        cco_await(co->wgroup == 0); // wait for all workers to finish
        puts("All workers done.");
    }

    free(co);
    return 0;
}


int main()
{
    struct all_together* go = c_new(struct all_together, {{all_together}});
    cco_run_task(go);
}
