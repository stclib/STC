# STC [coroutine](../include/stc/coroutine.h): Coroutines
![Coroutine](pics/coroutine.jpg)

An implementation of stackless coroutines, which are lightweight threads, providing a blocking
context cheaply using little memory per coroutine.

Coroutines are used to accomplish a non-preempted form of concurrency known as cooperative multitasking and, therefore, do not incur context switching when yielding to another thread. Within a coroutine, **yield** and **await** is accomplished by utilizing Duff's device within a thread's function and an external variable used in within the switch statement. This allows jumping (resuming) from a yield upon another function call. In order to block threads, these yields may be guarded by a conditional so that successive calls to the same function will yield unless the guard conditional is true.

Because these coroutines are stackless, local variables within the coroutine where usage crosses `cco_yield` or `cco_await` must be stored in a struct which is passed as pointer to the coroutine. This has the advantages that they become very lightweight and therefore useful on severely memory constrained systems like small microcontrollers where other solutions are impractical or less desirable.

### Coroutine API

NB! ***cco_yield\*()*** / ***cco_await\*()*** may not be called from within a `switch` statement in a
`cco_scope` scope; Use `if-else-if` constructs instead.

|           |  Function / operator                 | Description                             |
|:----------|:-------------------------------------|:----------------------------------------|
|`cco_result` | `CCO_DONE`, `CCO_AWAIT`, `CCO_YIELD` | Default set of return values from coroutines |
|           | `cco_final:`                         | Label for cleanup position in coroutine |
| `bool`    | `cco_done(co)`                       | Is coroutine done?                      |
|           | `cco_scope(co) {}`                   | The coroutine scope                     |
|           | `cco_yield;`                         | Yield/suspend execution (return CCO_YIELD)|
|           | `cco_yield_v(ret);`                  | Yield/suspend execution (return ret)    |
|           | `cco_yield_final;`                   | Yield final suspend, enter cleanup-state |
|           | `cco_yield_final_v(ret);`            | Yield with a final value                 |
|           | `cco_await(condition);`              | Suspend until condition is true (return CCO_AWAIT)|
|           | `cco_await_coroutine(cocall);`       | Await for subcoro to finish (returns its ret value) |
|           | `cco_await_coroutine(cocall, retbit);` | Await for subcoro's return to be in (retbit \| CCO_DONE)  |
|           | `cco_return;`                        | Return from coroutine (inside cco_scope) |
|           | ***c_filter() interoperability with coroutine iterators***: ||
|           | `cco_flt_take(num);`                 | Use instead of *c_flt_take(num)* to ensure cleanup state |
|           | `cco_flt_takewhile(predicate);`      | Use instead of *c_flt_takewhile(pred)* to ensure cleanup state |
|           | `cco_flt_takewhile(predicate);`      | Use instead of *c_flt_takewhile(pred)* to ensure cleanup state |
|           | ***Container iteration in coroutines***: ||
|           | `c_foreach_it(external_it, ctype, cnt)` | Use iterator stored in coroutine object |
|           | `c_foreach_reverse_iter(external_it, ctype, cnt)` | Iterate in reverse order     |
|           | ***Task objects***:                  ||
|           | `cco_task_struct(Name, ...);`        | Define a coroutine task struct          |
|           | `cco_await_task(task, cco_runtime* rt);`| Await for task to finish             |
|           | `cco_await_task(task, rt, retbit);`  | Await for task's return to be in (retbit \| CCO_DONE) |
|`cco_result`| `cco_resume_task(task, rt);`        | Resume suspended task                   |
|           | ***Semaphores***:                    ||
|           | `cco_semaphore`                      | Semaphore type                          |
|           | `cco_await_semaphore(sem)`           | Await for the semaphore count > 0       |
|`cco_semaphore`| `cco_semaphore_from(long value)` | Create semaphore                        |
|           | `cco_semaphore_set(sem, long value)` | Set semaphore value                     |
|           | `cco_semaphore_release(sem)`         | Signal the semaphore (count += 1)       |
|           | ***Timers***:                        ||
|           | `cco_timer`                          | Timer type                              |
|           | `cco_await_timer(tm, double sec)`    | Await secs for timer to expire (usec prec.)|
|           | `cco_timer_start(tm, double sec)`    | Start timer for secs duration           |
|           | `cco_timer_restart(tm)`              | Restart timer with same duration        |
| `bool`    | `cco_timer_expired(tm)`              | Return true if timer is expired         |
| `double`  | `cco_timer_elapsed(tm)`              | Return seconds elapsed                  |
| `double`  | `cco_timer_remaining(tm)`            | Return seconds remaining                |
|           | ***From caller side***:              ||
| `void`    | `cco_stop(co)`                       | Next call of coroutine finalizes        |
| `void`    | `cco_reset(co)`                      | Reset state to initial (for reuse)      |
| `void`    | `cco_run_coroutine(cocall) {}`       | Run blocking until cocall is finished   |
|           | `cco_run_task(task) {}`         | Run blocking until task is finished |
|           | `cco_run_task(task, rt, STACKSZ) {}`| Run blocking until task is finished |
|           | ***Time functions***:                ||
| `double`  | `cco_time(void)`                     | Return secs with usec prec. since Epoch |
|           | `cco_sleep(double sec)`              | Sleep for seconds (msec or usec prec.)  |

## Implementation and examples

This small implementation of coroutines is inspired by
[Simon Tatham's coroutines](https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html) and
[Adam Dunkel's photothreads](https://dunkels.com/adam/pt), but provides big improvements regarding
ergonomics, features, and type-safety. Crucially, it also allows coroutines to self-cleanup when
cancelled (not resumed until they are done).

A coroutine function may have almost any signature, but the implementation adds support for
coroutines which returns an int, indicating CCO_DONE, CCO_AWAIT, or CCO_YIELD. It should also
take a struct pointer as parameter which must contains the member `int cco_state`. The struct should
normally store all *local* variables to be used within the coroutine, along with *input* and *output* data
for the coroutine.

Note that this implementation is not limited to support a certain set of coroutine types,
like generators. It can even operate like stackfull coroutines, i.e. you can efficiently
yield or await from a (deeply) nested coroutine call using cco_task objects described later.

The first example is a generator of Pythagorian triples, and stops when diagonal size > max_c.

[ [Run this code](https://godbolt.org/z/qrbP4fffr) ]
```c
#include "stc/coroutine.h"
#include <stdio.h>
#include <stdio.h>
// https://quuxplusone.github.io/blog/2019/03/06/pythagorean-triples/

struct triples {
    int max_c;     // input: max c.
    int a, b, c;   // output
    int cco_state; // required member
};

int triples(struct triples* i) {
    cco_scope(i) {  // the coroutine scope!
        for (i->c = 5;; ++i->c) {
            for (i->a = 1; i->a < i->c; ++i->a) {
                for (i->b = i->a + 1; i->b < i->c; ++i->b) {
                    if ((int64_t)i->a * i->a +
                        (int64_t)i->b * i->b ==
                        (int64_t)i->c * i->c)
                    {
                        if (i->c > i->max_c)
                            cco_return; // "jump" to cco_final if defined, else exit scope.
                        cco_yield;
                    }
                }
            }
        }
        cco_final:
        puts("done");
    }
    return 0; // CCO_DONE
}

int main(void) {
    struct triples co = {.max_c = 25};
    int n = 0;

    cco_run_coroutine(triples(&co)) {
        printf("%d: [%d, %d, %d]\n", ++n, co.a, co.b, co.c);
    }
}
```
The next variant skips the triples which are upscaled version of smaller ones by checking
the gcd() function. Note that the gcd1_triples struct contains the triples struct so that
both functions have separate call frames:

[ [Run this code](https://godbolt.org/z/c5GzGo1Px) ]
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
    struct triples tri;    // triples call frame
    int cco_state;
};

int gcd1_triples(struct gcd1_triples* i)
{
    cco_scope(i) {
        cco_reset(&i->tri);
        i->tri.max_c = i->max_c;

        while (triples(&i->tri) != CCO_DONE) {
            // Skip triples with GCD(a,b) > 1
            if (gcd(i->tri.a, i->tri.b) > 1)
                continue;

            // Done when count > max_n
            if (++i->count > i->max_n)
                cco_return;
            else
                cco_yield;
        }
        cco_final:
        cco_stop(&i->tri); // to cleanup state if still active
        triples(&i->tri);  // do cleanup (or no-op if done)
    }
    return 0;
}

int main(void) {
    struct gcd1_triples co = {.max_n = 100, .max_c = 100};
    int n = 0;

    cco_run_coroutine(gcd1_triples(&co)) {
        printf("%d: [%d, %d, %d]\n", ++n, co.tri.a, co.tri.b, co.tri.c);
    }
}
```
When using ***cco_run_coroutine()***, the coroutine is continuously resumed after each yield suspension.
However, this means that it first calls ***gcd1_triples()***, which immediately jumps to after the `cco_yield`
-statement and calls ***triples()***, which again jumps and resumes after its `cco_yield`-statement. This
is efficient only when yielding or awaiting from the top- or second-level call like here, but naturally not
when couroutine calls are more deeply nested or recursive.

The STC coroutine implementation therefore also contains task-objects (`cco_task`), which are base-coroutine
objects/enclosures. These can be executed using ***cco_run_task()*** instead of ***cco_run_coroutine()***.
Inner coroutine calls are done by ***cco_await_task()***, where you may await for a certain return value, normally CCO_YIELD or just CCO_DONE. It uses a stack of pointers of task-enclosures to call the current
inner-level function directly. The task-objects have the added benefit that coroutines can be managed
by a scheduler, which is useful when dealing with large numbers of coroutines (like in many games and
simulations).

Note that these two modes may be mixed, and that for short-lived coroutines (only a few suspends),
it is often beneficial to call the sub-coroutine directly rather than via ***cco_await_task()***
(which pushes the task on top of the runtime stack and yields - then executed on next resume).

The following example uses task-objects with 3-levels deep coroutine calls. It emulates an async generator
by awaiting a few seconds before producing a number, using a timer.
```c
// https://mariusbancila.ro/blog/2020/06/22/a-cpp20-coroutine-example/
#include <time.h>
#include <stdio.h>
#define i_static
#include "stc/cstr.h"
#include "stc/coroutine.h"

cco_task_struct (next_value,
    int val;
    cco_timer tm;
);

int next_value(struct next_value* co, cco_runtime* rt)
{
    cco_scope (co) {
        while (true) {
            cco_await_timer(&co->tm, 1 + rand() % 2); // suspend with CCO_AWAIT
            co->val = rand();
            cco_yield;                                // suspend with CCO_YIELD
        }
    }
    return 0;
}

void print_time()
{
    time_t now = time(NULL);
    char mbstr[64];
    strftime(mbstr, sizeof(mbstr), "[%H:%M:%S]", localtime(&now));
    printf("%s ", mbstr);
}

// PRODUCER
cco_task_struct (produce_items,
    struct next_value next;
    cstr text;
);

int produce_items(struct produce_items* p, cco_runtime* rt)
{
    cco_scope (p) {
        p->text = cstr_init();
        p->next.cco_func = next_value;
        while (true)
        {
            // await for CCO_YIELD (or CCO_DONE)
            cco_await_task(&p->next, rt, CCO_YIELD);
            cstr_printf(&p->text, "item %d", p->next.val);
            print_time();
            printf("produced %s\n", cstr_str(&p->text));
            cco_yield;
        }
        cco_final:
        cstr_drop(&p->text);
        puts("done produce");
    }
    return 0;
}

// CONSUMER
cco_task_struct (consume_items,
    int n, i;
    struct produce_items produce;
);

int consume_items(struct consume_items* c, cco_runtime* rt)
{
   cco_scope (c) {
        c->produce.cco_func = produce_items;

        for (c->i = 1; c->i <= c->n; ++c->i)
        {
            printf("consume #%d\n", c->i);
            cco_await_task(&c->produce, rt, CCO_YIELD);
            print_time();
            printf("consumed %s\n", cstr_str(&c->produce.text));
        }
        cco_final:
        cco_stop(&c->produce);
        cco_resume_task(&c->produce, rt);
        puts("done consume");
    }
    return 0;
}

int main(void)
{
    struct consume_items consume = {
        .n = 5,
        .cco_func = consume_items,
    };
    cco_run_task(&consume);
}
```
