# STC [Coroutines](../include/stc/coroutine.h)
![Coroutine](pics/coroutine.png)

This is small and portable implementation of coroutines.

* Stackful or stackless typesafe coroutines.
* Supports both asymmetric coroutine calls (structured concurrency) and symmetric transfer of control.
* Good ergonomics and minimal boilerplate code.
* Supports "throwing" errors, handled after the `cco_async` scopes during immediate unwinding of the call stack.
* Recovery from errors mechanism.
* Small memory usage and efficient context switching. No heap allocation required by default.
* Coroutines may be cleaned up after the `cco_async` scope. Will also happen on errors and cancelation.
Unhandled errors will exit program with an error message including the offendig throw's line number.

STC coroutines may behave stackless or stackful.
- In stackless mode, the root coroutine object stores the call tree of coroutine objects (typically
on the stack), which holds all variables used within their coroutine scopes. This has the advantages
that they become extremely lightweight and therefore useful on severely memory constrained systems
like microcontrollers with limited resources.
- In stackful mode, the coroutine frames are allocated on the heap just before they await another
coroutine. See examples.


## Methods and statements

#### Coroutine basics
```c++
                cco_async (Coroutine* co) { ... }                   // The coroutine scope.
                cco_yield;                                          // Yield/suspend execution with CCO_YIELD returned.
                cco_yield_v(value);                                 // Yield/suspend execution with value returned.
                cco_await(bool condition);                          // Await for condition to be true else suspend with CCO_AWAIT.
                cco_await_coroutine(coroutine(co));                 // Await for coroutine to finish else suspend with CCO_AWAIT.
                cco_await_coroutine(coroutine(co), int awaitbits);  // Await for coroutine suspend value in (awaitbits | CCO_DONE).
                cco_return;                                         // Jump to `cco_drop:` label and cleanup. If there is no cco_drop:,
                                                                    // or if called after passing it, exit the `cco_async` scope.
                cco_drop:                                           // Label marks where cleanup of coroutine object happens.
bool            cco_is_active(Coroutine* co);                       // Is coroutine active? (= not done).
bool            cco_is_done(Coroutine* co);                         // Is coroutine done?
void            cco_reset_state(Coroutine* co);                     // Reset state to initial (for reuse).
void            cco_stop(Coroutine* co);                            // Next coroutine resume is at cco_done: or after `cco_async` scope.
                cco_run_coroutine(coroutine(co)) {};                // Run blocking until coroutine is finished.
```
- All cco-features must be placed within the cco_async scope.
- Never use return, use cco_return.
- Destruction (cco_drop) may be asynchronous.

#### Tasks (coroutine function-objects) and fibers (green thread-like entity within a thread)
```c++
                cco_task_struct(name) { <name>_base base; ... };    // A custom coroutine task struct; extends cco_task struct.
void            cco_await_task(cco_task* task);                     // Await for task to return CCO_DONE (asymmetric call).
void            cco_await_task(cco_task* task, int awaitbits);      // Await until task's suspend/return value
                                                                    // to be in (awaitbits | CCO_DONE).
void            cco_yield_to(cco_task* task);                       // Yield to task (symmetric control transfer).
void            cco_task_error(int error);                          // Throw an error and unwind call stack after the cco_async block.
                                                                    // Error accessible as `fb->error` and `fb->error_line`.
void            cco_recover_task();                                 // Reset error, and jump to original resume point in current task.
void            cco_resume_task(cco_task* task);                    // Resume suspended task, return value in `fb->result`.
cco_fiber*      cco_new_fiber(cco_task* task);                      // Create an initial fiber from a task.
cco_fiber*      cco_new_fiber(cco_task* task, void* env);           // Create an initial fiber from a task and env (inputs or a future).
cco_fiber*      cco_spawn(cco_task* task);                          // Spawn a new parallel task.
cco_fiber*      cco_spawn(cco_task* task, void* env);               // Same, env may be used as a future or anything.
cco_fiber*      cco_spawn(cco_task* task, void* env, cco_fiber* fb);// Same, env may be used as a future or anything.
bool            cco_joined();                                       // True if there are no other parallel spawned fibers running.

                cco_run_task(cco_task* task) {}                     // Run task blocking until it and spawned fibers are finished.
                cco_run_task(cco_task* task, void *env) {}          // Run task blocking with env data
                cco_run_task(fb_iter, cco_task* task, void *env) {} // Run task blocking. fb_iter reference the current fiber.

                cco_run_fiber(cco_fiber** fiber_ref) {}             // Run fiber(s) blocking.
                cco_run_fiber(fb_iter, cco_fiber* fiber) {}         // Run fiber(s) blocking. fb_iter reference the current fiber.
```
#### Timers and time functions
```c++
                cco_start_timer(cco_timer* tm, double sec);         // Start timer with seconds duration.
                cco_restart_timer(cco_timer* tm);                   // Restart timer with previous duration.
bool            cco_timer_expired(cco_timer* tm);                   // Return true if timer is expired.
double          cco_timer_elapsed(cco_timer* tm);                   // Return elapsed seconds.
double          cco_timer_remaining(cco_timer* tm);                 // Return remaining seconds.
                cco_await_timer(cco_timer* tm, double sec);         // Start timer with duration and await for it to expire.

double          cco_time(void);                                     // Return seconds since 1970-01-01_00:00 UTC (usec precision).
                cco_sleep(double sec);                              // Sleep for seconds (msec or usec precision).

```
#### Semaphores
```c++
cco_semaphore   cco_make_semaphore(long value);                     // Create semaphore
                cco_set_semaphore(cco_semaphore* sem, long value);  // Set initial semaphore value
                cco_acquire_semaphore(cco_semaphore* sem);          // if (count > 0) count -= 1
                cco_release_semaphore(cco_semaphore* sem);          // "Signal" the semaphore (count += 1)
                cco_await_semaphore(cco_semaphore* sem);            // Await for the semaphore count > 0, then count -= 1
```
#### Interoperability with fb_iterators and filters
```c++
                // Container fb_iteration within coroutines
                for (cco_each(fb_iter_name, CntType, cnt)) ...;          // Use an existing fb_iterator (stored in coroutine object)
                for (cco_each_reverse(fb_iter_name, CntType, cnt)) ...;  // Iterate in reverse order

                // c_filter() interoperability with coroutine fb_iterators
                cco_flt_take(int num);                              // Use instead of *c_flt_take(num)* to ensure cleanup
                cco_flt_takewhile(bool predicate);                  // Use instead of *c_flt_takewhile(pred)* to ensure cleanup
```

## Types
| Type name         | Type definition / usage                             | Used to represent... |
|:------------------|:----------------------------------------------------|:---------------------|
|`cco_result`       | **enum** `CCO_DONE`, `CCO_AWAIT`, `CCO_YIELD` | Default set of return values from coroutines |
|`cco_task`         | Base function-object coroutine type                 |                      |
|`cco_timer`        | Timer type                                          |                      |
|`cco_semaphore`    | Semaphore type                                      |                      |
|`cco_taskrunner`   | Coroutine | Executor coroutine which handles asymmetric and<br> symmetric coroutine control flows, |
|`cco_fiber`        | Struct type | Represent a thread-like entity within a thread |

## Rules
1. Avoid declaring local variables within a `cco_async` scope. They are only alive until next `cco_yield..`
or `cco_await..` suspension point. Normally place them in the coroutine struct. Be particularly careful with
control variables in loops.
2. Do not call ***cco_yield\**** or ***cco_await\**** inside a `switch` statement within a
`cco_async` scope. In general, use `if-else-if` to avoid this trap.
3. Resuming a coroutine after it has returned CCO_DONE / 0 is undefined behaviour.
4. There may only be one `cco_async` scope per coroutine.

## Implementation and examples
A regular coroutine may have any signature, however this implementation has specific support for
coroutines which returns `int`, indicating CCO_DONE, CCO_AWAIT, CCO_YIELD, or a custom value.
It should take a struct pointer as one of the parameter which must contains a state-state member named `state`.
The coroutine struct should normally store all *local* variables to be used within the coroutine
(technically those which lifetime crosses a `cco_yield..` or a `cco_await..` statement), along with
*input* and *output* data for the coroutine.

Both asymmetric and symmetric coroutine control flow transfer are supported when using ***tasks***
(closures/functors), and they may be combined.

This implementation is not limited to support only a certain set of coroutine types,
like generators. They operate like stackful coroutines, i.e. tasks can efficiently yield
or await from a deeply nested coroutine call by utilizing extended `cco_task` structs described below.

----
### Generators

Generator are among the simplest types of coroutines and is easy to write:

[ [Run this code](https://godbolt.org/z/xMf3d44Gz) ]
```c++
#include <stdio.h>
#include <stc/coroutine.h>

struct Gen {
    cco_base base;
    int start, end, value;
};

int Gen(struct Gen* g) {
    cco_async (g) {
        for (g->value = g->start; g->value < g->end; ++g->value)
            cco_yield;
    }
    return 0;
}

int main(void) {
    struct Gen gen = {.start=10, .end=20};

    while (Gen(&gen)) {
        printf("%d, ", gen.value);
    }
}
```
To be more expressive, you may use the `cco_run_coroutine()` macro:
```c++
    cco_run_coroutine(Gen(&gen)) {
        printf("%d, ", gen.value);
    }
```

----
### Iterable generators
Although the generator above is simple to use, sometimes it could be useful to iterate through the items
the way you iterate STC containers. Then you may apply [other algorithms](algorithm_api.md) on the sequence as well.
Notice that `Gen` now becomes the "container", while `Gen_iter` is the coroutine:

<details>
<summary>Iterable generator coroutine implementation</summary>

[ [Run this code](https://godbolt.org/z/bbd8oGYxG) ]
```c++
#include <stdio.h>
#include <stc/coroutine.h>

typedef int Gen_value;
typedef struct {
    Gen_value start, end, value;
} Gen;

typedef struct {
    cco_base base;
    Gen* g;
    Gen_value* ref;
} Gen_iter;

int Gen_next(Gen_iter* it) {
    cco_async (it) {
        for (*it->ref = it->g->start; *it->ref < it->g->end; ++*it->ref)
            cco_yield;
    }

    it->ref = NULL; // stop
    return 0;
}

Gen_iter Gen_begin(Gen* g) {
    Gen_iter it = {.g = g, .ref = &g->value};
    Gen_next(&it); // advance to first
    return it;
}

int main(void) {
    Gen gen = {.start=10, .end=20};

    for (c_each(i, Gen, gen)) {
        printf("%d, ", *i.ref);
    }
}
```

</details>

----
### Actor models of concurrency in video games and simulations
A common usage of coroutines is long-running concurrent tasks, often found in video games. An example of this is the
[Dining philosopher's problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem). The following
implementation uses `cco_await` and `cco_yield`. It avoids deadlocks by awaiting for both forks to be
available before aquiring them. It also avoids starvation by increasing both neighbor's hunger when a philosopher
starts eating (because they must be waiting).

<details>
<summary>The "Dining philosophers" C implementation</summary>

[ [Run this code](https://godbolt.org/z/EK6zs6vbz) ]
```c++
#include <stdio.h>
#include <time.h>
#include <stc/random.h>
#include <stc/coroutine.h>

enum {num_philosophers = 5};
enum PhMode {ph_thinking, ph_hungry, ph_eating};

// Philosopher coroutine
struct Philosopher {
    int id;
    cco_timer tm;
    enum PhMode mode;
    int hunger;
    struct Philosopher* left;
    struct Philosopher* right;
    cco_base base; // required
};

int Philosopher(struct Philosopher* self) {
    double duration;
    cco_async (self) {
        while (1) {
            duration = 1.0 + crand64_real()*2.0;
            printf("Philosopher %d is thinking for %.0f minutes...\n", self->id, duration*10);
            self->hunger = 0;
            self->mode = ph_thinking;
            cco_await_timer(&self->tm, duration);

            printf("Philosopher %d is hungry...\n", self->id);
            self->mode = ph_hungry;
            cco_await(self->hunger >= self->left->hunger &&
                      self->hunger >= self->right->hunger);
            self->left->hunger++; // don't starve the neighbours
            self->right->hunger++;

            duration = 0.5 + crand64_real();
            printf("Philosopher %d is eating for %.0f minutes...\n", self->id, duration*10);
            self->hunger = INT32_MAX;
            self->mode = ph_eating;
            cco_await_timer(&self->tm, duration);
        }
    }
    printf("Philosopher %d done\n", self->id);
    return 0;
}

// Dining coroutine
struct Dining {
    struct Philosopher philos[num_philosophers];
    cco_base base; // required
};

int Dining(struct Dining* self) {
    cco_async (self) {
        for (int i = 0; i < num_philosophers; ++i) {
            self->philos[i] = (struct Philosopher){
                .id = i + 1,
                .left = &self->philos[(i - 1 + num_philosophers) % num_philosophers],
                .right = &self->philos[(i + 1) % num_philosophers],
            };
        }

        while (1) {
            // NB! local i OK here as it does not cross yield/await.
            for (int i = 0; i < num_philosophers; ++i) {
                Philosopher(&self->philos[i]); // resume until next yield
            }
            cco_yield; // suspend, return control back to caller who
                       // can do other tasks before resuming dining.
        }
    }

    // cleanup: stop the philosphers
    for (int i = 0; i < num_philosophers; ++i) {
        cco_stop(&self->philos[i]);
        Philosopher(&self->philos[i]); // execute philos cleanup.
    }
    puts("Dining done");
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
```
</details>

----
### Tasks
A task is a coroutine functor/enclosure. The base task type `cco_task`, contains a typesafe function pointer.

#### Error handling with tasks
Tasks allows for scheduling coroutines and more efficient deep nesting of coroutine calls/awaits.
Also, tasks have an excellent error handling mechanism, i.e. an error can be thrown, which will unwind the "call stack",
and errors may be handled and recoveded higher up in the call tree in a simple, structured manner.

<details>
<summary>Implementation of nested awaiting coroutines with error handling</summary>

The following example shows a task `start` which awaits `TaskA`, => awaits `TaskB`, => awaits `TaskC`. `TaskC` throws
an error, which causes unwinding of the call stack. The error is finally handled after `TaskA`'s `cco_async` scope
and recovered using `cco_recover_task()`. This call will resume control back to the original suspension point in the
current task. Because the "call-tree" is fixed, the coroutine frames to be called may be pre-allocated on the stack,
which is very fast.

[ [Run this code](https://godbolt.org/z/z1WWPhsan) ]
<!--{%raw%}-->
```c++
#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (TaskA) { TaskA_base base; int a; };
cco_task_struct (TaskB) { TaskB_base base; double d; };
cco_task_struct (TaskC) { TaskC_base base; float x, y; };

typedef struct {
    struct TaskA A;
    struct TaskB B;
    struct TaskC C;
} Subtasks;

int TaskC(struct TaskC* self) {
    cco_async (self) {
        printf("TaskC start: {%g, %g}\n", self->x, self->y);

        // assume there is an error...
        cco_task_error(99);

        puts("TaskC work");
        cco_yield;
        puts("TaskC more work");

        cco_drop:
        puts("TaskC done");
    }
    return 0;
}

int TaskB(struct TaskB* self) {
    cco_async (self) {
        printf("TaskB start: %g\n", self->d);

        Subtasks* sub = cco_fb()->env;
        cco_await_task(&sub->C);
        puts("TaskB work");

        cco_drop:
        puts("TaskB done");
    }
    return 0;
}

int TaskA(struct TaskA* self) {
    cco_async (self) {
        printf("TaskA start: %d\n", self->a);

        Subtasks* sub = cco_fb()->env;
        cco_await_task(&sub->B);

        puts("TaskA work");

        cco_drop:
        if (cco_fb()->error == 99) {
            // if error not handled, will cause 'unhandled error'...
            printf("TaskA recovered error '99' thrown on line %d\n", cco_fb()->error_line);
            cco_recover_task();
        }
        puts("TaskA done");
    }
    return 0;
}

int start(cco_task* self) {
    cco_async (self) {
        puts("start");

        Subtasks* sub = cco_fb()->env;
        cco_await_task(&sub->A);

        cco_drop:
        puts("done");
    }
    return 0;
}

int main(void)
{
    Subtasks env = {
        {{TaskA}, 42},
        {{TaskB}, 3.1415},
        {{TaskC}, 1.2f, 3.4f},
    };
    cco_task task = {{start}};

    int count = 0;
    cco_run_task(&task, &env) { ++count; }
    printf("resumes: %d\n", count);
}
```
<!--{%endraw%}-->
</details>

#### Coroutines objects allocated on the heap

Sometimes the call-tree is dynamic or more complex, then we can dynamically allocate the coroutine frames before
they are awaited. This is somewhat more general and simpler, but requires heap allocation. Note that the coroutine
frames are now freed at the end of the coroutine functions (after any cleanup after cco_async {}). Example is based on
the previous, but also shows how to use the env field in `cco_fiber` to return a value from the coroutine
call/await:

<details>
<summary>Implementation of coroutine objects on the heap</summary>

[ [Run this code](https://godbolt.org/z/TbWYsbaaq) ]
<!--{%raw%}-->
```c++
#include <stdio.h>
#include <stc/coroutine.h>

cco_task_struct (TaskA) { TaskA_base base; int a; };
cco_task_struct (TaskB) { TaskB_base base; double d; };
cco_task_struct (TaskC) { TaskC_base base; float x, y; };

typedef struct { double value; int error; } Result;

int TaskC(struct TaskC* self) {
    cco_async (self) {
        printf("TaskC start: {%g, %g}\n", self->x, self->y);

        // assume there is an error...
        cco_task_error(99);

        puts("TaskC work");
        cco_yield;

        puts("TaskC more work");
        // initial return value
        cco_env(Result *)->value = self->x * self->y;

        cco_drop:
        puts("TaskC done");
    }

    c_free_n(self, 1);
    return 0;
}

int TaskB(struct TaskB* self) {
    cco_async (self) {
        printf("TaskB start: %g\n", self->d);
        cco_await_task(cco_new_task(TaskC, 1.2f, 3.4f));

        puts("TaskB work");
        cco_env(Result *)->value += self->d;

        cco_drop:
        puts("TaskB done");
    }

    c_free_n(self, 1);
    return 0;
}

int TaskA(struct TaskA* self) {
    cco_async (self) {
        printf("TaskA start: %d\n", self->a);
        cco_await_task(cco_new_task(TaskB, 3.1415));

        puts("TaskA work");
        cco_env(Result *)->value += self->a; // final return value;

        cco_drop:
        if (cco_fb()->error == 99) {
            // if error not handled, will cause 'unhandled error'...
            printf("TaskA recovered error '99' thrown on line %d\n", fb->error_line);

            cco_env(Result *)->error = cco_fb()->error; // set error in output
            cco_recover_task(); // reset error to 0 and jump to past the await TaskB call.
        }
        puts("TaskA done");
    }

    c_free_n(self, 1);
    return 0;
}

int start(cco_task* self) {
    cco_async (self) {
        puts("start");
        cco_await_task(cco_new_task(TaskA, 42));

        cco_drop:
        puts("done");
    }

    c_free_n(self, 1);
    return 0;
}

int main(void)
{
    cco_task* task = c_new(cco_task, {{start}});

    int count = 0;
    Result result = {0};
    cco_run_task(task, &result) { ++count; }

    printf("\nresult: %g, error: %d\n", result.value, result.error);
    printf("resumes: %d\n", count);
}
```
<!--{%endraw%}-->
</details>

----
### Producer-consumer pattern (symmetric coroutines) tasks
Tasks are executed using an *executor*, which is easy to do via the ***cco_run_task()*** macro.
Coroutines awaits (or "calls") other coroutines with ***cco_await_task()***, in which case the awaited coroutine will give
control back to the caller whenever it finishes or reaches a `cco_yield` or another `cco_await*` suspension point in the
code. This is knows as asymmetric calls.

However, coroutines may also transfer control directly to another coroutine using ***cco_yield_to()***.
In this case, control will not be returned back to caller after it finishes or is suspended, known as a symmetric call.
This is useful when two or more coroutines cooperate like in the simple case of the producer-consumer pattern used in
the following example:

<details>
<summary>Producer-consumer coroutine implementation</summary>

[ [Run this code](https://godbolt.org/z/faxr8dz5E) ]
```c++
#include <time.h>
#include <stdio.h>
#include <stc/coroutine.h>
#define T Inventory, int
#include <stc/queue.h>

// Example shows symmetric coroutines producer/consumer style.

cco_task_struct (produce) {
    produce_base base; // must be first (compile-time checked)
    struct consume* consumer;
    Inventory inventory;
    int limit, batch, serial, total;
};

cco_task_struct (consume) {
    consume_base base; // must be first
    struct produce* producer;
};


int produce(struct produce* self) {
    cco_async (self) {
        while (1) {
            if (self->serial > self->total) {
                if (Inventory_is_empty(&self->inventory))
                    cco_return; // cleanup and finish
            }
            else if (Inventory_size(&self->inventory) < self->limit) {
                for (c_range(self->batch))
                    Inventory_push(&self->inventory, ++self->serial);

                printf("produced %d items, Inventory has now %d items:\n",
                       self->batch, (int)Inventory_size(&self->inventory));

                for (c_each(i, Inventory, self->inventory))
                    printf(" %2d", *i.ref);
                puts("");
            }

            cco_yield_to(self->consumer); // symmetric transfer
        }
    }

    cco_cancel_task(self->consumer);
    Inventory_drop(&self->inventory);
    puts("cleanup producer");
    return 0;
}

int consume(struct consume* self) {
    cco_async (self) {
        int n, sz;
        while (1) {
            n = rand() % 10;
            sz = (int)Inventory_size(&self->producer->inventory);
            if (n > sz) n = sz;

            for (c_range(n))
                Inventory_pop(&self->producer->inventory);
            printf("consumed %d items\n", n);

            cco_yield_to(self->producer); // symmetric transfer
        }
    }

    puts("done consumer");
    return 0;
}

int main(void)
{
    srand((unsigned)time(0));
    struct produce producer = {
        .base = {produce},
        .inventory = {0},
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
```

</details>

----
### Scheduled coroutines
The task-objects have the added benefit that coroutines can be managed by a scheduler,
which is useful when dealing with large numbers of coroutines (like in simulations).
Below is a simple coroutine scheduler using a queue. It sends the suspended coroutines
to the end of the queue, and resumes the coroutine in the front.
Note that the scheduler awaits the next CCO_YIELD to be returned, not *only* the default CCO_DONE
(in the code below, `| CCO_DONE` is redundant and only to show how to await for multiple/custom bit-values).

The example heap allocates the coroutine frames on a queue, so that the scheduler can pick the next coroutine to
execute from a pool of coroutines. This also allows it to run on a different thread/scope that may outlive
the scope in that it was created.

<details>
<summary>Scheduled coroutines implementation</summary>

[ [Run this code](https://godbolt.org/z/71GqPYn3e) ]
```c++
// Based on https://www.youtube.com/watch?v=8sEe-4tig_A
#include <stdio.h>
#include <stc/coroutine.h>

#define T Tasks, cco_task*, (c_no_clone)
#define i_keydrop(p) c_free_n(*p, 1) // { puts("free task"); c_free_n(*p, 1); }
#include <stc/queue.h>

cco_task_struct (Scheduler) {
    Scheduler_base base;
    cco_task* _pulled;
    Tasks tasks;
};

cco_task_struct (TaskA) {
    TaskA_base base;
};

cco_task_struct (TaskX) {
    TaskX_base base;
    char id;
};

int scheduler(struct Scheduler* self) {
    cco_async (self) {
        while (!Tasks_is_empty(&self->tasks)) {
            self->_pulled = Tasks_pull(&self->tasks);

            cco_await_task(self->_pulled, CCO_YIELD | CCO_DONE);

            if (cco_fb()->result == CCO_YIELD) {
                Tasks_push(&self->tasks, self->_pulled);
            } else { // CCO_DONE
                Tasks_value_drop(&self->tasks, &self->_pulled);
            }
        }

        cco_drop:
        Tasks_drop(&self->tasks);
        puts("Task queue dropped");
    }
    return 0;
}

static int taskX(struct TaskX* self) {
    cco_async (self) {
        printf("Hello from task %c\n", self->id);
        cco_yield;
        printf("%c is back doing work\n", self->id);
        cco_yield;
        printf("%c is back doing more work\n", self->id);
        cco_yield;

        cco_done:
        printf("%c is done\n", self->id);
    }

    return 0;
}

static int TaskA(struct TaskA* self) {
    cco_async (self) {
        puts("Hello from task A");
        cco_yield;

        puts("A is back doing work");
        cco_yield;

        puts("A adds task C");
        Tasks *_tasks = cco_fb()->env; // local var only alive until cco_yield.
        Tasks_push(_tasks, cco_cast_task(c_new(struct TaskX, {.base={taskX}, .id='C'})));
        cco_yield;

        puts("A is back doing more work");
        cco_yield;

        cco_drop:
        puts("A is done");
    }
    return 0;
}

int main(void) {
    // Allocate everything on the heap, so it could be ran in another thread.
    struct Scheduler* schedule = c_new(struct Scheduler, {
        .base={scheduler},
        .tasks = c_make(Tasks, {
            cco_cast_task(c_new(struct TaskA, {.base={TaskA}})),
            cco_cast_task(c_new(struct TaskX, {.base={taskX}, .id='B'})),
        })
    });

    cco_run_task(schedule, &schedule->tasks);

    // schedule is now cleaned up/destructed, free heap mem.
    c_free_n(schedule, 1);
}
```

</details>
