#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (Nested) {
    Nested_base base;
    const char* name;
    int level;
    cco_timer tm;
};

int nested(struct Nested* o) {
    cco_async (o) {
        printf("%s%d: start\n", o->name, o->level);

        if (o->level < 6) {
            cco_await_task(c_new(struct Nested, {{nested}, o->name, o->level + 1}));
        }
        printf("%s%d: running\n", o->name, o->level);
        cco_await_timer(&o->tm, 0.2);

        if (o->level == 3) {
            puts("CANCEL"); cco_throw(cco_CANCEL);
        }
        printf("%s%d: finishing\n", o->name, o->level);
        cco_await_timer(&o->tm, 0.1);

        cco_finalize:
        printf("%s%d: done\n", o->name, o->level);
    }

    c_free_n(o, 1);
    return 0;
}


int main(void) {
    cco_run_task(c_new(struct Nested, {{nested}, "A", 1}));
}
