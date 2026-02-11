#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (taskA, struct Output*) {
    taskA_base base;
    int a;
};
cco_task_struct (taskB, struct Output*) {
    taskB_base base;
    double d;
};
cco_task_struct (taskC, struct Output*) {
    taskC_base base;
    float x, y;
};


cco_task_struct (subTask) {
    subTask_base base;
    int id, step;
};

int subTask(struct subTask* o) {
    cco_async (o) {
        for (o->step = 1; o->step <= 3; ++o->step) {
            if (o->id == 102 && o->step == 3)
               cco_throw(cco_CANCEL, o->id); // Demo: throwing an error in one of the spawned tasks.

            printf("subTask %d: step %d\n", o->id, o->step);
            cco_yield;
        }

        cco_finalize:
        printf("subTask %d done:\n", o->id);
    }

    c_free_n(o, 1);
    return 0;
}


struct Output {
    double value;
    int error;
};

int taskC(struct taskC* o) {
    cco_async (o) {
        printf("taskC start: {%g, %g}\n", o->x, o->y);

        // assume there is an error...
        cco_throw(99); // Demo: throwing an error in an awaiting task.

        puts("taskC work");
        cco_yield;
        puts("taskC more work");

        // initial return value
        {struct Output* e = cco_env(o);
         e->value = o->x + o->y;}
        cco_yield;
        
        cco_finalize:
        switch (cco_error().code) {
            case 99:
                printf("taskC error '99' thrown in %s:%d\n", cco_error().file, cco_error().line);
                //cco_recover; // Recover from error here and resume after the cco_throw().
        }
        puts("taskC done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskB(struct taskB* o) {
    cco_async (o) {
        printf("taskB start: %g\n", o->d);
        cco_await_task(c_new(struct taskC, {{taskC}, 1.2f, 3.4f}));

        puts("taskB work");
        {struct Output* e = cco_env(o);
         e->value += o->d;} // accumulate return value
        cco_yield;

        puts("Spawning 3 tasks");
        cco_on_child_error(cco_SHUTDOWN, cco_wg()); // set error handler for the spawned tasks.
        cco_spawn(c_new(struct subTask, {{subTask}, 101}), cco_wg());
        cco_spawn(c_new(struct subTask, {{subTask}, 102}), cco_wg());
        cco_spawn(c_new(struct subTask, {{subTask}, 103}), cco_wg());
        puts("Spawned 3 tasks.");
        cco_await_all(cco_wg());
        puts("All spawned tasks done.");

        cco_finalize:
        switch (cco_error().code) {
            case 99:
                printf("taskB recovered error '99' thrown in %s:%d\n", cco_error().file, cco_error().line);
                cco_recover; // Recover from error and resume at the cco_await_task(taskC) call.
            case cco_SHUTDOWN:
                printf("taskB was shut down by child %d in %s:%d\n", (int)cco_error().info, cco_error().file, cco_error().line);
                cco_recover; // Recover from error and resume at the cco_await_all() call.
        }
        puts("taskB done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskA(struct taskA* o) {
    cco_async (o) {
        printf("taskA start: %d\n", o->a);
        cco_await_task(c_new(struct taskB, {{taskB}, 3.1415}));

        puts("taskA work");
        {struct Output* e = cco_env(o);
         e->value = o->a;} // final return value;
        cco_yield;

        cco_finalize:
        puts("taskA done");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct Output output = {0};
    int count = 0;

    puts("start");

    struct taskA* start = c_new(struct taskA, {{taskA}, 42});
    cco_run_task(start, &output) { ++count; }

    printf("\nresult: %g, error: %d\n", output.value, output.error);
    printf("resumes: %d, fb size: %d\n", count, (int)sizeof(cco_fiber));
}
