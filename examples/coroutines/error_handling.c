#include <stdio.h>
#include <time.h>
#include "stc/coroutine.h"

cco_task_struct (Subtask) {
    Subtask_base base;
    int id;
    cco_timer tmr;
};

int subtask(struct Subtask* o) {
    cco_async (o) {
        printf("Start task %d\n", o->id);

        cco_await_timer(&o->tmr, 0.1);
        if (o->id == 4) {
            cco_throw(cco_CANCEL, o->id); // Demo: throwing an error in one of the spawned tasks.
        }
        printf("WORKS task %d\n", o->id);

        cco_finalize:
        switch (cco_error().code) {
            case cco_CANCEL:
                printf("Cancelling %d because %d failed\n", o->id, (int)cco_error().info);
                //cco_recover;
        }
        printf("Clean task %d\n", o->id);
    }
    free(o);
    return 0;
}


cco_task_struct (Start) {
    Start_base base;
};

int start(struct Start* o) {
    cco_async (o) {
        for (c_range32(i, 6)) {
            cco_spawn( c_new(struct Subtask, {{subtask}, i}), cco_wg() );
        }
        puts("START");

        cco_finalize:
        cco_await_all(cco_wg());
        switch (cco_error().code) {
            case cco_SHUTDOWN:
                printf("Maintask was shut down by child %d in %s:%d\n", 
                       (int)cco_error().info, cco_error().file, cco_error().line);
        }
        puts("DONE");
    }
    free(o);
    return 0;
}


int main(void)
{
    cco_run_task( c_new(struct Start, {{start}}) );
}
