#include <stdio.h>
#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/views.h>

#define i_type IVec
#define i_val int
#include <stc/cstack.h>

#define i_type SVec
#define i_valclass csview
#include <stc/cstack.h>

// filters and transforms:
#define flt_skipValue(i, x) (*i.ref != (x))
#define flt_isEven(i) ((*i.ref & 1) == 0)
#define flt_isOdd(i) (*i.ref & 1)
#define flt_square(i) (*i.ref * *i.ref)

void demo1(void)
{
    c_auto (IVec, vec) {
        c_forlist (i, int, {0, 1, 2, 3, 4, 5, 80, 6, 7, 80, 8, 9, 80, 10, 11, 12, 13, 14, 15, 80, 16, 17})
            IVec_push(&vec, *i.ref);

        puts("demo1:");
        c_forfilter (i, IVec, vec, flt_skipValue(i, 80))
            printf(" %d", *i.ref);
        puts("");

        int res, sum = 0;
        c_forfilter (i, IVec, vec
                      , c_flt_skipwhile(i, *i.ref != 80)
                     && c_flt_skip(i, 1)
                     && c_flt_skipwhile(i, *i.ref != 80)
                     &&   flt_isEven(i)
                     &&   flt_skipValue(i, 80)
                      , c_flt_take(i, 5) // short-circuit
        ){
            sum += res = flt_square(i);
            printf(" %d", res);
        }
        printf("\n sum: %d\n", sum);
    }
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
    c_auto (IVec, vector) {
        puts("demo2:");

        c_forfilter (x, crange, crange_literal(INT64_MAX)
                      , c_flt_skipwhile(x, *x.ref != 11)
                     && *x.ref % 2 != 0
                      , c_flt_take(x, 5))
            IVec_push(&vector, *x.ref * *x.ref);
        c_foreach (x, IVec, vector) printf(" %d", *x.ref);
        puts("");
    }
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
void demo3(void)
{
    c_auto (SVec, words, words_containing_i) {
        const char* sentence = "This is a sentence in C99.";
        c_fortoken (w, sentence, " ")
            SVec_push(&words, *w.ref);

        c_forfilter (w, SVec, words, 
                     csview_contains(*w.ref, c_sv("i")))
            SVec_push(&words_containing_i, *w.ref);

        puts("demo3:");
        c_foreach (w, SVec, words_containing_i)
            printf(" %.*s", c_ARGsv(*w.ref));
        puts("");
    }
}

void demo4(void)
{
    csview s = c_sv("ab123cReAghNGnΩoEp"); // Ω = multi-byte
    c_auto (cstr, out) {
        c_forfilter (i, csview, s, utf8_isupper(utf8_peek(i.ref))) {
            char chr[4];
            utf8_encode(chr, utf8_tolower(utf8_peek(i.ref)));
            cstr_push(&out, chr);
        }
        puts("demo4:");
        printf(" %s\n", cstr_str(&out));
    }
}

void demo5(void)
{
    #define flt_even(i) ((*i.ref & 1) == 0)
    #define flt_mid_decade(i) ((*i.ref % 10) != 0)
    puts("demo5:");
    crange r1 = crange_make(1963, INT32_MAX);
    c_forfilter (i, crange, r1
                  , c_flt_skip(i,15)
                 && c_flt_skipwhile(i, flt_mid_decade(i))
                 && c_flt_skip(i,30)
                 &&   flt_even(i)
                  , c_flt_take(i,10))
        printf(" %lld", *i.ref);
    puts("");
}

int main(void)
{
    demo1();
    demo2();
    demo3();
    demo4();
    demo5();
}

#include "../src/utf8code.c"
