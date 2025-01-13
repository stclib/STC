#include <time.h>
#include <stdio.h>
#include "stc/coroutine.h"
#define i_type Inventory, int
#include "stc/queue.h"

// Example shows symmetric coroutines producer/consumer style.

cco_task_struct (produce) {
    produce_state cco; // must be first (compile-time checked)
    struct consume* consumer;
    Inventory inv;
    int limit, batch, serial, total;
};

cco_task_struct (consume) {
    consume_state cco; // must be first
    struct produce* producer;
};


int produce(struct produce* co, cco_runtime* rt) {
    cco_routine (co) {
        while (1) {
            if (co->serial > co->total) {
                if (Inventory_is_empty(&co->inv))
                    cco_return; // cleanup and finish
            }
            else if (Inventory_size(&co->inv) < co->limit) {
                for (c_range(co->batch))
                    Inventory_push(&co->inv, ++co->serial);

                printf("produced %d items, Inventory has now %d items:\n",
                       co->batch, (int)Inventory_size(&co->inv));

                for (c_each(i, Inventory, co->inv))
                    printf(" %2d", *i.ref);
                puts("");
            }

            cco_yield_to(co->consumer, rt); // symmetric transfer
        }

        cco_finally:
        cco_cancel_task(co->consumer, rt);
        Inventory_drop(&co->inv);
        puts("cleanup producer");
    }
    return 0;
}

int consume(struct consume* co, cco_runtime* rt) {
    cco_routine (co) {
        int n, sz;
        while (1) {
            n = rand() % 10;
            sz = (int)Inventory_size(&co->producer->inv);
            if (n > sz) n = sz;

            for (c_range(n))
                Inventory_pop(&co->producer->inv);
            printf("consumed %d items\n", n);

            cco_yield_to(co->producer, rt); // symmetric transfer
        }

        cco_finally:
        puts("cleanup consumer");
    }
    return 0;
}

int main(void)
{
    srand((unsigned)time(0));
    struct produce producer = {
        .cco = {produce},
        .inv = {0},
        .limit = 12,
        .batch = 8,
        .total = 50,
    };
    struct consume consumer = {
        .cco = {consume},
        .producer = &producer,
    };
    producer.consumer = &consumer;

    cco_run_task(&producer);
}
