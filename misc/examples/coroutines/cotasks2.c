#include <time.h>
#include <stdio.h>
#include "stc/coroutine.h"
#define i_type Inventory, int
#include "stc/queue.h"

// Example shows symmetric coroutines producer/consumer style.

// PRODUCER
cco_task_struct (produce_items) {
    produce_items_state cco; // must be first
    struct consume_items* consumer;
    Inventory inv;
    int limit, batch, serial, total;
};

// CONSUMER
cco_task_struct (consume_items) {
    consume_items_state cco; // must be first
    struct produce_items* producer;
};

int produce_items(struct produce_items* co, cco_runtime* rt) {
    cco_routine (co) {
        while (1) {
            if (co->serial > co->total) {
                if (Inventory_is_empty(&co->inv))
                    cco_return; // cleanup and finish
            }
            else if (Inventory_size(&co->inv) < co->limit) {
                c_forrange (co->batch)
                    Inventory_push(&co->inv, ++co->serial);

                printf("produced %d items, Inventory has now %d items:\n",
                       co->batch, (int)Inventory_size(&co->inv));

                c_foreach (i, Inventory, co->inv)
                    printf(" %2d", *i.ref);
                puts("");
            }

            cco_yield_task(co->consumer, rt); // yield to consumer
        }

        cco_cleanup:
        cco_cancel_task(co->consumer, rt);
        Inventory_drop(&co->inv);
        puts("cleanup producer");
    }
    return 0;
}

int consume_items(struct consume_items* co, cco_runtime* rt) {
    cco_routine (co) {
        while (1) {
            int n = rand() % 10;
            int sz = (int)Inventory_size(&co->producer->inv);
            if (n > sz) n = sz;

            c_forrange (n)
                Inventory_pop(&co->producer->inv);
            printf("consumed %d items\n", n);

            cco_yield_task(co->producer, rt); // yield to producer
        }

        cco_cleanup:
        puts("cleanup consumer");
    }
    return 0;
}

int main(void)
{
    srand(time(0));
    struct produce_items producer = {
        .cco = {produce_items},
        .inv = {0},
        .limit = 12,
        .batch = 8,
        .total = 50,
    };
    struct consume_items consumer = {
        .cco = {consume_items},
        .producer = &producer,
    };
    producer.consumer = &consumer;

    cco_run_task(&producer);
}
