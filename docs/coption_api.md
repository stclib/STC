# STC [coption](../include/stc/coption.h): Command line argument parsing

This describes the API of the *coption_get()* function for command line argument parsing.

See [getopt_long](https://www.freebsd.org/cgi/man.cgi?getopt_long(3)) for a similar posix function.

## Types

```c
typedef enum {
    coption_no_argument,
    coption_required_argument,
    coption_optional_argument
} coption_type;

typedef struct {
    const char *name;
    coption_type type;
    int val;
} coption_long;

typedef struct {
    int ind;            /* equivalent to posix optind */
    int opt;            /* equivalent to posix optopt */
    const char *optstr; /* points to the option string, if any */
    const char *arg;    /* equivalent to posix optarg */
    ...
} coption;
```

## Methods

```c
coption         coption_init(void);
int             coption_get(coption *opt, int argc, char *argv[],
                            const char *shortopts, const coption_long *longopts);
```

## Example

```c
#include <stdio.h>
#include <stc/coption.h>

int main(int argc, char *argv[]) {
    coption_long longopts[] = {
        {"foo", coption_no_argument,       'f'},
        {"bar", coption_required_argument, 'b'},
        {"opt", coption_optional_argument, 'o'},
        {0}
    };
    const char* shortopts = "xy:z::123";
    if (argc == 1) 
        printf("Usage: program -x -y ARG -z [ARG] -1 -2 -3 --foo --bar ARG --opt [ARG] [ARGUMENTS]\n", argv[0]);
    coption opt = coption_init();
    int c;
    while ((c = coption_get(&opt, argc, argv, shortopts, longopts)) != -1) {
        switch (c) {
            case '?': printf("error: unknown option: %s\n", opt.optstr); break;
            case ':': printf("error: missing argument for %s\n", opt.optstr); break;
            default:  printf("option: %c [%s]\n", opt.opt, opt.arg ? opt.arg : ""); break;
        }
    }
    printf("\nNon-option arguments:");
    for (int i = opt.ind; i < argc; ++i)
        printf(" %s", argv[i]);
    putchar('\n');
}
    return 0;
}
```
