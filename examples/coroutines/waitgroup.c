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

    c_free_obj(o);
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

    c_free_obj(o);
    return 0;
}


cco_task_struct (Maintask, void*, 2) { // NB: using 2 level spawned task-groups
    Maintask_base base;
};

int maintask(struct Maintask* o) {
    cco_async (o) {

        cco_group_scope { // sleeper group scope
            cco_spawn(c_new(struct Sleeper, {{sleeper}}), cco_scope());
            cco_suspend; // start sleeper

            cco_group_scope { // workers group scope
                for (c_range32(idx, 8)) { // NB: local id, do not yield or await inside loop.
                    struct Worker* work = c_new(struct Worker, {{worker}, idx + 1});
                    cco_spawn(work, cco_scope());
                }
                cco_suspend; // starts workers.

                cco_await_n(1, cco_scope()); // await for 1 launched worker to finish
                puts("1 worker done.");

                cco_await_n(3, cco_scope()); // await for 3 more workers to finish
                puts("3 more workers done");

                //puts("CANCEL"); cco_throw(cco_CANCEL); // testing...

                cco_await_all(cco_scope()); // await for remaining workers to finish
                puts("All workers done.");
            }

            cco_await_all(cco_scope()); // await sleeper - should finish last also in case of cancel.
        }

        cco_finalize:
        cco_await_cancel_groups(o); // needed in case of cancellation: cancel and await all
                                    // spawned subtasks in both groups, first inner, then outer.
        puts("Maintask done");
    }

    c_free_obj(o);
    return 0;
}


int main(void)
{
    cco_run_task(c_new(struct Maintask, {{maintask}}));
}
