// The MIT License (MIT)
// Copyright (c) 2018 Maksim Andrianov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Program to emulates the phone book.
#include <stc/cstr.h>
#define T StrMap, cstr, cstr, (c_keypro | c_valpro)
#include <stc/hashmap.h>

#define T StrSet, cstr, (c_keypro)
#include <stc/hashset.h>

void print_phone_book(StrMap phone_book)
{
    for (c_each_kv(k, v, StrMap, phone_book))
        printf("%s\t- %s\n", cstr_str(k), cstr_str(v));
}

int main(void)
{
    StrMap phone_book = c_make(StrMap, {
        {"Lilia Friedman", "(892) 670-4739"},
        {"Tariq Beltran", "(489) 600-7575"},
        {"Laiba Juarez", "(303) 885-5692"},
        {"Elliott Mooney", "(945) 616-4482"},
    });

    printf("Phone book:\n");
    print_phone_book(phone_book);

    StrMap_emplace(&phone_book, "Zak Byers", "(551) 396-1880");
    StrMap_emplace(&phone_book, "Zak Byers", "(551) 396-1990");

    printf("\nPhone book after adding Zak Byers:\n");
    print_phone_book(phone_book);

    if (StrMap_contains(&phone_book, "Tariq Beltran"))
        printf("\nTariq Beltran is in phone book\n");

    StrMap_erase(&phone_book, "Tariq Beltran");
    StrMap_erase(&phone_book, "Elliott Mooney");

    printf("\nPhone book after erasing Tariq and Elliott:\n");
    print_phone_book(phone_book);

    StrMap_emplace_or_assign(&phone_book, "Zak Byers", "(555) 396-188");

    printf("\nPhone book after update phone of Zak Byers:\n");
    print_phone_book(phone_book);

    StrMap_drop(&phone_book);
}
