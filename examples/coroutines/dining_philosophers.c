// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include <stc/random.h>
#include <stc/coroutine.h>

enum {num_philosophers = 5};
enum PhMode {ph_THINKING, ph_HUNGRY, ph_EATING};

// Philosopher coroutine: use task coroutine
cco_task_struct (Philosopher, cco_timer*) { // optional: make cco_env(o) return cco_timer*
    Philosopher_base base; // required
    int id;
    cco_timer timer;
    enum PhMode mode;
    int hunger;
    struct Philosopher* left;
    struct Philosopher* right;
};

int Philosopher(struct Philosopher* o) {
    cco_async (o) {
        while (1) {
            double duration = 1.0 + crand64_real()*2.0;
            printf("%4.1f: Philosopher %d is thinking for %.1f minutes...\n", 10*cco_timer_elapsed(cco_env(o)), o->id, duration*10);
            o->hunger = 0;
            o->mode = ph_THINKING;
            cco_await_timer(&o->timer, duration);

            printf("%4.1f: Philosopher %d is hungry...\n", 10*cco_timer_elapsed(cco_env(o)), o->id);
            o->mode = ph_HUNGRY;
            cco_await(o->hunger >= o->left->hunger &&
                      o->hunger >= o->right->hunger);
            o->left->hunger += (o->left->mode == ph_HUNGRY); // don't starve the neighbours
            o->right->hunger += (o->right->mode == ph_HUNGRY);
            o->hunger = INT32_MAX;
            o->mode = ph_EATING;

            duration = 0.5 + crand64_real();
            printf("%4.1f: Philosopher %d is eating for %.1f minutes...\n", 10*cco_timer_elapsed(cco_env(o)), o->id, duration*10);
            cco_await_timer(&o->timer, duration);

            if (o->id == 3) { // For demonstration purpose:
                // CANCELING after philosopher 3 has eaten.
                cco_throw(cco_CANCEL, o->id);
            }
        }
        cco_finalize:
        printf("Philosopher %d done\n", o->id);
    }

    return 0;
}

// Dining coroutine
cco_task_struct (Dining) {
    Dining_base base; // required
    float duration;
    struct Philosopher philos[num_philosophers];
    int i;
    cco_timer timer;
    cco_group group;
};

int Dining(struct Dining* o) {
    cco_async (o) {
        for (int i = 0; i < num_philosophers; ++i) {
            o->philos[i] = (struct Philosopher){
                .base = {Philosopher},
                .id = i + 1,
                .left = &o->philos[(i - 1 + num_philosophers) % num_philosophers],
                .right = &o->philos[(i + 1) % num_philosophers],
            };
            cco_spawn(&o->philos[i], cco_wg(), &o->timer); // pass in cco_env().
        }

        cco_on_child_error(cco_SHUTDOWN, cco_wg());
        cco_await_timer(&o->timer, o->duration);
        puts("Time's up! Stopping dining...");  

        cco_finalize:
        switch (cco_error().code) {
            case cco_SHUTDOWN:
                printf("Philosopher %d CANCELED the dining.\n", (int)cco_error().info);
                break;
        }
        cco_await_cancel_all(cco_wg());
        printf("Dining time of %.1f minutes is over.\n", cco_timer_elapsed(&o->timer)*10);
    }
    return 0;
}

int main(void)
{
    struct Dining dining = {.base={Dining}, .duration=6.0f};

    crand64_seed((uint64_t)time(NULL));
    cco_run_task(&dining);
}
