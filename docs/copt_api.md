# Module [copt](../stc/copt.h): Command line argument parsing

This describes the API of string type *copt_get()* function for command line argument parsing.
See [getopt_long](https://www.freebsd.org/cgi/man.cgi?getopt_long(3)) for a similar freebsd function.

## Types

```c
enum {
    copt_no_argument = 0,
    copt_required_argument = 1,
    copt_optional_argument = 2
};
typedef struct {
    int ind;             /* equivalent to optind */
    int opt;             /* equivalent to optopt */
    const char *arg;     /* equivalent to optarg */
    const char *faulty;  /* points to the faulty option, if any */
    int longindex;       /* index of long option; or -1 if short */
    ...
} copt_t;

typedef struct {
    const char *name;
    int has_arg;
    int val;
} copt_long_t;

const copt_t copt_inits;
```

## Methods

```c
copt_t              copt_init(void);
int                 copt_get(copt_t *opt, int argc, char *argv[],
                             const char *shortopts, const copt_long_t *longopts);
```

## Example

```c
#include <stdio.h>
#include "stc/copt.h"

int main(int argc, char *argv[]) {
    static copt_long_t long_options[] = {
      {"verbose", copt_no_argument,       'V'},
      {"help",    copt_no_argument,       'H'},
      {"add",     copt_no_argument,       'a'},
      {"append",  copt_no_argument,       'b'},
      {"delete",  copt_required_argument, 'd'},
      {"create",  copt_required_argument, 'c'},
      {"file",    copt_required_argument, 'f'},
      {NULL}
    };
    copt_t opt = copt_inits;
    int c;
    while ((c = copt_get(&opt, argc, argv, ":if:lr", long_options)) != -1) {
        switch (c) {
            case 'V': case 'H':
            case 'a': case 'b':
            case 'd': case 'c':
            case 'i': case 'l':
            case 'r':
                printf("option: %c\n", c);
                break;
            case 'f':
                printf("filename: %s\n", opt.arg);
                break;
            case ':':
                printf("option %s needs a value\n", opt.faulty);
                break;
            case '?':
                printf("unknown option: %s\n", opt.faulty);
                break;
        }
    }

    for (; opt.ind < argc; ++opt.ind) {
        printf("extra arguments: %s\n", argv[opt.ind]);
    }
    return 0;
}
```
