// https://mariusbancila.ro/blog/2020/06/22/a-cpp20-coroutine-example/

#include <time.h>
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/coroutine.h"

struct next_value {
    int val;
    cco_timer tm;
    cco_state cco;
};

int next_value(struct next_value* co) {
    cco_routine (co) {
        while (true) {
            cco_await_timer(&co->tm, 1 + rand() % 2);
            co->val = rand();
            cco_yield;
        }
    }
    return 0;
}

void print_time(void) {
    time_t now = time(NULL);
    char mbstr[64];
    strftime(mbstr, sizeof(mbstr), "[%H:%M:%S]", localtime(&now));
    printf("%s ", mbstr);
}

// PRODUCER
struct produce_items {
    struct next_value next;
    cstr text;
    cco_state cco;
};

int produce_items(struct produce_items* pr) {
    cco_routine (pr) {
        pr->text = cstr_init();
        while (true) {
            cco_await_coroutine(next_value(&pr->next), CCO_YIELD);
            cstr_printf(&pr->text, "item %d", pr->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&pr->text));
            cco_yield;
        }

        cco_cleanup:
        cstr_drop(&pr->text);
        puts("done produce");
    }
    return 0;
}

// CONSUMER
struct consume_items {
    int n, i;
    cco_state cco;
};

int consume_items(struct consume_items* co, struct produce_items* pr) {
   cco_routine (co) {
        for (co->i = 1; co->i <= co->n; ++co->i) {
            printf("consume #%d\n", co->i);
            cco_await_coroutine(produce_items(pr), CCO_YIELD);
            print_time();
            printf("consumed %s\n", cstr_str(&pr->text));
        }

        cco_cleanup:
        puts("done consume");
    }
    return 0;
}

int main(void) {
    struct produce_items produce = {0};
    struct consume_items consume = {.n=5};
    int count = 0;

    cco_run_coroutine(consume_items(&consume, &produce)) {
        ++count;
        //cco_sleep(0.001);
        //if (consume.i == 3) cco_stop(&consume);
    }
    cco_cancel(&produce, produce_items);
    printf("count: %d\n", count);
}
