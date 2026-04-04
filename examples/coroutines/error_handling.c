#include <stdio.h>
#include "stc/coroutine.h"

cco_task_struct (Subtask, cco_timer*) {
    Subtask_base base;
    int id;
};

int subtask(struct Subtask* o) {
    cco_async (o) {
        printf("Start task %d\n", o->id);
        cco_await(cco_timer_elapsed(cco_data(o)) > 0.5);

        if (o->id == 2) {
            // Demo: throwing an error in one of the spawned tasks.
            cco_throw(cco_CANCEL, o->id); 
        }
        printf("Work task %d\n", o->id);

        cco_finalize:
        if (cco_catch(cco_CANCEL)) {
            printf("Cancelling %d due to %d\n", o->id, (int)cco_err().info.num);
            //cco_recover; // Cancel the cancellation...
        }
        printf("Clean task %d\n", o->id);
    }
    free(o);
    return 0;
}


cco_task_struct (Start) {
    Start_base base;
    cco_timer tmr;
};

int start(struct Start* o) {
    cco_async (o) {
        for (c_range32(i, 6)) {
            cco_spawn( c_new(struct Subtask, {{subtask}, i}), cco_wg(), &o->tmr );
        }
        puts("START");
        cco_await_timer(&o->tmr, 0.2);

        //puts("THROW"); cco_throw(cco_CANCEL, -1);
        cco_await_all(cco_wg());

        cco_finalize:
        if (cco_catch_any()) {
            if (cco_catch(cco_SHUTDOWN))
                printf("Maintask and subtasks shutdown caused by %d in %s:%d\n",
                       (int)cco_err().info.num, cco_err().file, cco_err().line);
            else if (cco_catch(cco_CANCEL))
                printf("Maintask was canceled in %s:%d\n", cco_err().file, cco_err().line);
            cco_await_cancel_all(cco_wg());
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
