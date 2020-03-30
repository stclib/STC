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

#ifndef CGETOPT__H__
#define CGETOPT__H__

#include <string.h>

enum {
    c_getopt_none     = 0,
    c_getopt_required = 1,
    c_getopt_optional = 2
};
typedef struct {
	int ind;   /* equivalent to optind */
	int opt;   /* equivalent to optopt */
	char *arg; /* equivalent to optarg */
	int longidx; /* index of a long option; or -1 if short */
	/* private variables not intended for external uses */
	int i, pos, n_args;
} c_getopt_t;

typedef struct {
	char *name;
	int has_arg;
	int val;
} c_longopt_t;

static const c_getopt_t c_getopt_init = {1, 0, 0, -1, 1, 0, 0};

static void _c_getopt_permute(char *argv[], int j, int n) { /* move argv[j] over n elements to the left */
	int k;
	char *p = argv[j];
	for (k = 0; k < n; ++k)
		argv[j - k] = argv[j - k - 1];
	argv[j - k] = p;
}


// Parse command-line options and arguments
//
// This fuction has a similar interface to GNU's getopt_long(). Each call
// parses one option and returns the option name.  st->arg points to the option
// argument if present. The function returns -1 when all command-line arguments
// are parsed. In this case, st->ind is the index of the first non-option
// argument.
//
// @param st        status; shall be initialized to c_getopt_init on the first call
// @param argc      length of argv[]
// @param argv      list of command-line arguments; argv[0] is ignored
// @param permute   non-zero to move options ahead of non-option arguments
// @param optstr    option string
// @param longopts  long options
//
// @return          ASCII val for a short option; longopt.val for a long option;
//                  -1 if argv[] is fully processed; '?' for an unknown option or
//                  an ambiguous long option; ':' if an option argument is missing
//
static int c_getopt(c_getopt_t *st, int argc, char *argv[], int permute, const char *optstr, const c_longopt_t *longopts) {
	int opt = -1, i0, j;
	if (permute) {
		while (st->i < argc && (argv[st->i][0] != '-' || argv[st->i][1] == '\0'))
			++st->i, ++st->n_args;
	}
	st->arg = 0, st->longidx = -1, i0 = st->i;
	if (st->i >= argc || argv[st->i][0] != '-' || argv[st->i][1] == '\0') {
		st->ind = st->i - st->n_args;
		return -1;
	}
	if (argv[st->i][0] == '-' && argv[st->i][1] == '-') { /* "--" or a long option */
		if (argv[st->i][2] == '\0') { /* a bare "--" */
			_c_getopt_permute(argv, st->i, st->n_args);
			++st->i, st->ind = st->i - st->n_args;
			return -1;
		}
		st->opt = 0, opt = '?', st->pos = -1;
		if (longopts) { /* parse long options */
			int k, n_exact = 0, n_partial = 0;
			const c_longopt_t *o = 0, *o_exact = 0, *o_partial = 0;
			for (j = 2; argv[st->i][j] != '\0' && argv[st->i][j] != '='; ++j) {} /* find the end of the option name */
			for (k = 0; longopts[k].name != 0; ++k)
				if (strncmp(&argv[st->i][2], longopts[k].name, j - 2) == 0) {
					if (longopts[k].name[j - 2] == 0) ++n_exact, o_exact = &longopts[k];
					else ++n_partial, o_partial = &longopts[k];
				}
			if (n_exact > 1 || (n_exact == 0 && n_partial > 1)) return '?';
			o = n_exact == 1? o_exact : n_partial == 1? o_partial : 0;
			if (o) {
				st->opt = opt = o->val, st->longidx = o - longopts;
				if (argv[st->i][j] == '=') st->arg = &argv[st->i][j + 1];
				if (o->has_arg == 1 && argv[st->i][j] == '\0') {
					if (st->i < argc - 1) st->arg = argv[++st->i];
					else opt = ':'; /* missing option argument */
				}
			}
		}
	} else { /* a short option */
		const char *p;
		if (st->pos == 0) st->pos = 1;
		opt = st->opt = argv[st->i][st->pos++];
		p = strchr((char*)optstr, opt);
		if (p == 0) {
			opt = '?'; /* unknown option */
		} else if (p[1] == ':') {
			if (argv[st->i][st->pos] == 0) {
				if (st->i < argc - 1) st->arg = argv[++st->i];
				else opt = ':'; /* missing option argument */
			} else st->arg = &argv[st->i][st->pos];
			st->pos = -1;
		}
	}
	if (st->pos < 0 || argv[st->i][st->pos] == 0) {
		++st->i, st->pos = 0;
		if (st->n_args > 0) /* permute */
			for (j = i0; j < st->i; ++j)
				_c_getopt_permute(argv, j, st->n_args);
	}
	st->ind = st->i - st->n_args;
	return opt;
}

/* // demo:
    int main(int argc, char *argv[])
    {
      static c_longopt_t longopts[] = {
        { "foo", c_getopt_none,     301 },
        { "bar", c_getopt_required, 302 },
        { "opt", c_getopt_optional, 303 },
        { NULL, 0, 0 }
      };
      c_getopt_t opt = c_getopt_init;
      int i, c;
      while ((c = c_getopt(&opt, argc, argv, 1, "xy:", longopts)) >= 0) {
        if (c == 'x') printf("-x\n");
        else if (c == 'y') printf("-y %s\n", opt.arg);
        else if (c == 301) printf("--foo\n");
        else if (c == 302) printf("--bar %s\n", opt.arg? opt.arg : "(null)");
        else if (c == 303) printf("--opt %s\n", opt.arg? opt.arg : "(null)");
        else if (c == '?') printf("unknown opt: -%c\n", opt.opt? opt.opt : ':');
        else if (c == ':') printf("missing arg: -%c\n", opt.opt? opt.opt : ':');
      }
      printf("Non-option arguments:");
      for (i = opt.ind; i < argc; ++i)
        printf(" %s", argv[i]);
      putchar('\n');
      return 0;
    }
*/


#endif
