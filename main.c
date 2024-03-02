#include <stdio.h>

//#define STRUT_ENABLE_SHORTHAND
//#define STRUT_IMPLEMENTATION
//#include "strut.h"
#include "tests.h"

int main(int argc, const char* argv[]) {
    return run_all_tests();
    //StrutStr s = {0};
    //_strut_debug_str(&s);
    //_strut_validate_mem_layout();
    // strut_init(&s, 7);
    // strut_appc(&s, "Hello");
    // strut_appc(&s, " world!\n");
    // printf("%s", strut_data(&s));
    // strut_free(&s);
    // int sum = add_two(3,4);
    // printf("3 + 4 = %d\n", sum);
    // return 0;
}
//gcc -Wall -Wextra -o main main.c
