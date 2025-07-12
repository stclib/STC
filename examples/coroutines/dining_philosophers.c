// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include <stc/random.h>
#include <stc/coroutine.h>

enum {num_philosophers = 5};
enum PhMode {ph_thinking, ph_hungry, ph_eating};

// Philosopher coroutine
struct Philosopher {
    cco_base base; // required
    int id;
    cco_timer tm;
    enum PhMode mode;
    int hunger;
    struct Philosopher* left;
    struct Philosopher* right;
};

int Philosopher(struct Philosopher* o) {
    double duration;
    cco_async (o) {
        while (1) {
            duration = 1.0 + crand64_real()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", o->id, duration*10);
            o->hunger = 0;
            o->mode = ph_thinking;
            cco_await_timer(&o->tm, duration);

            printf("Philosopher %d is hungry...\n", o->id);
            o->mode = ph_hungry;
            cco_await(o->hunger >= o->left->hunger &&
                      o->hunger >= o->right->hunger);

            duration = 0.5 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", o->id, duration*10);
            o->hunger = 3;
            o->mode = ph_eating;
            cco_await_timer(&o->tm, duration);

            // increase the neighbours hunger only if they are already hungry.
            if (o->left->mode == ph_hungry) ++o->left->hunger;
            if (o->right->mode == ph_hungry) ++o->right->hunger;
        }

        cco_drop:
        printf("Philosopher %d done\n", o->id);
    }
    return 0;
}

// Dining coroutine
struct Dining {
    cco_base base; // required
    struct Philosopher philos[num_philosophers];
};

int Dining(struct Dining* o) {
    cco_async (o) {
        for (int i = 0; i < num_philosophers; ++i) {
            o->philos[i] = (struct Philosopher){
                .id = i + 1,
                .left = &o->philos[(i - 1 + num_philosophers) % num_philosophers],
                .right = &o->philos[(i + 1) % num_philosophers],
            };
        }

        while (1) {
            // NB! local i OK here as it does not cross yield/await.
            for (int i = 0; i < num_philosophers; ++i) {
                Philosopher(&o->philos[i]); // resume until next yield
            }
            cco_suspend; // return control back to caller who
                         // can do other tasks before resuming dining.
        }

        cco_drop:
        for (int i = 0; i < num_philosophers; ++i) {
            cco_stop(&o->philos[i]);
            Philosopher(&o->philos[i]); // execute philos cleanup.
        }
        puts("Dining done");
    }
    return 0;
}

int main(void)
{
    struct Dining dining = {0};
    cco_timer tm = cco_make_timer(5.0);
    crand64_seed((uint64_t)time(NULL));

    cco_run_coroutine(Dining(&dining)) {
        if (cco_timer_expired(&tm))
            cco_stop(&dining);
        cco_sleep(0.001); // do other things
    }
}
