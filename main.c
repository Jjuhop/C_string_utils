#include <stdio.h>

#include "strut.h"
#include "adder.h"

int main(int argc, const char* argv[]) {
    StringH s;
    strut_init(&s, 7);
    strut_appc(&s, "Hello");
    strut_appc(&s, " world!\n");
    printf("%s", strut_data(&s));
    strut_free(&s);
    int sum = add_two(3,4);
    printf("3 + 4 = %d\n", sum);
    return 0;
}
//gcc -Wall -Wextra -o main main.c
