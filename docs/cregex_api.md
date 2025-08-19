# STC [cregex](../include/stc/cregex.h): Regular Expressions


## Description

**cregex** is a small and fast unicode UTF8 regular expression parser. It is based on Rob Pike's non-backtracking NFA-based regular expression implementation for the Plan 9 project. See Russ Cox's articles [Implementing Regular Expressions](https://swtch.com/~rsc/regexp/) on why NFA-based regular expression engines often are superiour to the common backtracking implementations (hint: NFAs have no "bad/slow" RE patterns).

The API is simple and includes powerful string pattern matches and replace functions. See example below and in the example folder.

## Constants
- compile-flags
    - CREG_DOTALL     - dot matches newline too: can be set/overridden by (?s) and (?-s) in the RE
    - CREG_ICASE      - ignore case mode: can be set/overridden by (?i) and (?-i) in the RE
- match-flags
    - CREG_FULLMATCH  - like start-, end-of-line anchors were in pattern: "^ ... $"
    - CREG_NEXT       - use end of previous match[0] as start of input
- replace-flags
    - CREG_STRIP      - only keep the replaced matches, strip the rest

## Methods
```c++
                // Compile and return a cregex from a RE pattern. Struct member error holds status.
cregex          cregex_from(const char* pattern);
cregex          cregex_make(const char* pattern, int cflags);

                // Compile and initialize a regex, Returns CREG_OK, or negative error code on failure
int             cregex_compile(cregex *self, const char* pattern, int cflags = CREG_DEFAULT);

                // Destroy
void            cregex_drop(cregex* self);

                // Num. of capture groups in regex, excluding the 0th group which is the full match
int             cregex_captures(const cregex* self);

                // Match RE. Return CREG_OK, CREG_NOMATCH, or CREG_MATCHERROR
int             cregex_match(const cregex* re, const char* str,
                        csview match[] = NULL, int flags = CREG_DEFAULT);
int             cregex_match_sv(const cregex* re, csview sv,
                        csview match[] = NULL, int flags = CREG_DEFAULT);

                // Check if there are matches in input
bool            cregex_is_match(const cregex* re, const char* str);

                // All-in-one single match (compile + match + drop)
int             cregex_match_aio(const char* pattern, const char* str,
                        csview match[] = NULL, int flags = CREG_DEFAULT);
int             cregex_match_aio_sv(const char* pattern, csview sv,
                        csview match[] = NULL, int flags = CREG_DEFAULT);

                // Replace count matched instances, optionally transform the replacement.
cstr            cregex_replace(const cregex* re, const char* str, const char* replace,
                        int count = 0 /* all */,
                        bool(*xform)(int group, csview match, cstr* result) = NULL,
                        int flags = CREG_DEFAULT); /* match|replace flags */
cstr            cregex_replace_sv(const cregex* re, csview sv, const char* replace,
                        int count = 0 /* all */,
                        bool(*xform)(int group, csview match, cstr* result) = NULL,
                        int flags = CREG_DEFAULT);

                // All-in-one replacement (compile + match/replace + drop)
cstr            cregex_replace_aio(const char* pattern, const char* str, const char* replace,
                        int count = 0 /* all */,
                        bool(*xform)(int group, csview match, cstr* result) = NULL,
                        int flags = CREG_DEFAULT); /* compile|match|replace flags */
cstr            cregex_replace_aio_sv(const char* pattern, csview sv, const char* replace,
                        int count = 0 /* all */,
                        bool(*xform)(int group, csview match, cstr* result) = NULL,
                        int flags = CREG_DEFAULT);
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
```c++
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

[ [Run this code](https://godbolt.org/z/cz6qfPG1E) ]
```c++
#include <stc/cregex.h>

int main(void) {
    const char* input = "start date is 2023-03-01, end date 2025-12-31.";
    const char* pattern = "\\b(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)\\b";

    cregex re = cregex_from(pattern);

    // Lets find the first date in the string:
    csview match[4]; // full-match, year, month, date.
    if (cregex_match(&re, input, match) == CREG_OK)
        printf("Found date: " c_svfmt "\n", c_svarg(match[0]));
    else
        printf("Could not match any date\n");

    // Lets change all dates into US date format MM/DD/YYYY, and strip unmatched text.
    cstr us_dates = cregex_replace(&re, input, "$2/$3/$1;", .flags=CREG_STRIP);
    printf("%s\n", cstr_str(&us_dates));

    // Free allocated data
    cstr_drop(&us_dates);
    cregex_drop(&re);
}
```
For a single match you may use the all-in-one function:
```c++
if (cregex_match_aio(pattern, input, match))
    printf("Found date: " c_svfmt "\n", c_svarg(match[0]));
```

### Iterate through regex matches, for (*c_match()*)

To iterate multiple matches in an input string, you may use
```c++
csview match[5] = {0};
while (cregex_match_next(&re, input, match) == CREG_OK)
    for (int k = 1; i <= cregex_captures(&re); ++k)
        printf("submatch %d: " c_svfmt "\n", k, c_svarg(match[k]));
```
There is also a `c_match` macro which simplifies this:
```c++
for (c_match(it, &re, input))
    for (c_range(k, 1, cregex_captures(&re) + 1))
        printf("submatch %d: " c_svfmt "\n", k, c_svarg(it.match[k]));
```

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
| \n \t \r | Newline, tab, carriage return | |
| \d \s \w | Digit, whitespace, alphanumeric character | |
| \D \S \W | Do not match the groups described above | |
| \p{Cc} or \p{Cntrl} | UTF8 control character | * |
| \p{L} or \p{Alpha} | UTF8 letter | * |
| \p{Ll} or \p{Lower} | UTF8 lowercase letter | * |
| \p{Lu} or \p{Upper} | UTF8 uppercase letter | * |
| \p{Lt} | Titlecase letter | * |
| \p{L&} | Cased letter (Ll Lu Lt) | * |
| \p{Lm} | Modifier letter | * |
| \p{Nd} or \p{Digit} | Decimal number | * |
| \p{Nl} | Numeric letter | * |
| \p{No} | Other number | * |
| \p{P}  | Punctuation | * |
| \p{Pc} | Connector punctuation | * |
| \p{Pd} | Dash punctuation | * |
| \p{Pi} | Initial punctuation | * |
| \p{Pf} | Final punctuation | * |
| \p{Ps} | Start/open punctuation | * |
| \p{Pe} | End/close punctuation | * |
| \p{Sc} | Currency symbol | * |
| \p{Sm} | Math symbol | * |
| \p{Sk} | Modifier symbol | * |
| \p{Zl} | Line separator | * |
| \p{Zp} | Paragraph separator | * |
| \p{Zs} | Space separator | * |
| \p{Alpha} | Alphabetic letter (L) | * |
| \p{Lower} | Lowercase letter (Ll) | * |
| \p{Upper} | Uppercase letter (Lu) | * |
| \p{Alnum} | Alpha-numeric letter (L Nl Nd) | * |
| \p{Blank} | Blank (Zs \t) | * |
| \p{Space} | Whitespace: (Zs \t\r\n\v\f) | * |
| \p{Word} | Word character: (Alnum Pc) | * |
| \p{XDigit} | Hex number | * |
| \p{Arabic} | Unicode script | * |
| \p{Bengali} | Unicode script | * |
| \p{Cyrillic} | Unicode script | * |
| \p{Devanagari} | Unicode script | * |
| \p{Georgian} | Unicode script | * |
| \p{Greek} | Unicode script | * |
| \p{Han} | Unicode script | * |
| \p{Hiragana} | Unicode script | * |
| \p{Katakana} | Unicode script | * |
| \p{Latin} | Unicode script | * |
| \p{Thai} | Unicode script | * |
| \P{***Class***} | Do not match the classes described above | * |
| [:alnum:] [:alpha:] [:ascii:] | ASCII character class. NB: only to be used inside [] brackets | * |
| [:blank:] [:cntrl:] [:digit:] | " | * |
| [:graph:] [:lower:] [:print:] | " | * |
| [:punct:] [:space:] [:upper:] | " | * |
| [:xdigit:] [:word:] | " | * |
| [:^***class***:] | Match character not in the ASCII class | * |
| $***n*** | *n*-th replace backreference to capture group. ***n*** in 0-9. $0 is the entire match. | * |
| $***nn;*** | As above, but can handle ***nn*** < CREG_MAX_CAPTURES. | * |

## Limitations

The main goal of **cregex** is to be small and fast with limited but useful unicode support. In order to
reach these goals, **cregex** currently does not support the following features (non-exhaustive list):
- In order to limit table sizes, several general UTF8 character classes are missing, like \p{L}, \p{S},
and specific scripts like \p{Tibetan}. Some/all of these may be added in the future as an
alternative source file with unicode tables to link with. Currently, only code points from from the
Basic Multilingual Plane (BMP) are supported, which contains all the most commonly used unicode characters,
symbols and scripts.
- {n, m} syntax for repeating previous token min-max times.
- Non-capturing groups
- Lookaround and backreferences (cannot be implemented efficiently).

If you need a more feature complete, but bigger library, use [RE2 with C-wrapper](https://github.com/google/re2)
which uses the same type of regex engine as **cregex**, or use [PCRE2](https://www.pcre.org/).
