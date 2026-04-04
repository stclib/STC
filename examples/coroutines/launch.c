#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (TaskA, struct Output*) {
    TaskA_base base;
    int a;
};
cco_task_struct (TaskB, struct Output*) {
    TaskB_base base;
    float x, y;
};
cco_task_struct (TaskC, struct Output*) {
    TaskC_base base;
    double d;
};
cco_task_struct (SubTask) {
    SubTask_base base;
    int id, step;
};


int subTask(struct SubTask* o) {
    cco_async (o) {
        for (o->step = 'a'; o->step <= 'c'; ++o->step) {
            printf("SubTask%d: step %c\n", o->id, o->step);
            
            if (o->id == 2 && o->step == 'b') {
               //cco_throw(cco_CANCEL, o->id); // Demo: throwing an error in one of the spawned tasks.
            }
            cco_yield;
        }

        cco_finalize:
        if (cco_catch(cco_CANCEL)) {
            printf("SubTask%d: cancelled at %s:%d\n", o->id, cco_err().file, cco_err().line);
            //cco_recover;     // clear cancellation and resume after the cco_throw() above.
            //cco_clear_err(); // alt: just clear errcode to avoid all other subtasks to be cancelled.
        }
        printf("subTask%d: done\n", o->id);
    }

    c_free_n(o, 1);
    return 0;
}


struct Output {
    double value;
    int error;
};


int taskC(struct TaskC* o) {
    cco_async (o) {
        printf("TaskC: start %g\n", o->d);
        //cco_await_task(c_new(struct TaskB, {{taskB}, 1.2f, 3.4f}));

        puts("TaskC: work");
        cco_data(o)->value += o->d; // accumulate return value
        cco_yield;

        cco_spawn(c_new(struct SubTask, {{subTask}, 1}), cco_wg());
        cco_spawn(c_new(struct SubTask, {{subTask}, 2}), cco_wg());
        cco_spawn(c_new(struct SubTask, {{subTask}, 3}), cco_wg());
        puts("TaskC: spawned 3 tasks");
        cco_await_all(cco_wg());
        puts("TaskC: all spawned tasks done");

        cco_finalize:
        if (cco_catch(cco_SHUTDOWN)) { // just for info
            printf("TaskC: shutdown by child %d in %s:%d\n", (int)cco_err().info.num, cco_err().file, cco_err().line);
        }
        puts("TaskC: done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskB(struct TaskB* o) {
    cco_async (o) {
        printf("TaskB: start {%g, %g}\n", o->x, o->y);
        cco_await_task(c_new(struct TaskC, {{taskC}, 3.1415}));

        cco_throw(99); // Demo: throwing an error in an awaiting task.

        puts("TaskB: work");
        
        cco_yield;
        puts("TaskB: more work");

        // initial return value
        cco_data(o)->value += o->x + o->y;
        cco_yield;
        
        cco_finalize:
        if (cco_catch(99)) {
            printf("TaskB: handling error '99' thrown in %s:%d\n", cco_err().file, cco_err().line);
            //cco_clear_err();
            //cco_await_cancel_all(cco_wg());
            cco_recover;
        }        
        puts("TaskB: done");
    }

    c_free_n(o, 1);
    return 0;
}


int taskA(struct TaskA* o) {
    cco_async (o) {
        printf("TaskA: start %d\n", o->a);
        //cco_await_task(c_new(struct TaskC, {{taskC}, 3.1415}));
        cco_await_task(c_new(struct TaskB, {{taskB}, 1.2f, 3.4f}));

        puts("TaskA: work");
        cco_data(o)->value += o->a; // final return value;
        cco_yield;

        cco_finalize:
        puts("TaskA: done");
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct Output output = {0};
    cco_run_task(c_new(struct TaskA, {{taskA}, 42}), &output);

    printf("\nresult: %g, error: %d\n", output.value, output.error);
}
