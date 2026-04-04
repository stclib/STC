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
        cco_await_timer(&o->tm, 1.0);

        cco_finalize:
        printf("Sleeper done: %f\n", cco_timer_elapsed(&o->tm));
    }

    c_free_n(o, 1);
    return 0;
}


cco_task_struct (Maintask) {
    Maintask_base base;
    cco_group workers;
};

int maintask(struct Maintask* o) {
    cco_async (o) {
        // use default waitgroup for sleeper
        cco_spawn(c_new(struct Sleeper, {{sleeper}}), cco_wg());
        cco_suspend; // start sleeper

        { // spawn workers
            for (c_range32(id, 8)) { // NB: local id, do not yield or await inside loop.
                struct Worker* work = c_new(struct Worker, {{worker}, id});
                cco_spawn(work, &o->workers);
            }
            cco_suspend; // starts workers.

            puts("Await 1");
            cco_await_n(1, &o->workers); // await for 1 launched worker to finish
            puts("1 worker done.");

            //cco_throw(cco_CANCEL); // testing...

            cco_await_n(3, &o->workers); // await for 3 more workers to finish
            puts("3 more workers done");

            cco_await_all(&o->workers); // await for remaining workers to finish
            puts("All workers done.");
        }

        cco_await_all(cco_wg()); // await sleeper.

        cco_finalize:
        if (cco_err().code) { // manually cancel spawned tasks on error
            cco_await_cancel_all(&o->workers);
            cco_await_cancel_all(cco_wg());
        }
        puts("Maintask done");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    cco_run_task(c_new(struct Maintask, {{maintask}}));
}
