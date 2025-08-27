#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (nested) {
    nested_base base;
    const char* name;
    int level;
    cco_timer tm;
};

int nested(struct nested* o) {
    cco_async (o) {
        printf("%s%d: start\n", o->name, o->level);

        if (o->level < 6) {
            struct nested* c = c_new(struct nested, {{nested}, .name=o->name, .level=o->level + 1});
            cco_await_task(c);
        }
        printf("%s%d: running\n", o->name, o->level);
        cco_await_timer(&o->tm, 0.2);

        if (o->level == 3) {
            puts("CANCEL");
            cco_throw(CCO_CANCEL);
        }
        cco_await_timer(&o->tm, 0.2);

        cco_finalize:
        printf("%s%d: done\n", o->name, o->level);
    }

    c_free_n(o, 1);
    return 0;
}


int main(void)
{
    struct nested* go = c_new(struct nested, {{nested}, .name="A", .level=1});
    cco_run_task(go);
}
