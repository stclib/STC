// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <stc/crand.h>
#include <stc/algo/coroutine.h>
 
// Define the number of philosophers and forks
enum {
    num_philosophers = 5,
    num_forks = num_philosophers,
};

struct Philosopher {
    int id;
    cco_timer tm;
    cco_sem* left_fork;
    cco_sem* right_fork;
    int cco_state; // required
};

struct Dining {
    // Define semaphores for the forks
    cco_sem forks[num_forks];
    struct Philosopher ph[num_philosophers];
    int ph_idx;
    int cco_state; // required
};

 
// Philosopher coroutine
void philosopher(struct Philosopher* p)
{
    cco_routine(p) {
        while (1) {
            int duration = (int)(1000 + crand() % 2000); // 1-3 seconds
            printf("Philosopher %d is thinking for %d minutes...\n", p->id, duration/100);
            cco_timer_await(&p->tm, duration);

            printf("Philosopher %d is hungry...\n", p->id);
            cco_sem_await(p->left_fork);
            cco_sem_await(p->right_fork);
            
            duration = (int)(500 + crand() % 1000);
            printf("Philosopher %d is eating for %d minutes...\n", p->id, duration/100);
            cco_timer_await(&p->tm, duration);
            
            cco_sem_release(p->left_fork);
            cco_sem_release(p->right_fork);
        }
        
        cco_final:
        printf("Philosopher %d finished\n", p->id);
    }
}


// Dining coroutine
void dining(struct Dining* d)
{
    cco_routine(d) {
        for (int i = 0; i < num_forks; ++i)
            cco_sem_set(&d->forks[i], 1); // all forks available
        for (int i = 0; i < num_philosophers; ++i) {
            cco_reset(&d->ph[i]);
            d->ph[i].id = i + 1;
            d->ph[i].left_fork = &d->forks[i];
            d->ph[i].right_fork = &d->forks[(i + 1) % num_forks];
        }

        while (1) {
            for (d->ph_idx = 0; d->ph_idx < num_philosophers; ++d->ph_idx) {
                philosopher(&d->ph[d->ph_idx]);
                cco_yield();
            }
        }

        cco_final:
        for (int i = 0; i < num_philosophers; ++i) {
            cco_stop(&d->ph[i]);
            philosopher(&d->ph[i]);
        }
        puts("Dining finished");
    }
}


int main()
{
    struct Dining dine;
    cco_reset(&dine);
    cco_timer tm = cco_timer_from(10000);
    csrand((uint64_t)time(NULL));

    while (!cco_done(&dine)) {
        if (cco_timer_expired(&tm))
            cco_stop(&dine);
        dining(&dine);
        cco_sleep(1);
    }
}
