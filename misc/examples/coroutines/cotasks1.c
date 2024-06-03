// https://mariusbancila.ro/blog/2020/06/22/a-cpp20-coroutine-example/

#include <time.h>
#include <stdio.h>
#define i_static
#include "stc/cstr.h"
#include "stc/coroutine.h"

struct next_value {
    int val;
    int cco_state;
    cco_timer tm;
};

int next_value(struct next_value* co)
{
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

struct produce_items {
    struct next_value next;
    cstr text;
    int cco_state;
};

int produce_items(struct produce_items* p)
{
    cco_scope (p) {
        p->text = cstr_init();
        while (true)
        {
            cco_await(next_value(&p->next) != CCO_AWAIT);
            cstr_printf(&p->text, "item %d", p->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&p->text));
            cco_yield;
        }
        cco_final:
            cstr_drop(&p->text);
            puts("done produce");
    }
    return 0;
}

// CONSUMER

struct consume_items {
    int n, i;
    int cco_state;
};

int consume_items(struct consume_items* c, struct produce_items* p)
{
   cco_scope (c) {
        for (c->i = 1; c->i <= c->n; ++c->i)
        {
            printf("consume #%d\n", c->i);
            cco_await(produce_items(p) != CCO_AWAIT);
            print_time();
            printf("consumed %s\n", cstr_str(&p->text));
        }
        cco_final:
        puts("done consume");
    }
    return 0;
}

int main(void)
{
    struct produce_items produce = {0};
    struct consume_items consume = {.n=3};
    int count = 0;

    cco_run_coroutine(consume_items(&consume, &produce))
    {
        ++count;
        //cco_sleep(0.001);
        //if (consume.i == 3) cco_stop(&consume);
    }
    cco_stop(&produce);
    produce_items(&produce);
    printf("count: %d\n", count);
}
