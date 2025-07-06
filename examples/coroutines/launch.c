#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (taskA) {
    taskA_base base;
    int a;
};
cco_task_struct (taskB) {
    taskB_base base;
    double d;
};
cco_task_struct (taskC) {
    taskC_base base;
    float x, y;
};

cco_task_struct (myTask) {
    myTask_base base;
    int n, end;
};

typedef struct {
    double value;
    int error;
    int myCount;
} Output;

#define OUT cco_env(Output *)

int myTask(struct myTask* o) {
    cco_async (o) {
        for (; o->n < o->end; ++o->n) {
            printf("myTask: %d\n", o->n);
            cco_yield;
        }

        cco_drop:
        OUT->myCount -= 1;
        puts("myTask done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskC(struct taskC* o) {
    cco_async (o) {
        printf("taskC start: {%g, %g}\n", o->x, o->y);

        // assume there is an error...
        cco_task_throw(99);

        puts("taskC work");
        cco_yield;
        puts("taskC more work");

        // initial return value
        OUT->value = o->x * o->y;

        cco_drop:
        puts("taskC done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskB(struct taskB* o) {
    cco_async (o) {
        printf("taskB start: %g\n", o->d);
        cco_await_task(cco_new_task(taskC, 1.2f, 3.4f));

        puts("taskB work");
        OUT->value += o->d;

        OUT->myCount = 3;
        cco_spawn(cco_new_task(myTask, 1, 6));
        cco_spawn(cco_new_task(myTask, 101, 104));
        cco_spawn(cco_new_task(myTask, 1001, 1008));
        puts("Spawned 3 tasks.");

        cco_await(OUT->myCount == 0);
        puts("Joined");
        puts("taskB done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskA(struct taskA* o) {
    cco_async (o) {
        printf("taskA start: %d\n", o->a);
        cco_await_task(cco_new_task(taskB, 3.1415));

        puts("taskA work");
        OUT->value += o->a; // final return value;

        cco_drop:
        if (cco_err().code == 99) {
            printf("taskA recovered error '99' thrown on line %d\n", cco_err().line);
            OUT->error = cco_err().code; // set error in output
            cco_recover_task(); // reset error to 0 and jump to after the await call.
        }
        puts("taskA done");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    Output output = {0};
    int count = 0;

    puts("start");
    struct taskA* start = c_new(struct taskA, {{taskA}, 42});
    cco_run_task(start, &output) { ++count; }

    printf("\nresult: %g, error: %d\n", output.value, output.error);
    printf("resumes: %d, fb size: %d\n", count, (int)sizeof(cco_fiber));
}
