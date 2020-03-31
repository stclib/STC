// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Inspired by https://attractivechaos.wordpress.com/2018/08/31/a-survey-of-argument-parsing-libraries-in-c-c
// Fixed major bugs with option arguments (both long and short), and shows a useful demo of the features.
// Has a more consistent API, added arg->err output field, written in C99.

#ifndef COPT__H__
#define COPT__H__

#include <string.h>
#include <stdbool.h>

enum {
    copt_no_argument       = 0,
    copt_required_argument = 1,
    copt_optional_argument = 2
};
typedef struct {
    int ind;   // equivalent to optind
    int opt;   // equivalent to optopt
    char *arg; // equivalent to optarg
    char *err; // points to the faulty option
    int longindex; // idx of long option; or -1 if short
    int _i, _pos, _nargs;
    char _err[4];
} copt_t;

struct copt_option {
    char *name;
    int has_arg;
    int val;
};

static const copt_t copt_init = {1, 0, NULL, NULL, -1, 1, 0, 0, {'-', 0, 0, 0}};

static void _copt_permute(char *argv[], int j, int n) { // move argv[j] over n elements to the left
    int k;
    char *p = argv[j];
    for (k = 0; k < n; ++k)
        argv[j - k] = argv[j - k - 1];
    argv[j - k] = p;
}


// Parse command-line options and arguments
// 
// This fuction has a similar interface to GNU's getopt_long(). Each call
// parses one option and returns the option name.  opt->arg points to the option
// argument if present. The function returns -1 when all command-line arguments
// are parsed. In this case, opt->ind is the index of the first non-option
// argument.
//
// @param opt       output; must be initialized to copt_init on first call
// @param permute   true: move options ahead of non-option arguments
// @return          ASCII val for a short option; longopt.val for a long option;
//                  -1 if argv[] is fully processed; '?' for an unknown option or
//                  an ambiguous long option; ':' if an option argument is missing
//
static int copt_getopt(copt_t *opt, int argc, char *argv[],
                       const char *shortopts, const struct copt_option *longopts,
                       bool permute) {
    int optc = -1, i0, j;
    if (permute) {
        while (opt->_i < argc && (argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0'))
            ++opt->_i, ++opt->_nargs;
    }
    opt->arg = 0, opt->longindex = -1, i0 = opt->_i;
    if (opt->_i >= argc || argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0') {
        opt->ind = opt->_i - opt->_nargs;
        return -1;
    }
    if (argv[opt->_i][0] == '-' && argv[opt->_i][1] == '-') { // "--" or a long option
        if (argv[opt->_i][2] == '\0') { // a bare "--"
            _copt_permute(argv, opt->_i, opt->_nargs);
            ++opt->_i, opt->ind = opt->_i - opt->_nargs;
            return -1;
        }
        opt->opt = 0, optc = '?', opt->_pos = -1;
        if (longopts) { // parse long options
            int k, n_exact = 0, n_partial = 0;
            const struct copt_option *o = 0, *o_exact = 0, *o_partial = 0;
            for (j = 2; argv[opt->_i][j] != '\0' && argv[opt->_i][j] != '='; ++j) {} // find the end of the option name
            for (k = 0; longopts[k].name != 0; ++k)
                if (strncmp(&argv[opt->_i][2], longopts[k].name, j - 2) == 0) {
                    if (longopts[k].name[j - 2] == 0) ++n_exact, o_exact = &longopts[k];
                    else ++n_partial, o_partial = &longopts[k];
                }
            opt->err = argv[opt->_i];
            if (n_exact > 1 || (n_exact == 0 && n_partial > 1)) return '?';
            o = n_exact == 1? o_exact : n_partial == 1? o_partial : 0;
            if (o) {
                opt->opt = optc = o->val, opt->longindex = o - longopts;
                if (o->has_arg != copt_no_argument) {
                    if (argv[opt->_i][j] == '=') 
                        opt->arg = &argv[opt->_i][j + 1];
                    else if (argv[opt->_i][j] == '\0' && opt->_i < argc - 1 && (o->has_arg == copt_required_argument || argv[opt->_i + 1][0] != '-'))
                        opt->arg = argv[++opt->_i];
                    else if (o->has_arg == copt_required_argument)
                        optc = ':'; // missing option argument
                }
            }
        }
    } else { // a short option
        const char *p;
        if (opt->_pos == 0) opt->_pos = 1;
        optc = opt->opt = argv[opt->_i][opt->_pos++];
        opt->_err[1] = optc, opt->err = opt->_err;
        p = strchr((char *) shortopts, optc);
        if (p == 0) {
            optc = '?'; // unknown option
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
        if (opt->_nargs > 0) // permute
            for (j = i0; j < opt->_i; ++j)
                _copt_permute(argv, j, opt->_nargs);
    }
    opt->ind = opt->_i - opt->_nargs;
    return optc;
}

/*  // demo:
    int main(int argc, char *argv[])
    {
        static struct copt_option longopts[] = {
            {"foo", copt_no_argument,       'f'},
            {"bar", copt_required_argument, 'b'},
            {"opt", copt_optional_argument, 'o'},
            {NULL}
        };
        const char* optstr = "xy:z::123";
        printf("program -x -y ARG -z [ARG] -1 -2 -3 --foo --bar ARG --opt [ARG] [ARGUMENTS]\n");
        int c;
        copt_t opt = copt_init;
        while ((c = copt_getopt(&opt, argc, argv, optstr, longopts, true)) != -1) {
            switch (c) {
                case '?': printf("error: unknown option: %s\n", opt.err); break;
                case ':': printf("error: missing argument for %s\n", opt.err); break;
                default:  printf("option: %c [%s]\n", c, opt.arg ? opt.arg : ""); break;
            }
        }
        printf("\nNon-option arguments:");
        for (int i = opt.ind; i < argc; ++i)
            printf(" %s", argv[i]);
        putchar('\n');
        return 0;
    }
*/

#endif
