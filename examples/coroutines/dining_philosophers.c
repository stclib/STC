// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include <stc/random.h>
#include <stc/coroutine.h>

enum {num_philosophers = 5};
enum PhMode {ph_THINKING, ph_HUNGRY, ph_EATING};

// Philosopher coroutine: use task coroutine
cco_task_struct (Philosopher) {
    Philosopher_base base; // required
    int id;
    cco_timer tm;
    enum PhMode mode;
    int hunger;
    struct Philosopher* left;
    struct Philosopher* right;
};

int Philosopher(struct Philosopher* o) {
    cco_async (o) {
        while (1) {
            double duration = 1.0 + crand64_real()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", o->id, duration*10);
            o->hunger = 0;
            o->mode = ph_THINKING;
            cco_await_timer(&o->tm, duration);

            printf("Philosopher %d is hungry...\n", o->id);
            o->mode = ph_HUNGRY;
            cco_await(o->hunger >= o->left->hunger &&
                      o->hunger >= o->right->hunger);
            o->left->hunger += (o->left->mode == ph_HUNGRY); // don't starve the neighbours
            o->right->hunger += (o->right->mode == ph_HUNGRY);
            o->hunger = INT32_MAX;
            o->mode = ph_EATING;

            duration = 0.5 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", o->id, duration*10);
            cco_await_timer(&o->tm, duration);
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
    cco_timer tm;
    cco_group wg;
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
            cco_spawn(&o->philos[i], &o->wg);
        }
        cco_await_timer(&o->tm, o->duration);

        cco_finalize:
        cco_await_cancel_group(&o->wg);
        puts("Dining done");
    }
    return 0;
}

int main(void)
{
    struct Dining dining = {.base={Dining}, .duration=5.0f, .wg = {0}};

    crand64_seed((uint64_t)time(NULL));
    cco_run_task(&dining);
}
