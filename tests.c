#include "tests.h"

#define STRUT_IMPLEMENTATION
#include "strut.h"

#include <stdio.h>

void perr(const char* emsg) {
    fprintf(stderr, "\tFAIL: %s\n", emsg);
}

int run_all_tests()
{
    int (*test_funcs[])() = {
        test_init_free,
        test_append
    };

    for (size_t i = 0; i < (sizeof(test_funcs)/sizeof(test_funcs[0])); i++) {
        int err = test_funcs[i]();
        if (err) return err;
        printf("\tOK\n");
    }
}

int test_init_free()
{
    printf("Testing init & free\n");
    StrutStr s1 = {0};
    if (STRUT_IS_HEAP(s1)) {
        perr("Zero-init viewed as HEAP string");
        _strut_debug_str(&s1);
        return -1;
    }
    strut_init(&s1, STRUT_MAX_STACKLEN);
    if (STRUT_IS_HEAP(s1)) {
        perr("Small cap init makes it HEAP string");
        _strut_debug_str(&s1);
        return -1;
    }
    strut_init(&s1, 256);
    if (!STRUT_IS_HEAP(s1)) {
        perr("Not on heap despite large init");
        _strut_debug_str(&s1);
        return -1;
    }
    strut_free(&s1);
    return 0;
}

int test_append()
{
    printf("Testing append\n");
    return 0;
}
