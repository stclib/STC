#include <stc/coption.h>
#include <stdio.h>

// mkdir option parsing.
struct AppArgs { 
    const char *mode;
    bool parent;
    bool verbose;
    const char *context;
    int index;
};

int parseArgs(int argc, char *argv[], struct AppArgs *args)
{
    const char *shortopts = "pvm:Z";
    coption_long longopts[] = {
        {"mode",    coption_required_argument, 'm'},
        {"parent",  coption_no_argument,       'p'},
        {"verbose", coption_no_argument,       'v'},
        {"context", coption_optional_argument, 'Z'},
        {"help",    coption_no_argument,       'H'},
        {"version", coption_no_argument,       'V'},
        {NULL}
    };
    *args = (struct AppArgs){NULL};
    coption opt = coption_init();
    int c;
    while ((c = coption_get(&opt, argc, argv, shortopts, longopts)) != -1) {
        switch (c) {
            case 'm': args->mode = opt.arg; break;
            case 'p': args->parent = true; break;
            case 'v': args->verbose = true; break;
            case 'Z': args->context = opt.arg? opt.arg : "DEFAULT"; break;
            case 'H':
                printf("Usage: mkdir [OPTION]... DIRECTORY...\n"
                       "Create the DIRECTORY(ies), if they do not already exist.\n\n"
                       "Mandatory arguments to long options are mandatory for short options too.\n"
                       "  -m, --mode=MODE   set file mode (as in chmod), not a=rwx - umask\n"
                       "  -p, --parents     no error if existing, make parent directories as needed\n"
                       "  -v, --verbose     print a message for each created directory\n"
                       "  -Z                   set SELinux security context of each created directory\n"
                       "                         to the default type\n"
                       "      --context[=CTX]  like -Z, or if CTX is specified then set the SELinux\n"
                       "                         or SMACK security context to CTX\n"
                       "      --help     display this help and exit\n"
                       "      --version  output version information and exit\n\n"
                       "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n"
                       "Full documentation at: <https://www.gnu.org/software/coreutils/mkdir>\n"
                       "or available locally via: info '(coreutils) mkdir invocation'\n");
                return 1;
            case 'V': 
                printf("mkdir (GNU coreutils) 8.30\n"
                       "Copyright (C) 2018 Free Software Foundation, Inc.\n"
                       "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
                       "This is free software: you are free to change and redistribute it.\n"
                       "There is NO WARRANTY, to the extent permitted by law.\n");
                return 1;
            case ':':
                printf("mkdir: option '%s' requires an argument\n"
                       "Try 'mkdir --help' for more information.\n", opt.optstr);
                return -1;
            case '?':
                printf("mkdir: invalid option '%s'\n"
                       "Try 'mkdir --help' for more information.\n", opt.optstr);
                return -2;
        }
    }

    if (opt.ind == argc) {
        printf("mkdir: missing operand\n"
               "Try 'mkdir --help' for more information.\n");
        return 2;
    }
    args->index = opt.ind;
    return 0;
}


int main(int argc, char* argv[])
{
    struct AppArgs args;
    int ret = parseArgs(argc, argv, &args);
    if (ret != 0)
        return ret < 0? ret : 0;

    printf("mkdir:");
    for (int i=args.index; i < argc; ++i) {
        printf(" %s\n", argv[i]);
    }
    printf("  mode:%s\n  parent:%d\n  verbose:%d\n  context:%s\n",
           args.mode? args.mode:"",
           args.parent, args.verbose,
           args.context? args.context:"");
}
