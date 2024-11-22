// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include "stc/random.h"
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
    cco_state cco; // required
};

struct Dining {
    // Define semaphores for the forks
    cco_semaphore forks[num_forks];
    struct Philosopher ph[num_philosophers];
    cco_state cco; // required
};


// Philosopher coroutine
int philosopher(struct Philosopher* ph)
{
    double duration;
    cco_routine (ph) {
        while (1) {
            duration = 1.0 + crand64_real()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", ph->id, duration*10);
            cco_await_timer_sec(&ph->tm, duration);

            printf("Philosopher %d is hungry...\n", ph->id);
            cco_await_semaphore(ph->left_fork);
            cco_await_semaphore(ph->right_fork);

            duration = 0.5 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", ph->id, duration*10);
            cco_await_timer_sec(&ph->tm, duration);

            cco_release_semaphore(ph->left_fork);
            cco_release_semaphore(ph->right_fork);
        }

        cco_cleanup:
        printf("Philosopher %d finished\n", ph->id);
    }
    return 0;
}


// Dining coroutine
int dining(struct Dining* di)
{
    cco_routine (di) {
        for (int i = 0; i < num_forks; ++i)
            cco_set_semaphore(&di->forks[i], 1); // all forks available
        for (int i = 0; i < num_philosophers; ++i) {
            cco_reset_state(&di->ph[i]);
            di->ph[i].id = i + 1;
            di->ph[i].left_fork = &di->forks[i];
            di->ph[i].right_fork = &di->forks[(i + 1) % num_forks];
        }

        while (1) {
            // per-"frame" logic resume each philosopher
            for (int i = 0; i < num_philosophers; ++i) {
                philosopher(&di->ph[i]);
            }
            cco_yield; // suspend, return control back to main
        }

        cco_cleanup:
        for (int i = 0; i < num_philosophers; ++i) {
            cco_stop(&di->ph[i]);
            philosopher(&di->ph[i]);
        }
        puts("Dining finished");
    }
    return 0;
}

int main(void)
{
    struct Dining dine;
    cco_reset_state(&dine);
    int n=0;
    cco_timer tm = cco_make_timer_sec(5.0);
    crand64_seed((uint64_t)time(NULL));

    cco_run_coroutine(dining(&dine))
    {
        if (cco_timer_expired(&tm))
            cco_stop(&dine);
        cco_sleep_sec(0.001);
        ++n;
    }
    printf("n=%d\n", n);
}
