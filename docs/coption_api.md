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
#include <stc/coption.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    static coption_long long_options[] = {
      {"verbose", coption_no_argument,       'V'},
      {"help",    coption_no_argument,       'H'},
      {"add",     coption_no_argument,       'a'},
      {"append",  coption_no_argument,       'b'},
      {"delete",  coption_required_argument, 'd'},
      {"create",  coption_required_argument, 'c'},
      {"file",    coption_required_argument, 'f'},
      {NULL}
    };
    coption opt = coption_init();
    int c;
    while ((c = coption_get(&opt, argc, argv, ":if:lr", long_options)) != -1) {
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
                printf("option %s needs a value\n", opt.optstr);
                break;
            case '?':
                printf("unknown option: %s\n", opt.optstr);
                break;
        }
    }

    for (; opt.ind < argc; ++opt.ind) {
        printf("extra arguments: %s\n", argv[opt.ind]);
    }
    return 0;
}
```
