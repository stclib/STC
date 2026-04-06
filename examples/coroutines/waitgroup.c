#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (Worker) {
    Worker_base base;
    int id; // parameter
    cco_timer tm;
};

int worker(struct Worker* o) {
    cco_async (o) {
        printf("Worker %d starting\n", o->id);
        cco_await_timer(&o->tm, o->id/8.0);

        cco_finalize:
        printf("Worker %d done: %f\n", o->id, cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (Sleeper) {
    Sleeper_base base;
    cco_timer tm;
};

int sleeper(struct Sleeper* o) {
    cco_async (o) {
        printf("Sleeper starting\n");
        cco_await_timer(&o->tm, 1.1);

        cco_finalize:
        printf("Sleeper done: %f\n", cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (Maintask, void*, 2) { // NB: using 2 level task groups spawned
    Maintask_base base;
    cco_group workers;
};

int maintask(struct Maintask* o) {
    cco_async (o) {
        
        { // sleeper - grp 0
            cco_spawn(c_new(struct Sleeper, {{sleeper}}), cco_grp(0));
            cco_suspend; // start sleeper

            { // workers - grp 1
                for (c_range32(idx, 8)) { // NB: local id, do not yield or await inside loop.
                    struct Worker* work = c_new(struct Worker, {{worker}, idx + 1});
                    cco_spawn(work, cco_grp(1));
                }
                cco_suspend; // starts workers.

                cco_await_n(1, cco_grp(1)); // await for 1 launched worker to finish
                puts("1 worker done.");

                cco_await_n(3, cco_grp(1)); // await for 3 more workers to finish
                puts("3 more workers done");

                //puts("CANCEL"); cco_throw(cco_CANCEL); // testing...

                cco_await_all(cco_grp(1)); // await for remaining workers to finish
                puts("All workers done.");
            }

            cco_await_all(cco_grp(0)); // await sleeper - should finish last also in case of cancel.
        }
        cco_finalize:
        cco_await_cancel_groups(o); // needed in case of cancellation: cancel and await all
                                    // spawned subtasks in both groups, first 1, then 0.
        puts("Maintask done");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    cco_run_task(c_new(struct Maintask, {{maintask}}));
}
