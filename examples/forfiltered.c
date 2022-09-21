#include <stdio.h>
#include <stc/csview.h>

#define i_type IVec
#define i_val int
#include <stc/cstack.h>

#define i_type SVec
#define i_val_bind csview
#include <stc/cstack.h>

void demo1(void)
{
    c_auto (IVec, vec) {
        c_forarray (int, v, {0, 1, 2, 3, 4, 5, 80, 6, 7, 80, 8, 9, 80, 10, 11, 12, 13})
            IVec_push(&vec, *v);

        c_forfiltered (i, IVec, vec, *i.ref != 80)
            printf(" %d", *i.ref);            

        puts("");
        c_forfiltered (i, IVec, vec, i.idx >= 3 // drop(3)
                                  && i.idx != 5
                                  && *i.ref != 7,
                                     i.count < 5) // take(5)
            printf(" %d", *i.ref);
        puts("");
    }
}


/* Rust:
fn main() {
    let vector = (1..)            // Infinite range of integers
        .filter(|x| x % 2 != 0)   // Collect odd numbers
        .take(5)                  // Only take five numbers
        .map(|x| x * x)           // Square each number
        .collect::<Vec<usize>>(); // Return as a new Vec<usize>
    println!("{:?}", vector);     // Print result
}
*/

void demo2(void)
{
    c_auto (IVec, vec) {
        int n = 0;
        for (size_t x=1;; ++x)
            if (n == 5) break;
            else if (x & 1) ++n, IVec_push(&vec, x*x);

        c_foreach (i, IVec, vec) printf(" %d", *i.ref);
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
        .take(2)
        .filter(|word| word.contains("i"))
        .collect();
    println!("{:?}", words_containing_i);
}
*/
void demo3(void)
{
    c_auto (SVec, words, words_containing_i) {
        const char* sentence = "This is a sentence in Rust.";
        c_fortoken(w, sentence, " ")
            SVec_push(&words, *w.ref);

        c_forfiltered (w, SVec, words, csview_contains(*w.ref, c_sv("i")),
                                       w.count < 2) // take(2)
            SVec_push(&words_containing_i, *w.ref);

        c_foreach (w, SVec, words_containing_i)
            printf(" %.*s", c_ARGsv(*w.ref));
        puts("");
    }
}


int main(void)
{
    demo1();
    demo2();
    demo3();
}
