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


cco_task_struct (something) {
    something_base base;
    cco_timer tm;
};

int something(struct something* co, cco_fiber* fb) {
    cco_async (co) {
        printf("something starting\n");
        cco_await_timer(&co->tm, 3.0);
        printf("something done: %f\n", cco_timer_elapsed(&co->tm));
    }

    free(co); (void)fb;
    return 0;
}


cco_task_struct (ccomain) {
    ccomain_base base;
    int wgroup;
};

int ccomain(struct ccomain* co, cco_fiber* fb) {
    cco_async (co) {
        struct something* thing = c_new(struct something, {{something}});
        cco_spawn(thing, fb);

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
    struct ccomain* go = c_new(struct ccomain, {{ccomain}});
    cco_run_task(go);
}
