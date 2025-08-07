// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include <stc/random.h>
#include <stc/coroutine.h>

enum {num_philosophers = 5};
enum PhMode {ph_THINKING, ph_HUNGRY, ph_EATING};
#define TIMEFAC 0.2

// Philosopher coroutine: use task coroutine
cco_task_struct (Philosopher, double) {
    Philosopher_base base; // required
    int id;
    cco_timer tm;
    enum PhMode mode;
    bool* fork1_taken;
    bool* fork2_taken;
};

int Philosopher(struct Philosopher* o) {
    cco_async (o) {
        while (1) {
            double duration = (1.0 + crand64_real())*5.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", o->id, duration*5);
            o->mode = ph_THINKING;
            cco_await_timer(&o->tm, duration*TIMEFAC);

            printf("Philosopher %d is hungry...\n", o->id);
            o->mode = ph_HUNGRY;
            cco_await(! *o->fork1_taken);
            *o->fork1_taken = true;
            cco_await(! *o->fork2_taken);
            *o->fork2_taken = true;

            duration = 1.0 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", o->id, duration*5);
            o->mode = ph_EATING;
            cco_await_timer(&o->tm, duration*TIMEFAC);
            *o->fork1_taken = false;
            *o->fork2_taken = false;
        }

        cco_drop:
        printf("Philosopher %d done\n", o->id);
    }
    return 0;
}


// Dining coroutine
cco_task_struct (Dining, double) {
    Dining_base base; // required
    float duration;
    struct Philosopher philos[num_philosophers];
    bool forks[num_philosophers];
    int i;
    cco_timer tm;
    cco_group philos_wg;
};

int Dining(struct Dining* o) {
    cco_async (o) {
        for (int i = 0; i < num_philosophers; ++i) {
            o->philos[i] = (struct Philosopher){
                .base = {Philosopher},
                .id = i + 1,
                .fork1_taken = &o->forks[(i + (1 & i)) % num_philosophers],
                .fork2_taken = &o->forks[(i + (1 & (i + 1))) % num_philosophers],
            };

            // Launch each philosopher in a new concurrent fiber
            cco_launch(&o->philos[i], &o->philos_wg);
        }

        // Limit the simulation time.
        cco_await_timer(&o->tm, o->duration);

        cco_drop:
        // Cancel all philosophers and await for them to finish.
        puts("Cancelling...");
        cco_await_cancel(&o->philos_wg);
        puts("Dining done");
    }
    return 0;
}


int main(void)
{
    struct Dining dining = {.base={Dining}, .duration=5.0f};

    crand64_seed((uint64_t)time(NULL));
    cco_run_task(&dining);
}
