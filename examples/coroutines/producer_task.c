#include <time.h>
#include <stdio.h>
#include <stc/coroutine.h>
#define T Inventory, int
#include <stc/queue.h>

// Example shows symmetric coroutines producer/consumer style.

cco_task_struct (produce) {
    produce_base base; // must be first (compile-time checked)
    struct consume* consumer;
    Inventory inv;
    int limit, batch, serial, total;
};

cco_task_struct (consume) {
    consume_base base; // must be first
    struct produce* producer;
};


int produce(struct produce* o) {
    cco_async (o) {
        while (1) {
            if (o->serial > o->total) {
                if (Inventory_is_empty(&o->inv))
                    cco_return; // cleanup and finish
            }
            else if (Inventory_size(&o->inv) < o->limit) {
                for (c_range(o->batch))
                    Inventory_push(&o->inv, ++o->serial);

                printf("produced %d items, Inventory has now %d items:\n",
                       o->batch, (int)Inventory_size(&o->inv));

                for (c_each(i, Inventory, o->inv))
                    printf(" %2d", *i.ref);
                puts("");
            }

            cco_yield_to(o->consumer); // symmetric transfer
        }

        cco_finalize:
        puts("stop consumer");
        cco_await_cancel_task(o->consumer);
        Inventory_drop(&o->inv);
        puts("producer dropped");
    }
    return 0;
}

int consume(struct consume* o) {
    cco_async (o) {
        int n, sz;
        while (1) {
            n = rand() % 10;
            sz = (int)Inventory_size(&o->producer->inv);
            if (n > sz) n = sz;

            for (c_range(n))
                Inventory_pop(&o->producer->inv);
            printf("consumed %d items\n", n);

            cco_yield_to(o->producer); // symmetric transfer
        }

        cco_finalize:
        puts("consumer drop step 1");
        cco_yield;
        puts("consumer drop step 2");
        cco_yield;
        puts("consumer dropped");
    }
    return 0;
}

int main(void)
{
    srand((unsigned)time(0));
    struct produce producer = {
        .base = {produce},
        .inv = {0},
        .limit = 12,
        .batch = 8,
        .total = 50,
    };
    struct consume consumer = {
        .base = {consume},
        .producer = &producer,
    };
    producer.consumer = &consumer;

    cco_run_task(&producer);
}
