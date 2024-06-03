#include <stdio.h>
#define i_import
#include "stc/cstr.h"
#define i_implement
#include "stc/csview.h"
#include "stc/algorithm.h"

#define i_TYPE IVec,int
#include "stc/stack.h"

// filters and transforms:
#define f_skipValue(x) (*value != (x))
#define f_isEven() ((*value & 1) == 0)
#define f_square() (*value * *value)

void demo1(void)
{
    IVec vec = c_init(IVec, {0, 1, 2, 3, 4, 5, 80, 6, 7, 80, 8, 9, 80,
                             10, 11, 12, 13, 14, 15, 80, 16, 17});

    c_filter(IVec, vec, f_skipValue(80) && printf(" %d", (int) *value));
    puts("");

    int sum = 0;
    c_filter(IVec, vec
         , c_flt_skipwhile(*value != 80)
        && c_flt_skip(1)
        && f_isEven()
        && f_skipValue(80)
        && (sum += f_square(), c_flt_take(5))
    );
    printf("\n sum: %d\n", sum);
    IVec_drop(&vec);
}


/* Rust:
fn main() {
    let vector = (1..)            // Infinite range of integers
        .skip_while(|x| *x != 11) // Skip initial numbers unequal 11
        .filter(|x| x % 2 != 0)   // Collect odd numbers
        .take(5)                  // Only take five numbers
        .map(|x| x * x)           // Square each number
        .collect::<Vec<usize>>(); // Return as a new Vec<usize>
    println!("{:?}", vector);     // Print result
}
*/
void demo2(void)
{
    IVec vector = {0};
    c_filter(crange, c_iota(0)           // Infinite range of integers
         , c_flt_skipwhile(*value != 11) // Skip initial numbers unequal 11
        && (*value % 2) != 0             // Collect odd numbers
        && (c_flt_map(*value * *value),  // Square each number
            IVec_push(&vector, *value),  // Populate output IVec
            c_flt_take(5))               // Only take five numbers
    );
    c_foreach (i, IVec, vector)
        printf(" %d", *i.ref);           // Print result
    puts("");
    IVec_drop(&vector);
}

/* Rust:
fn main() {
    let sentence = "This is a sentence in Rust.";
    let words: Vec<&str> = sentence
        .split_whitespace()
        .collect();
    let words_containing_i: Vec<&str> = words
        .into_iter()
        .filter(|word| word.contains("i"))
        .collect();
    println!("{:?}", words_containing_i);
}
*/
#define i_type SVec
#define i_keyclass csview
#include "stc/stack.h"

void demo3(void)
{
    csview sentence = c_sv("This is a sentence in C99.");
    SVec words = {0};
    c_fortoken_sv (i, " ", sentence) // split words
        SVec_push(&words, i.token);

    SVec words_containing_i = {0};
    c_filter(SVec, words
         , csview_contains(*value, "i")
        && SVec_push(&words_containing_i, *value)
    );
    c_foreach (w, SVec, words_containing_i)
        printf(" %.*s", c_SVARG(*w.ref));

    puts("");
    c_drop(SVec, &words, &words_containing_i);
}

void demo4(void)
{
    // Keep only uppercase letters and convert them to lowercase:
    csview s = c_sv("ab123cReAghNGnΩoEp"); // Ω = multi-byte
    cstr out = {0};
    char chr[4];

    c_filter(csview, s
         , utf8_isupper(utf8_peek(value))
        && (utf8_encode(chr, utf8_tolower(utf8_peek(value))),
            cstr_push(&out, chr))
    );
    printf(" %s\n", cstr_str(&out));
    cstr_drop(&out);
}


int main(void)
{
    puts("demo1"); demo1();
    puts("demo2"); demo2();
    puts("demo3"); demo3();
    puts("demo4"); demo4();
}
