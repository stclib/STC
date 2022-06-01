#include <stdio.h>
// Example of static sized (stack allocated) bitsets

#define i_type Bits
#define i_len 80 // enable fixed bitset on the stack
#include <stc/cbits.h>

int main() 
{
    Bits s1 = Bits_from("1110100110111");
    
    printf("size %" PRIuMAX "\n", Bits_size(&s1));
    char buf[256];
    Bits_to_str(&s1, buf, 0, -1);
    printf("buf: %s: count=%" PRIuMAX "\n", buf, Bits_count(&s1));

    Bits_reset(&s1, 8);
    c_autobuf (str, char, Bits_size(&s1) + 1)
        printf(" s1: %s\n", Bits_to_str(&s1, str, 0, -1));

    Bits s2 = Bits_clone(s1);

    Bits_flip_all(&s2);
    Bits_reset(&s2, 66);
    Bits_reset(&s2, 67);
    printf(" s2: ");
    c_forrange (i, Bits_size(&s2))
        printf("%d", Bits_test(&s2, i));
    puts("");

    printf("xor: ");
    Bits_xor(&s1, &s2);
    c_forrange (i, Bits_size(&s1))
        printf("%d", Bits_test(&s1, i));
    puts("");

    printf("all: ");
    Bits_set_pattern(&s1, 0x3333333333333333);
    c_forrange (i, Bits_size(&s1))
        printf("%d", Bits_test(&s1, i));
    puts("");
}
