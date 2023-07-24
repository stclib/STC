#include <stdio.h>
#define i_import
#include <stc/cstr.h>
#define i_implement
#include <stc/csview.h>
#include <stc/algorithm.h>

#define i_type IVec
#define i_key int
#include <stc/cstack.h>

// filters and transforms:
#define flt_skipValue(i, x) (*i.ref != (x))
#define flt_isEven(i) ((*i.ref & 1) == 0)
#define flt_isOdd(i) (*i.ref & 1)
#define flt_square(i) (*i.ref * *i.ref)

void demo1(void)
{
    IVec vec = c_init(IVec, {0, 1, 2, 3, 4, 5, 80, 6, 7, 80, 8, 9, 80,
                             10, 11, 12, 13, 14, 15, 80, 16, 17});

    c_forfilter (i, IVec, vec, flt_skipValue(i, 80))
        printf(" %d", *i.ref);
    puts("");

    int sum = 0;
    c_forfilter (i, IVec, vec,
        c_flt_skipwhile(i, *i.ref != 80) &&
        c_flt_skip(i, 1)                 &&
        flt_isEven(i)                    &&
        flt_skipValue(i, 80)             &&
        c_flt_take(i, 5) // short-circuit
    ){
        sum += flt_square(i);
    }

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
    crange r = crange_init(INT64_MAX);
    c_forfilter (x, crange, r,
        c_flt_skipwhile(x, *x.ref != 11) &&
        (*x.ref % 2) != 0                &&
        c_flt_take(x, 5)
    ){
        IVec_push(&vector, (int)(*x.ref * *x.ref));
    }
    c_foreach (x, IVec, vector) printf(" %d", *x.ref);

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
#include <stc/cstack.h>

void demo3(void)
{
    const char* sentence = "This is a sentence in C99.";
    SVec words = {0}; 
    c_fortoken (w, sentence, " ") // split words
        SVec_push(&words, *w.ref);

    SVec words_containing_i = {0};
    c_forfilter (w, SVec, words, 
                    csview_contains(*w.ref, "i"))
        SVec_push(&words_containing_i, *w.ref);

    c_foreach (w, SVec, words_containing_i)
        printf(" %.*s", c_SV(*w.ref));

    puts("");
    c_drop(SVec, &words, &words_containing_i);
}

void demo4(void)
{
    // Keep only uppercase letters and convert them to lowercase:
    csview s = c_sv("ab123cReAghNGnΩoEp"); // Ω = multi-byte
    cstr out = {0};

    c_forfilter (i, csview, s, utf8_isupper(utf8_peek(i.ref))) {
        char chr[4];
        utf8_encode(chr, utf8_tolower(utf8_peek(i.ref)));
        cstr_push(&out, chr);
    }

    printf(" %s\n", cstr_str(&out));
    cstr_drop(&out);
}

void demo5(void)
{
    #define flt_even(i) ((*i.ref & 1) == 0)
    #define flt_mid_decade(i) ((*i.ref % 10) != 0)
    crange R = crange_init(1963, INT32_MAX);

    c_forfilter (i, crange, R,
        c_flt_skip(i,15)                      &&
        c_flt_skipwhile(i, flt_mid_decade(i)) &&
        c_flt_skip(i,30)                      &&
        flt_even(i)                           &&
        c_flt_take(i,5)
    ){
        printf(" %lld", *i.ref);
    }
    puts("");
}

int main(void)
{
    puts("demo1"); demo1();
    puts("demo2"); demo2();
    puts("demo3"); demo3();
    puts("demo4"); demo4();
    puts("demo5"); demo5();
}
