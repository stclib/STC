// https://mariusbancila.ro/blog/2020/06/22/a-cpp20-coroutine-example/

#include <time.h>
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/coroutine.h"

cco_task_struct (next_value) {
    next_value_state cco;
    int val;
    cco_timer tm;
};

int next_value(struct next_value* co, cco_runtime* rt)
{
    (void)rt;
    cco_scope (co) {
        while (true) {
            cco_await_timer(&co->tm, 1 + rand() % 2);
            co->val = rand();
            cco_yield;
        }
    }
    return 0;
}

void print_time(void)
{
    time_t now = time(NULL);
    char mbstr[64];
    strftime(mbstr, sizeof(mbstr), "[%H:%M:%S]", localtime(&now));
    printf("%s ", mbstr);
}

// PRODUCER

cco_task_struct (produce_items) {
    produce_items_state cco;
    struct next_value next;
    cstr text;
};

int produce_items(struct produce_items* p, cco_runtime* rt)
{
    cco_scope (p) {
        p->text = cstr_init();
        p->next.cco.func = next_value;
        while (true)
        {
            // await for next CCO_YIELD (or CCO_DONE) in next_value
            cco_await_task(&p->next, rt, CCO_YIELD);
            cstr_printf(&p->text, "item %d", p->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&p->text));
            cco_yield;
        }

        cco_cleanup:
        cstr_drop(&p->text);
        puts("done produce");
    }
    return 0;
}

// CONSUMER

cco_task_struct (consume_items) {
    consume_items_state cco;
    struct produce_items produce;
    int n, i;
};

int consume_items(struct consume_items* c, cco_runtime* rt)
{
   cco_scope (c) {
        c->produce.cco.func = produce_items;

        for (c->i = 1; c->i <= c->n; ++c->i)
        {
            printf("consume #%d\n", c->i);
            cco_await_task(&c->produce, rt, CCO_YIELD);
            print_time();
            printf("consumed %s\n", cstr_str(&c->produce.text));
        }

        cco_cleanup:
        cco_stop(&c->produce);
        cco_resume_task(&c->produce, rt);
        puts("done consume");
    }
    return 0;
}

int main(void)
{
    struct consume_items consume = {
        .cco = {consume_items},
        .n = 3,
    };
    cco_run_task(&consume);
}
