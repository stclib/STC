# STC [cregex](../include/stc/cregex.h): Regular Expressions


## Description

**cregex** is a small and fast unicode UTF8 regular expression parser. It is based on Rob Pike's non-backtracking NFA-based regular expression implementation for the Plan 9 project. See Russ Cox's articles [Implementing Regular Expressions](https://swtch.com/~rsc/regexp/) on why NFA-based regular expression engines often are superiour to the common backtracking implementations (hint: NFAs have no "bad/slow" RE patterns).

The API is simple and includes powerful string pattern matches and replace functions. See example below and in the example folder.

## Methods

```c
enum {
    // compile-flags
    cre_c_dotall = 1<<0,    // dot matches newline too
    cre_c_caseless = 1<<1,  // ignore case
    // match-flags
    cre_m_fullmatch = 1<<2, // like start-, end-of-line anchors were in pattern: "^ ... $"
    cre_m_next = 1<<3,      // use end of previous match[0] as start of input
    cre_m_startend = 1<<4,  // use match[0] as start+end of input
    // replace-flags
    cre_r_strip = 1<<5,     // only keep the replaced matches, strip the rest
};

cregex      cregex_init(void);

cregex      cregex_from(const char* pattern, int cflags);
            // return 1 = success, negative = error.
int         cregex_compile(cregex *self, const char* pattern, int cflags);

            // num. of capture groups in regex. 0 if RE is invalid. First group is the full match.
int         cregex_captures(const cregex* self); 

            // return 1=match, 0=nomatch, -1=error. match array size: at least num groups in RE (1+).
int         cregex_find(const cregex* re, const char* input, csview match[], int mflags);
int         cregex_find_sv(const cregex* re, csview input, csview match[]);
int         cregex_find_pattern(const char* pattern, const char* input, csview match[], int cmflags);

bool        cregex_is_match(const cregex* re, const char* input);

cstr        cregex_replace(const cregex* re, const char* input, const char* replace, unsigned count);
cstr        cregex_replace_sv(const cregex* re, csview input, const char* replace, unsigned count,
                              int rflags, bool(*mfun)(int capgrp, csview match, cstr* mstr));
cstr        cregex_replace_pattern(const char* pattern, const char* input, const char* replace, unsigned count,
                                   int rflags, bool(*mfun)(int capgrp, csview match, cstr* mstr));

void        cregex_drop(cregex* self); // destroy
```

### Error codes
- cre_success = 1
- cre_nomatch = 0
- cre_matcherror = -1
- cre_outofmemory = -2
- cre_unmatchedleftparenthesis = -3
- cre_unmatchedrightparenthesis = -4
- cre_toomanysubexpressions = -5
- cre_toomanycharacterclasses = -6
- cre_malformedcharacterclass = -7
- cre_missingoperand = -8
- cre_unknownoperator = -9
- cre_operandstackoverflow = -10
- cre_operatorstackoverflow = -11
- cre_operatorstackunderflow = -12

### Limits
- cre_MAXCLASSES
- cre_MAXCAPTURES

## Usage

### Compiling a regular expression
```c
cregex re1 = cregex_init();
int result = cregex_compile(&re1, "[0-9]+", 0);
if (result < 0) return result;

const char* url = "(https?://|ftp://|www\\.)([0-9A-Za-z@:%_+~#=-]+\\.)+([a-z][a-z][a-z]?)(/[/0-9A-Za-z\\.@:%_+~#=\\?&-]*)?";
cregex re2 = cregex_from(url, 0);
if (re2.error) return re2.error;
...
cregex_drop(&re2);
cregex_drop(&re1);
```
If an error occurs ```cregex_compile``` returns a negative value, see error codes.

### Getting the first match
```c
#define i_implement
#include <stc/cstr.h>
#include <stc/cregex.h>

int main() {
    const char* input = "start date is 2023-03-01, and end date is 2025-12-31.";
    const char* pattern = "\\b(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)\\b";

    cregex re = cregex_from(pattern, 0);

    // Lets find the first date in the string:
    csview match[4]; // full-match, year, month, date.
    if (cregex_find(&re, input, match, 0) == cre_success)
        printf("Found date: %.*s\n", c_ARGsv(match[0]));
    else
        printf("Could not find any date\n");

    // Lets change all dates into US date format MM/DD/YYYY:
    cstr us_input = cregex_replace(&re, input, "$2/$3/$1");
    printf("US input: %s\n", cstr_str(&us_input));

    // Free allocated data
    cstr_drop(&us_input);
    cregex_drop(&re);
}
```

For a single match you may use the all-in-one function:
```c
if (cregex_find_pattern(pattern, input, match, 0))
    printf("Found date: %.*s\n", c_ARGsv(match[0]));
```

To compile, use: `gcc first_match.c src/cregex.c src/utf8code.c`.
In order to use a callback function in the replace call, see `examples/regex_replace.c`.

### Iterate through regex matches, *c_formatch*

To iterate multiple matches in an input string, you may use
```c
csview match[5] = {0};
while (cregex_find(&re, input, match, cre_m_next) == cre_success)
    c_forrange (k, int, cregex_captures(&re))
        printf("submatch %d: %.*s\n", k, c_ARGsv(match[k]));
```
There is also a safe macro which simplifies this:
```c
c_formatch (it, &re, input)
    c_forrange (k, int, cregex_captures(&re))
        printf("submatch %d: %.*s\n", k, c_ARGsv(it.match[k]));
```

## Using cregex in a project

**cregex** uses the following files: 
- `stc/cregex.h`, `stc/utf8.h`, `stc/csview.h`, `stc/cstr.h`, `stc/ccommon.h`, `stc/forward.h`
- `src/cregex.c`, `src/utf8code.c`.

## Regex Cheatsheet

| Metacharacter | Description | STC addition |
|:--:|:--:|:--:|
| c | Most characters (like c) match themselve literally | |
| \c | Some characters are used as metacharacters. To use them literally escape them | |
| . | Match any character, except newline unless in (?s) mode | |
| ? | Match the preceding token zero or one time | |
| * | Match the preceding token as often as possible | |
| + | Match the preceding token at least once and as often as possible | |
| \| | Match either the expression before the \| or the expression after it | |
| (c) | Match the expression inside the parentheses. This adds a capture group | |
| [c] | Match all characters inside the brackets. Ranges like a-z may also be used | |
| [^c] | Do not match the characters inside the bracket. | |
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
| (?i) (?-i)  | Ignore case on/off (override global) | * |
| (?s) (?-s)  | Dot matches newline on/off (override global) | * |
| \n \t \r | Match UTF8 newline, tab, carriage return | |
| \d \s \w | Match UTF8 digit, whitespace, alphanumeric character | |
| \D \S \W | Do not match the groups described above | |
| \p{Space} or \p{Sz} | Match UTF8 whitespace | * |
| \p{Digit} or \p{Nd} | Match UTF8 numeric | * |
| \p{XDigit} | Match UTF8 hex number | * |
| \p{Lower} or \p{Ll} | Match UTF8 lower case | * |
| \p{Upper} or \p{Lu} | Match UTF8 upper case | * |
| \p{Alpha} or \p{LC} | Match UTF8 cased letter | * |
| \p{Alnum} | Match UTF8 alpha numeric | * |
| \P{***class***} | Do not match the classes described above | * |
| [[:alnum:]] [[:alpha:]] [[:ascii:]] | Match ASCII character class | * |
| [[:blank:]] [[:cntrl:]] [[:digit:]] | Match ASCII character class | * |
| [[:graph:]] [[:lower:]] [[:print:]] | Match ASCII character class | * |
| [[:punct:]] [[:space:]] [[:upper:]] | Match ASCII character class | * |
| [[:xdigit:]] [[:word:]] | Match ASCII character class | * |
| [[:^***class***:]] | Do not match ASCII character class | * |
| $***n*** | *n*-th substitution backreference to capture group. ***n*** in 0-9. $0 is the entire match. | * |
| $***nn***; | As above, but can handle ***nn*** < cre_MAXCAPTURES. | * |

## Limitations

The main goal of **cregex** is to be small and fast with limited but useful unicode support. In order to reach these goals, **cregex** currently does not support the following features (non-exhaustive list):
- In order to limit table sizes, most general UTF8 character classes are missing, like \p{L}, \p{S}, and all specific scripts like \p{Greek} etc. Some/all of these may be added in the future as an alternative source file with unicode tables to link with.
- {n, m} syntax for repeating previous token min-max times.
- Non-capturing groups
- Lookaround and backreferences

If you need a more feature complete, but bigger library, use [RE2 with C-wrapper](https://github.com/google/re2) which uses the same type of regex engine as **cregex**, or use [PCRE2](https://www.pcre.org/).
