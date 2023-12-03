/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
Inspired by https://attractivechaos.wordpress.com/2018/08/31/a-survey-of-argument-parsing-libraries-in-c-c
Fixed major bugs with optional arguments (both long and short).
Added arg->optstr output field, more consistent API.

coption_get() is similar to GNU's getopt_long(). Each call parses one option and
returns the option name. opt->arg points to  the option argument if present.
The function returns -1 when all command-line arguments are parsed. In this case,
opt->ind is the index of the first non-option argument.

#include <stdio.h>
#include "stc/coption.h"

int main(int argc, char *argv[])
{
    coption_long longopts[] = {
        {"foo", coption_no_argument,       'f'},
        {"bar", coption_required_argument, 'b'},
        {"opt", coption_optional_argument, 'o'},
        {0}
    };
    const char* optstr = "xy:z::123";
    printf("program -x -y ARG -z [ARG] -1 -2 -3 --foo --bar ARG --opt [ARG] [ARGUMENTS]\n");
    int c;
    coption opt = coption_init();
    while ((c = coption_get(&opt, argc, argv, optstr, longopts)) != -1) {
        switch (c) {
            case '?': printf("error: unknown option: %s\n", opt.optstr); break;
            case ':': printf("error: missing argument for %s (%c)\n", opt.optstr, opt.opt); break;
            default:  printf("option: %c [%s]\n", opt.opt, opt.arg ? opt.arg : ""); break;
        }
    }
    printf("\nNon-option arguments:");
    for (int i = opt.ind; i < argc; ++i)
        printf(" %s", argv[i]);
    putchar('\n');
    return 0;
}
*/
#ifndef STC_COPTION_H_INCLUDED
#define STC_COPTION_H_INCLUDED

#include <string.h>
#include <stdbool.h>

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
    int ind;            /* equivalent to optind */
    int opt;            /* equivalent to optopt */
    const char *optstr; /* points to the option string */
    const char *arg;    /* equivalent to optarg */
    int _i, _pos, _nargs;
    char _optstr[4];
} coption;

static inline coption coption_init(void) {
     coption opt = {1, 0, NULL, NULL, 1, 0, 0, {'-', '?', '\0'}};
     return opt;
}

/* move argv[j] over n elements to the left */
static void coption_permute_(char *argv[], int j, int n) {
    int k;
    char *p = argv[j];
    for (k = 0; k < n; ++k)
        argv[j - k] = argv[j - k - 1];
    argv[j - k] = p;
}

/* @param opt   output; must be initialized to coption_init() on first call
 * @return      ASCII val for a short option; longopt.val for a long option;
 *              -1 if argv[] is fully processed; '?' for an unknown option or
 *              an ambiguous long option; ':' if an option argument is missing
 */
static int coption_get(coption *opt, int argc, char *argv[],
                       const char *shortopts, const coption_long *longopts) {
    int optc = -1, i0, j, posixly_correct = (shortopts && shortopts[0] == '+');
    if (!posixly_correct) {
        while (opt->_i < argc && (argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0'))
            ++opt->_i, ++opt->_nargs;
    }
    opt->opt = 0, opt->optstr = NULL, opt->arg = NULL, i0 = opt->_i;
    if (opt->_i >= argc || argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0') {
        opt->ind = opt->_i - opt->_nargs;
        return -1;
    }
    if (argv[opt->_i][0] == '-' && argv[opt->_i][1] == '-') { /* "--" or a long option */
        if (argv[opt->_i][2] == '\0') { /* a bare "--" */
            coption_permute_(argv, opt->_i, opt->_nargs);
            ++opt->_i, opt->ind = opt->_i - opt->_nargs;
            return -1;
        }
        optc = '?', opt->_pos = -1;
        if (longopts) { /* parse long options */
            int k, n_exact = 0, n_partial = 0;
            const coption_long *o = 0, *o_exact = 0, *o_partial = 0;
            for (j = 2; argv[opt->_i][j] != '\0' && argv[opt->_i][j] != '='; ++j) {} /* find the end of the option name */
            for (k = 0; longopts[k].name != 0; ++k)
                if (strncmp(&argv[opt->_i][2], longopts[k].name, (size_t)(j - 2)) == 0) {
                    if (longopts[k].name[j - 2] == 0) ++n_exact, o_exact = &longopts[k];
                    else ++n_partial, o_partial = &longopts[k];
                }
            opt->optstr = argv[opt->_i];
            if (n_exact > 1 || (n_exact == 0 && n_partial > 1)) return '?';
            o = n_exact == 1? o_exact : n_partial == 1? o_partial : 0;
            if (o) {
                opt->opt = optc = o->val;
                if (o->type != coption_no_argument) {
                    if (argv[opt->_i][j] == '=')
                        opt->arg = &argv[opt->_i][j + 1];
                    else if (argv[opt->_i][j] == '\0' && opt->_i < argc - 1 && (o->type == coption_required_argument ||
                                                                                argv[opt->_i + 1][0] != '-'))
                        opt->arg = argv[++opt->_i];
                    else if (o->type == coption_required_argument)
                        optc = ':'; /* missing option argument */
                }
            }
        }
    } else if (shortopts) { /* a short option */
        const char *p;
        if (opt->_pos == 0) opt->_pos = 1;
        optc = opt->opt = argv[opt->_i][opt->_pos++];
        opt->_optstr[1] = optc, opt->optstr = opt->_optstr;
        p = strchr(shortopts, optc);
        if (p == 0) {
            optc = '?'; /* unknown option */
        } else if (p[1] == ':') {
            if (argv[opt->_i][opt->_pos] != '\0')
                opt->arg = &argv[opt->_i][opt->_pos];
            else if (opt->_i < argc - 1 && (p[2] != ':' || argv[opt->_i + 1][0] != '-'))
                opt->arg = argv[++opt->_i];
            else if (p[2] != ':')
                optc = ':';
            opt->_pos = -1;
        }
    }
    if (opt->_pos < 0 || argv[opt->_i][opt->_pos] == 0) {
        ++opt->_i, opt->_pos = 0;
        if (opt->_nargs > 0) /* permute */
            for (j = i0; j < opt->_i; ++j)
                coption_permute_(argv, j, opt->_nargs);
    }
    opt->ind = opt->_i - opt->_nargs;
    return optc;
}

#endif // STC_COPTION_H_INCLUDED
