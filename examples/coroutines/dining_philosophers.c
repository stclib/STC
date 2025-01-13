// https://en.wikipedia.org/wiki/Dining_philosophers_problem
#include <stdio.h>
#include <time.h>
#include "stc/random.h"
#include "stc/coroutine.h"

enum {num_philosophers = 5};
enum PhState {ph_thinking, ph_hungry, ph_eating};

// Philosopher coroutine
struct Philosopher {
    int id;
    cco_timer tm;
    enum PhState state;
    int hunger;
    struct Philosopher* left;
    struct Philosopher* right;
    cco_state cco; // required
};

int Philosopher(struct Philosopher* self) {
    double duration;
    cco_routine (self) {
        while (1) {
            duration = 1.0 + crand64_real()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", self->id, duration*10);
            self->hunger = 0;
            self->state = ph_thinking;
            cco_await_timer_sec(&self->tm, duration);

            printf("Philosopher %d is hungry...\n", self->id);
            self->state = ph_hungry;
            cco_await(self->hunger >= self->left->hunger &&
                      self->hunger >= self->right->hunger);

            duration = 0.5 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", self->id, duration*10);
            self->hunger = 3;
            self->state = ph_eating;
            cco_await_timer_sec(&self->tm, duration);

            // increase the neighbours hunger only if they are already hungry.
            if (self->left->state == ph_hungry) ++self->left->hunger;
            if (self->right->state == ph_hungry) ++self->right->hunger;
        }

        cco_finally:
        printf("Philosopher %d done\n", self->id);
    }
    return 0;
}

// Dining coroutine
struct Dining {
    struct Philosopher philos[num_philosophers];
    cco_state cco; // required
};

int Dining(struct Dining* self) {
    cco_routine (self) {
        for (int i = 0; i < num_philosophers; ++i) {
            cco_reset(&self->philos[i]);
            self->philos[i].id = i + 1;
            self->philos[i].left = &self->philos[(i - 1 + num_philosophers) % num_philosophers];
            self->philos[i].right = &self->philos[(i + 1) % num_philosophers];
        }

        while (1) {
            // NB! local i OK here as it does not cross yield/await.
            for (int i = 0; i < num_philosophers; ++i) {
                Philosopher(&self->philos[i]); // resume until next yield
            }
            cco_yield; // suspend, return control back to caller who
                       // can do other tasks before resuming dining.
        }

        cco_finally:
        for (int i = 0; i < num_philosophers; ++i) {
            cco_stop(&self->philos[i]);
            Philosopher(&self->philos[i]); // execute philos. cco_finally.
        }
        puts("Dining done");
    }
    return 0;
}

int main(void)
{
    struct Dining dining = {0};
    cco_timer tm = cco_make_timer_sec(5.0);
    crand64_seed((uint64_t)time(NULL));

    cco_run_coroutine(Dining(&dining)) {
        if (cco_timer_expired(&tm))
            cco_stop(&dining);
        cco_sleep_sec(0.001); // do other things
    }
}
