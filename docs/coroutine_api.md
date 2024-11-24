# STC [coroutine](../include/stc/coroutine.h): Coroutines
![Coroutine](pics/coroutine.jpg)

An implementation of stackless coroutines, which are lightweight threads, providing a blocking
context cheaply using little memory per coroutine.

Coroutines are used to accomplish a non-preempted form of concurrency known as cooperative multitasking and, therefore, do not incur context switching when yielding to another thread. Within a coroutine, **yield** and **await** is accomplished by utilizing Duff's device within a thread's function and an external variable used in within the switch statement. This allows jumping (resuming) from a yield upon another function call. In order to block threads, these yields may be guarded by a conditional so that successive calls to the same function will yield unless the guard conditional is true.

Because these coroutines are stackless, local variables within the coroutine where usage crosses `cco_yield` or `cco_await` must be stored in a struct which is passed as pointer to the coroutine. This has the advantages that they become very lightweight and therefore useful on severely memory constrained systems like small microcontrollers where other solutions are impractical or less desirable.

## Methods and statements

- Local variables must not be declared within a `cco_routine` scope. Place them in the coroutine struct.
- ***cco_yield\**** / ***cco_await\**** must not be called inside a `switch` statement within a
`cco_routine` scope; use `if-else-if` constructs instead.

#### Coroutine basics
```c
                cco_routine (Coroutine* co) { ... }                 // The coroutine scope.
                cco_yield;                                          // Yield/suspend execution with CCO_YIELD.
                cco_yield_v(int value);                             // Yield/suspend execution with (bit)value.
                cco_yield_final;                                    // Yield final suspend, enter cleanup-state.
                cco_yield_final_v(int value);                       // Yield with a final (bit)value.
                cco_await(bool condition);                          // Suspend until condition is true (return CCO_AWAIT).
                cco_await_coroutine(coroutine(co));                 // Await for coroutine to finish.
                cco_await_coroutine(coroutine(co), int awaitbits);  // Await for coroutine suspend value in (awaitbits | CCO_DONE)
                cco_return;                                         // Execute `cco_cleanup:` section if specified,
                                                                    // then set coroutine in done state and return.
bool            cco_active(Coroutine* co);                          // Is coroutine active? (= not done).
bool            cco_done(Coroutine* co);                            // Is coroutine done?
void            cco_reset_state(Coroutine* co);                     // Reset state to initial (for reuse).
void            cco_stop(Coroutine* co);                            // Next resume of coroutine enters `cco_cleanup:`.
                cco_run_coroutine(coroutine(co)) {};                // Run blocking until coroutine is finished.
```
#### Task specific (coroutine function objects)
```c
                cco_task_struct(name) { <name>_state cco; ... };    // A custom coroutine task struct; extends cco_task struct.
void            cco_await_task(cco_task* task, cco_runtime* rt);    // Await for task to return CCO_DONE (asymmetric call).
void            cco_await_task(cco_task* task, cco_runtime* rt, int awaitbits); // Await until task's suspend/return value
                                                                                // to be in (awaitbits | CCO_DONE).
void            cco_yield_to(cco_task* task, cco_runtime* rt);      // Yield to task (symmetric control transfer).
void            cco_throw_error(uint16_t error, cco_runtime* rt);   // Throw an error and unwind call stack at cco_setup: label,
                                                                    // accessible as `rt->error` and `rt->error_line`.
void            cco_resume_task(cco_task* task, cco_runtime* rt);   // Resume suspended task, return value in `rt->result`.
                cco_run_task(cco_task* task) {};                    // Run blocking until task is finished.
```
#### Timers and time functions
```c
                cco_await_timer_sec(cco_timer* tm, double sec);     // Await seconds for timer to expire
                cco_start_timer_sec(cco_timer* tm, double sec);     // Start timer with seconds duration.
                cco_restart_timer(cco_timer* tm);                   // Restart timer with previous duration.
bool            cco_timer_expired(cco_timer* tm);                   // Return true if timer is expired.
double          cco_timer_elapsed_sec(cco_timer* tm);               // Return elapsed seconds.
double          cco_timer_remaining_sec(cco_timer* tm);             // Return remaining seconds.

double          cco_time(void);                                     // Return secs with usec precision since Epoch.
                cco_sleep_sec(double sec);                          // Sleep for seconds (msec or usec. precision)

```
#### Semaphores
```c
cco_semaphore   cco_make_semaphore(long value);                     // Create semaphore
                cco_set_semaphore(cco_semaphore* sem, long value);  // Set initial semaphore value
                cco_await_semaphore(cco_semaphore* sem);            // Await for the semaphore count > 0, then count -= 1
                cco_release_semaphore(cco_semaphore* sem);          // "Signal" the semaphore (count += 1)
```
#### Interoperability with iterators and filters
```c
                // Container iteration within coroutines
                cco_foreach (iter_name, ctype, cnt);                // Use an existing iterator (stored in coroutine object)
                cco_foreach_reverse (iter_name, ctype, cnt);        // Iterate in reverse order

                // c_filter() interoperability with coroutine iterators
                cco_flt_take(int num);                              // Use instead of *c_flt_take(num)* to ensure cleanup
                cco_flt_takewhile(bool predicate);                  // Use instead of *c_flt_takewhile(pred)* to ensure cleanup
```

## Types
| Type name         | Type definition / usage                             | Used to represent... |
|:------------------|:----------------------------------------------------|:---------------------|
|`cco_result`       | `CCO_DONE`, `CCO_AWAIT`, `CCO_YIELD`                | Default set of return values from coroutines |
|`cco_cleanup:`     | Label inside coroutine | Label at cleanup position in coroutine |
|`cco_task`         | Function object coroutine type |
|`cco_timer`        | Timer type |
|`cco_semaphore`    | Semaphore type |
|`cco_taskrunner`   | Coroutine | Executor coroutine which handles asymmetric and<br> symmetric coroutine control flows, |
|`cco_runtime`      | Struct type | Runtime object to manage cco_taskrunner states |

## Implementation and examples

This small implementation of coroutines is inspired by
[Simon Tatham's coroutines](https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html) and
[Adam Dunkel's photothreads](https://dunkels.com/adam/pt). However, it provides huge improvements
over these with regards to the ergonomics, features, and type-safety. Crucially, this implementation
allows coroutines to self-cleanup when cancelled (not resumed until they are done).

A regular coroutine function may have any signature, but the implementation adds support for
coroutines which returns an int, indicating CCO_DONE, CCO_AWAIT, or CCO_YIELD. It should
take a struct pointer as one of the parameter which must contains a state member struct `cco_state cco`.
The struct should normally store all *local* variables to be used within the coroutine (technically those which lifetime
crosses a `cco_yield*` or `cco_await*()` statement), along with *input* and *output* data for the coroutine.

Both asymmetric and symmetric coroutine control flow transfer are supported when using ***tasks***
(closures/functors), and they may be combined.

Note that this implementation is not limited to support a certain set of coroutine types,
like generators. It can even operate like stackfull coroutines, i.e. it can efficiently
yield or await from a (deeply) nested coroutine call using cco_task objects described below.

An example of a generator of Pythagorian triples. It stops when diagonal size > max_c.

[ [Run this code](https://godbolt.org/z/r7h3K3e5j) ]
```c
#include <stdio.h>
#include "stc/coroutine.h"

struct triples {
    cco_state cco;   // required member
    int64_t max_c;   // input: max c.
    int64_t a, b, c; // output
};

int triples(struct triples* co) {
    cco_routine (co) {  // the coroutine scope!
        for (co->c = 5;; ++co->c) {
            for (co->a = 1; co->a < co->c; ++co->a) {
                for (co->b = co->a + 1; co->b < co->c; ++co->b) {
                    if (co->a * co->a +
                        co->b * co->b ==
                        co->c * co->c)
                    {
                        if (co->c > co->max_c)
                            cco_return; // "jump" to cco_cleanup if defined, else exit scope.
                        cco_yield;
                    }
                }
            }
        }
        cco_cleanup:
        puts("done");
    }
    return 0; // CCO_DONE
}

int main(void) {
    struct triples co = {.max_c = 25};
    int n = 0;

    cco_run_coroutine(triples(&co)) {
        printf("%d: [%d, %d, %d]\n", ++n, (int)co.a, (int)co.b, (int)co.c);
    }
}
```
The next variant skips the triples which are upscaled version of smaller ones by checking
the gcd() function. Note that the gcd1_triples struct contains the triples struct so that
both functions have separate call frames:

[ [Run this code](https://godbolt.org/z/afPsf5xsn) ]
```c
int gcd(int a, int b) { // greatest common denominator
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

struct gcd1_triples {
    int max_n, max_c, count; // input: max_n, max_c limit #triples to be generated.
    struct triples tri;      // triples call frame
    cco_state cco;           // required
};

int gcd1_triples(struct gcd1_triples* co)
{
    cco_routine (co) {
        cco_reset(&co->tri);
        co->tri.max_c = co->max_c;

        while (triples(&co->tri) != CCO_DONE) {
            // Skip triples with GCD(a,b) > 1
            if (gcd(co->tri.a, co->tri.b) > 1)
                continue;

            // Done when count > max_n
            if (++co->count > co->max_n)
                cco_return;
            else
                cco_yield;
        }
        cco_cleanup:
        cco_stop(&co->tri); // to cleanup state if still active
        triples(&co->tri);  // do cleanup (or no-op if done)
    }
    return 0;
}

int main(void) {
    struct gcd1_triples co = {.max_n = 100, .max_c = 100};
    int n = 0;

    cco_run_coroutine(gcd1_triples(&co)) {
        printf("%d: [%d, %d, %d]\n", ++n, (int)co.a, (int)co.b, (int)co.c);
    }
}
```
When using ***cco_run_coroutine()***, the coroutine is continuously resumed after each yield suspension.
However, this means that it first calls ***gcd1_triples()***, which immediately jumps to after the `cco_yield`
-statement and calls ***triples()***, which again jumps and resumes after its `cco_yield`-statement. This
is efficient only when yielding or awaiting from the top- or second-level call like here, but naturally not
when couroutine calls are more deeply nested or recursive.

The STC coroutine implementation therefore also contains task-objects (`cco_task`), which are functors/closures.
These should be executed using ***cco_run_task()*** instead of ***cco_run_coroutine()***.
Inner coroutine calls are done by ***cco_await_task()***, where you may await for a certain return value, normally CCO_YIELD or just CCO_DONE. It uses a stack of pointers of task-enclosures to call the current
inner-level function directly. The task-objects have the added benefit that coroutines can be managed
by a scheduler, which is useful when dealing with large numbers of coroutines (like in many games and
simulations).

Note that these two modes may be mixed, and that for short-lived coroutines (only a few suspends),
it is often beneficial to call the sub-coroutine directly rather than via ***cco_await_task()***
(which pushes the task on top of the runtime stack and yields - then executed on next resume).

The following example uses task-objects with 3-levels deep coroutine calls. It emulates an async generator
by awaiting a few seconds before producing a number, using a timer.

[ [Run this code](https://godbolt.org/z/nqPKG6s6W) ]
```c
#include <time.h>
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/coroutine.h"

struct next_value {
    int val;
    cco_timer tm;
    cco_state cco;
};

int next_value(struct next_value* co) {
    cco_routine (co) {
        while (true) {
            cco_await_timer(&co->tm, 1 + rand() % 2);
            co->val = rand();
            cco_yield;
        }
    }
    return 0;
}

void print_time(void) {
    time_t now = time(NULL);
    char mbstr[64];
    strftime(mbstr, sizeof(mbstr), "[%H:%M:%S]", localtime(&now));
    printf("%s ", mbstr);
}

// PRODUCER
struct produce {
    struct next_value next;
    cstr text;
    cco_state cco;
};

int produce(struct produce* pr) {
    cco_routine (pr) {
        pr->text = cstr_init();
        while (true) {
            cco_await_coroutine(next_value(&pr->next), CCO_YIELD);
            cstr_printf(&pr->text, "item %d", pr->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&pr->text));
            cco_yield;
        }

        cco_cleanup:
        cstr_drop(&pr->text);
        puts("done producer");
    }
    return 0;
}

// CONSUMER
struct consume {
    int n, i;
    cco_state cco;
};

int consume(struct consume* co, struct produce* pr) {
   cco_routine (co) {
        for (co->i = 1; co->i <= co->n; ++co->i) {
            printf("consumer #%d\n", co->i);
            cco_await_coroutine(produce(pr), CCO_YIELD);
            print_time();
            printf("consumed %s\n", cstr_str(&pr->text));
        }

        cco_cleanup:
        puts("done consumer");
    }
    return 0;
}

int main(void) {
    struct produce producer = {0};
    struct consume consumer = {.n=5};
    int count = 0;

    cco_run_coroutine(consume(&consumer, &producer)) {
        ++count;
    }
    cco_stop(&producer);
    produce(&producer);
    printf("count: %d\n", count);
}
```
