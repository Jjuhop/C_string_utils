#include <stdio.h>

//#define STRUT_ENABLE_SHORTHAND
#include "strut.h"
#include "adder.h"

int main(int argc, const char* argv[]) {
    StrutStr s = {0};
    //s.ss.data[6] = 1;
    s.hs.len = 23412;
    char c = 'a';
    s.hs.data = &c;
    printf("First field (len) %lx\nSecond field (cap) %lx\nThird field (ptr) %lx\n", (unsigned long)s.hs.len, s.hs.cap, s.hs.data);
    printf("Is stack: %s\n", (STRUT_IS_HEAP(s) ? "true" : "false"));
    printf("Sizes:\n\theap string: %zu,\n\tstack string: %zu\n\tcombined: %zu\n", sizeof(struct strut_heap_string), sizeof(s.ss), sizeof(StrutStr));
    _strut_debug_str(&s);
    //STRUT_ASSERT(0, "Assertti\n");
    STRUT_PRINT("Testiprintti\n");
    char* arr = STRUT_MALLOC(10);
    if (!arr) {
      STRUT_EPRINT("Malloc fail\n");
      return 1;
    }
    arr[0] = 'a';
    arr[1] = 'b';
    _strut_validate_mem_layout();
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
