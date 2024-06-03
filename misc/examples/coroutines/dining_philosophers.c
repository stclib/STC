// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include "stc/crand.h"
#include "stc/coroutine.h"

// Define the number of philosophers and forks
enum {
    num_philosophers = 5,
    num_forks = num_philosophers,
};

struct Philosopher {
    int id;
    cco_timer tm;
    cco_semaphore* left_fork;
    cco_semaphore* right_fork;
    int cco_state; // required
};

struct Dining {
    // Define semaphores for the forks
    cco_semaphore forks[num_forks];
    struct Philosopher ph[num_philosophers];
    int cco_state; // required
};


// Philosopher coroutine
int philosopher(struct Philosopher* p)
{
    double duration;
    cco_scope(p) {
        while (1) {
            duration = 1.0 + crandf()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", p->id, duration*10);
            cco_await_timer(&p->tm, duration);

            printf("Philosopher %d is hungry...\n", p->id);
            cco_await_semaphore(p->left_fork);
            cco_await_semaphore(p->right_fork);

            duration = 0.5 + crandf();
            printf("Philosopher %d is eating for %.0f minutes...\n", p->id, duration*10);
            cco_await_timer(&p->tm, duration);

            cco_semaphore_release(p->left_fork);
            cco_semaphore_release(p->right_fork);
        }

        cco_final:
        printf("Philosopher %d finished\n", p->id);
    }
    return 0;
}


// Dining coroutine
int dining(struct Dining* d)
{
    cco_scope(d) {
        for (int i = 0; i < num_forks; ++i)
            cco_semaphore_set(&d->forks[i], 1); // all forks available
        for (int i = 0; i < num_philosophers; ++i) {
            cco_reset(&d->ph[i]);
            d->ph[i].id = i + 1;
            d->ph[i].left_fork = &d->forks[i];
            d->ph[i].right_fork = &d->forks[(i + 1) % num_forks];
        }

        while (1) {
            // per-"frame" logic resume each philosopher
            for (int i = 0; i < num_philosophers; ++i) {
                philosopher(&d->ph[i]);
            }
            cco_yield; // suspend, return control back to main
        }

        cco_final:
        for (int i = 0; i < num_philosophers; ++i) {
            cco_stop(&d->ph[i]);
            philosopher(&d->ph[i]);
        }
        puts("Dining finished");
    }
    return 0;
}

int main(void)
{
    struct Dining dine;
    cco_reset(&dine);
    int n=0;
    cco_timer tm = cco_timer_from(5.0); // seconds
    csrand((uint64_t)time(NULL));

    cco_run_coroutine(dining(&dine))
    {
        if (cco_timer_expired(&tm))
            cco_stop(&dine);
        cco_sleep(0.001);
        ++n;
    }
    printf("n=%d\n", n);
}
