#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (worker) {
    worker_state cco;
    int id;
    int *wg;
    cco_timer tm;
};

int worker(struct worker* co, cco_fiber* fb) {
    cco_routine(co) {
        printf("Worker %d starting\n", co->id);
        cco_await_timer(&co->tm, 1.0 + co->id / 4.0);
        printf("Worker %d done\n", co->id);

        cco_cleanup:
        *co->wg -= 1;
    }
    free(co);
    return 0;
}


cco_task_struct (something) {
    something_state cco;
    cco_timer tm;
};

int something(struct something* co, cco_fiber* fb) {
    cco_routine(co) {
        printf("something starting\n");
        cco_await_timer(&co->tm, 3.0);
        printf("something done\n");

        cco_cleanup:
    }
    free(co);
    return 0;
}


cco_task_struct (ccomain) {
    ccomain_state cco;
    int wgroup1;
};

int ccomain(struct ccomain* co, cco_fiber* fb) {
    cco_routine(co) {
        struct something* thing = c_new(struct something, {{something}});
        cco_spawn(thing, fb);

        for (c_range(i, 8)) {
            co->wgroup1 += 1;
            struct worker* work = c_new(struct worker, {.cco={worker}, .id=i, .wg=&co->wgroup1});
            cco_spawn(work, fb);
        }
        cco_await(co->wgroup1 == 0); // wait for all workers to finish
        puts("All workers completed.");
    }
    free(co);
    return 0;
}


int main()
{
    struct ccomain* go = c_new(struct ccomain, {{ccomain}});
    cco_run_task(go);
}
