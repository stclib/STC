# STC [cregex](../include/stc/cregex.h): Regular Expressions


## Description

**cregex** is a small and fast unicode UTF8 regular expression parser. It is based on Rob Pike's non-backtracking NFA-based regular expression implementation for the Plan 9 project. See Russ Cox's articles [Implementing Regular Expressions](https://swtch.com/~rsc/regexp/) on why NFA-based regular expression engines often are superiour to the common backtracking implementations (hint: NFAs have no "bad/slow" RE patterns).

The API is simple and includes powerful string pattern matches and replace functions. See example below and in the example folder.

## Methods

```c
enum {
    // compile-flags
    CREG_DOTALL = 1<<0,    // dot matches newline too: can be set/overridden by (?s) and (?-s) in RE
    CREG_ICASE = 1<<1,     // ignore case mode: can be set/overridden by (?i) and (?-i) in RE

    // match-flags
    CREG_FULLMATCH = 1<<2, // like start-, end-of-line anchors were in pattern: "^ ... $"
    CREG_NEXT = 1<<3,      // use end of previous match[0] as start of input
    CREG_STARTEND = 1<<4,  // use match[0] as start+end of input

    // replace-flags
    CREG_STRIP = 1<<5,     // only keep the replaced matches, strip the rest
};

cregex      cregex_init(void);
cregex      cregex_from(const char* pattern, int cflags = CREG_DEFAULT);
            // return CREG_OK, or negative error code on failure
int         cregex_compile(cregex *self, const char* pattern, int cflags = CREG_DEFAULT);

            // num. of capture groups in regex, excluding the 0th group which is the full match
int         cregex_captures(const cregex* self);

            // return CREG_OK, CREG_NOMATCH, or CREG_MATCHERROR
int         cregex_find(const cregex* re, const char* input, csview match[], int mflags = CREG_DEFAULT);
            // Search inside input string-view only
int         cregex_find_sv(const cregex* re, csview input, csview match[]);
            // All-in-one search (compile + find + drop)
int         cregex_find_pattern(const char* pattern, const char* input, csview match[], int cmflags = CREG_DEFAULT);

            // Check if there are matches in input
bool        cregex_is_match(const cregex* re, const char* input);

            // Replace all matches in input
cstr        cregex_replace(const cregex* re, const char* input, const char* replace, int count = INT_MAX);
            // Replace count matches in input string-view. Optionally transform replacement.
cstr        cregex_replace_sv(const cregex* re, csview input, const char* replace, int count = INT_MAX);
cstr        cregex_replace_sv(const cregex* re, csview input, const char* replace, int count,
                              bool(*transform)(int group, csview match, cstr* result), int rflags);

            // All-in-one replacement (compile + find/replace + drop)
cstr        cregex_replace_pattern(const char* pattern, const char* input, const char* replace, int count = INT_MAX);
cstr        cregex_replace_pattern(const char* pattern, const char* input, const char* replace, int count,
                                   bool(*transform)(int group, csview match, cstr* result), int rflags);
            // destroy
void        cregex_drop(cregex* self);
```

### Error codes
- CREG_OK = 0
- CREG_NOMATCH = -1
- CREG_MATCHERROR = -2
- CREG_OUTOFMEMORY = -3
- CREG_UNMATCHEDLEFTPARENTHESIS = -4
- CREG_UNMATCHEDRIGHTPARENTHESIS = -5
- CREG_TOOMANYSUBEXPRESSIONS = -6
- CREG_TOOMANYCHARACTERCLASSES = -7
- CREG_MALFORMEDCHARACTERCLASS = -8
- CREG_MISSINGOPERAND = -9
- CREG_UNKNOWNOPERATOR = -10
- CREG_OPERANDSTACKOVERFLOW = -11
- CREG_OPERATORSTACKOVERFLOW = -12
- CREG_OPERATORSTACKUNDERFLOW = -13

### Limits
- CREG_MAX_CLASSES
- CREG_MAX_CAPTURES

## Usage

### Compiling a regular expression
```c
cregex re1 = {0};
int result = cregex_compile(&re1, "[0-9]+");
if (result < 0) return result;

const char* url = "(https?://|ftp://|www\\.)([0-9A-Za-z@:%_+~#=-]+\\.)+([a-z][a-z][a-z]?)(/[/0-9A-Za-z\\.@:%_+~#=\\?&-]*)?";
cregex re2 = cregex_from(url);
if (re2.error != CREG_OK)
    return re2.error;
...
cregex_drop(&re2);
cregex_drop(&re1);
```
If an error occurs ```cregex_compile``` returns a negative error code stored in re2.error.

### Getting the first match and making text replacements

[ [Run this code](https://godbolt.org/z/c8as3oxj7) ]
```c
#define i_import // include dependent cstr, utf8 and cregex function definitions.
#include "stc/cregex.h"

int main(void) {
    const char* input = "start date is 2023-03-01, end date 2025-12-31.";
    const char* pattern = "\\b(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)\\b";

    cregex re = cregex_from(pattern);

    // Lets find the first date in the string:
    csview match[4]; // full-match, year, month, date.
    if (cregex_find(&re, input, match) == CREG_OK)
        printf("Found date: %.*s\n", c_SVARG(match[0]));
    else
        printf("Could not find any date\n");

    // Lets change all dates into US date format MM/DD/YYYY:
    cstr us_input = cregex_replace(&re, input, "$2/$3/$1");
    printf("%s\n", cstr_str(&us_input));

    // Free allocated data
    cstr_drop(&us_input);
    cregex_drop(&re);
}
```
For a single match you may use the all-in-one function:
```c
if (cregex_find_pattern(pattern, input, match))
    printf("Found date: %.*s\n", c_SVARG(match[0]));
```

To use: `gcc first_match.c`.
In order to use a callback function in the replace call, see `examples/regex_replace.c`.

### Iterate through regex matches, *c_formatch*

To iterate multiple matches in an input string, you may use
```c
csview match[5] = {0};
while (cregex_find(&re, input, match, CREG_NEXT) == CREG_OK)
    for (int k = 1; i <= cregex_captures(&re); ++k)
        printf("submatch %d: %.*s\n", k, c_SVARG(match[k]));
```
There are also `c_formatch` and `c_formatch_sv` macros which simplifies this:
```c
c_formatch (it, &re, input_str)
    for (int k = 1; i <= cregex_captures(&re); ++k)
        printf("submatch %d: %.*s\n", k, c_SVARG(it.match[k]));
```

## Using cregex in a project

The easiest is to `#define i_import` before `#include "stc/cregex.h"`. Make sure to do that in one translation unit only.

## Regex Cheatsheet

| Metacharacter | Description | STC addition |
|:--:|:--:|:--:|
| ***c*** | Most characters (like c) match themselve literally | |
| \\***c*** | Some characters are used as metacharacters. To use them literally escape them | |
| . | Match any character, except newline unless in (?s) mode | |
| ? | Match the preceding token zero or one time | |
| * | Match the preceding token as often as possible | |
| + | Match the preceding token at least once and as often as possible | |
| \| | Match either the expression before the \| or the expression after it | |
| (***expr***) | Match the expression inside the parentheses. ***This adds a capture group*** | |
| [***chars***] | Match any character inside the brackets. Ranges like a-z may also be used | |
| \[^***chars***\] | Match any character not inside the bracket. | |
| \x{***hex***} | Match UTF8 character/codepoint given as a hex number | * |
| ^ | Start of line anchor | |
| $ | End of line anchor | |
| \A | Start of input anchor | * |
| \Z | End of input anchor | * |
| \z | End of input including optional newline | * |
| \b | UTF8 word boundary anchor | * |
| \B | Not UTF8 word boundary | * |
| \Q | Start literal input mode | * |
| \E | End literal input mode | * |
| (?i) (?-i)  | Ignore case on/off (override CREG_ICASE) | * |
| (?s) (?-s)  | Dot matches newline on/off (override CREG_DOTALL) | * |
| \n \t \r | Match UTF8 newline, tab, carriage return | |
| \d \s \w | Match UTF8 digit, whitespace, alphanumeric character | |
| \D \S \W | Do not match the groups described above | |
| \p{Cc} or \p{Cntrl} | Match UTF8 control char | * |
| \p{Ll} or \p{Lower} | Match UTF8 lowercase letter | * |
| \p{Lu} or \p{Upper} | Match UTF8 uppercase letter | * |
| \p{Lt} | Match UTF8 titlecase letter | * |
| \p{L&} | Match UTF8 cased letter (Ll Lu Lt) | * |
| \p{Nd} or \p{Digit} | Match UTF8 decimal number | * |
| \p{Nl} | Match UTF8 numeric letter | * |
| \p{Pc} | Match UTF8 connector punctuation | * |
| \p{Pd} | Match UTF8 dash punctuation | * |
| \p{Pi} | Match UTF8 initial punctuation | * |
| \p{Pf} | Match UTF8 final punctuation | * |
| \p{Sc} | Match UTF8 currency symbol | * |
| \p{Zl} | Match UTF8 line separator | * |
| \p{Zp} | Match UTF8 paragraph separator | * |
| \p{Zs} | Match UTF8 space separator | * |
| \p{Alpha} | Match UTF8 alphabetic letter (L& Nl) | * |
| \p{Alnum} | Match UTF8 alpha-numeric letter (L& Nl Nd) | * |
| \p{Blank} | Match UTF8 blank (Zs \t) | * |
| \p{Space} | Match UTF8 whitespace: (Zs \t\r\n\v\f] | * |
| \p{Word} | Match UTF8 word character: (Alnum Pc) | * |
| \p{XDigit} | Match hex number | * |
| \p{Arabic} | Language class | * |
| \p{Cyrillic} | Language class | * |
| \p{Devanagari} | Language class | * |
| \p{Greek} | Language class | * |
| \p{Han} | Language class | * |
| \p{Latin} | Language class | * |
| \P{***Class***} | Do not match the classes described above | * |
| [:alnum:] [:alpha:] [:ascii:] | Match ASCII character class. NB: only to be used inside [] brackets | * |
| [:blank:] [:cntrl:] [:digit:] | " | * |
| [:graph:] [:lower:] [:print:] | " | * |
| [:punct:] [:space:] [:upper:] | " | * |
| [:xdigit:] [:word:] | " | * |
| [:^***class***:] | Match character not in the ASCII class | * |
| $***n*** | *n*-th replace backreference to capture group. ***n*** in 0-9. $0 is the entire match. | * |
| $***nn;*** | As above, but can handle ***nn*** < CREG_MAX_CAPTURES. | * |

## Limitations

The main goal of **cregex** is to be small and fast with limited but useful unicode support. In order to reach these goals, **cregex** currently does not support the following features (non-exhaustive list):
- In order to limit table sizes, most general UTF8 character classes are missing, like \p{L}, \p{S}, and most specific scripts like \p{Tibetan}. Some/all of these may be added in the future as an alternative source file with unicode tables to link with. Currently, only characters from from the Basic Multilingual Plane (BMP) are supported, which contains most commonly used characters (i.e. none of the "supplementary planes").
- {n, m} syntax for repeating previous token min-max times.
- Non-capturing groups
- Lookaround and backreferences (cannot be implemented efficiently).

If you need a more feature complete, but bigger library, use [RE2 with C-wrapper](https://github.com/google/re2) which uses the same type of regex engine as **cregex**, or use [PCRE2](https://www.pcre.org/).
