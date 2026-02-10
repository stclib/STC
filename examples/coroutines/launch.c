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


cco_task_struct (myTask) {
    myTask_base base;
    int id, n;
};

int myTask(struct myTask* o) {
    cco_async (o) {
        for (o->n = 1; o->n <= 3; ++o->n) {
            if (o->id == 101 && o->n == 3)
               cco_throw(cco_CANCEL); // Demo: throwing an error in one of the spawned tasks.

            printf("myTask %d: step %d\n", o->id, o->n);
            cco_yield;
        }

        cco_finalize:
        printf("myTask %d done:\n", o->id);
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
        //cco_throw(99); // Demo: throwing an error in an awaiting task.

        puts("taskC work");
        cco_yield;
        puts("taskC more work");

        // initial return value
        {struct Output* e = cco_env(o);
         e->value = o->x + o->y;}
        cco_yield;

        cco_finalize:
        switch (cco_error_code()) {
            case 99:
                printf("taskC error '99' thrown on line %d\n", cco_error()->line);
                //cco_recover; // reset error to 0 and proceed after the await taskC call.
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

        puts("Spawning 3 tasks.");
        cco_on_child_error(cco_SHUTDOWN, cco_wg()); // set error handler for the spawned tasks.
        cco_spawn(c_new(struct myTask, {{myTask}, 100}), cco_wg());
        cco_spawn(c_new(struct myTask, {{myTask}, 101}), cco_wg());
        cco_spawn(c_new(struct myTask, {{myTask}, 102}), cco_wg());
        cco_yield;
        puts("Spawned 3 tasks.");

        cco_finalize:
        if (cco_error()->code == 99) {
            printf("taskB recovered error '99' thrown on line %d\n", cco_error()->line);
            cco_recover; // reset error to 0 and proceed after the await taskB call.
        }
        cco_await_all(cco_wg());
        puts("Joined");
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
