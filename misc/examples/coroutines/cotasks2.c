// https://mariusbancila.ro/blog/2020/06/22/a-cpp20-coroutine-example/

#include <time.h>
#include <stdio.h>
#define i_static
#include <stc/cstr.h>
#include <stc/coroutine.h>

cco_task_struct (next_value,
    int val;
    cco_timer tm;
);

int next_value(struct next_value* co, cco_runtime* rt)
{
    cco_routine (co) {
        while (true) {
            cco_timer_await(&co->tm, 1 + rand() % 2);
            co->val = rand();
            cco_yield();
        }
    }
    return 0;
}

void print_time()
{
    time_t now = time(NULL);
    char mbstr[64];
    strftime(mbstr, sizeof(mbstr), "[%H:%M:%S]", localtime(&now)); 
    printf("%s ", mbstr);
}

// PRODUCER

cco_task_struct (produce_items,
    struct next_value next;
    cstr str;
);

int produce_items(struct produce_items* p, cco_runtime* rt)
{
    cco_routine (p) {
        p->str = cstr_null;
        while (true)
        {
            // await for next CCO_YIELD in next_value()
            cco_task_await(&p->next, rt, CCO_YIELD);
            cstr_printf(&p->str, "item %d", p->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&p->str));
            cco_yield();
        }
        cco_cleanup:
            cstr_drop(&p->str);
            puts("done produce");
    }
    return 0;
}

// CONSUMER

cco_task_struct (consume_items,
    int n, i;
    struct produce_items produce;
);

int consume_items(struct consume_items* c, cco_runtime* rt)
{
   cco_routine (c) {
        for (c->i = 1; c->i <= c->n; ++c->i)
        {
            printf("consume #%d\n", c->i);
            cco_task_await(&c->produce, rt, CCO_YIELD);
            print_time();
            printf("consumed %s\n", cstr_str(&c->produce.str));
        }
        cco_cleanup:
            cco_stop(&c->produce);
            cco_resume(&c->produce, rt);
            puts("done consume");
    }
    return 0;
}

int main(void)
{
    struct consume_items consume = {
        .n=5,
        .cco_func=consume_items,
        .produce={.cco_func=produce_items, .next={.cco_func=next_value}},
    };
    int count = 0;

    cco_task_block_on(&consume)
    {
        ++count;
        //cco_sleep(0.001);
        //if (consume.i == 3)
        //    cco_stop(&consume);
    }
    printf("count: %d\n", count);
}