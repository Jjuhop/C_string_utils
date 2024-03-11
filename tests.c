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
        test_ensure_additional_ccap,
        test_append,
        test_appendc,
        test_get_remaining_ccap,
        test_get_len,
        test_get_cstr,
    };

    int (*internal_test_funcs[])() = {
        test_strlen,
        test_strcpy,
        test_strcmp
    };


    int err = 0;

    printf("TESTING BASIC FUNCTIONS\n");
    for (size_t i = 0; i < (sizeof(test_funcs)/sizeof(test_funcs[0])); i++) {
        err = test_funcs[i]();
        if (err) break;
        printf("\tOK\n");
    }

    printf("TESTING INTERNAL FUNCTIONS\n");
    for (size_t i = 0; i < (sizeof(internal_test_funcs)/sizeof(internal_test_funcs[0])); i++) {
        err = internal_test_funcs[i]();
        if (err) break;
        printf("\tOK\n");
    }

    return err;
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

int test_ensure_additional_ccap()
{
    printf("Testing ensure_additional_ccap\n");
    StrutStr s = {0};
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN);
    if (STRUT_IS_HEAP(s)) {
        perr("Ensure cap that fits in stack moves the string to heap");
        _strut_debug_str(&s);
        return -1;
    }
    const char* cstr = "Hello World!";
    _strut_strcpy(s.ss, cstr);
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN - _strut_strlen(cstr));
    if (STRUT_IS_HEAP(s)) {
        perr("Ensure cap that fits in stack after adding stuff moves the string to heap");
        _strut_debug_str(&s);
        return -1;
    }
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN);
    if (!STRUT_IS_HEAP(s)) {
        perr("Ensure cap does not move the string to heap when needed");
        _strut_debug_str(&s);
        return -1;
    }
    if (_strut_strcmp(strut_get_cstr(&s), cstr) != 0) {
        perr("String does not have the correct contents");
        _strut_debug_str(&s);
        return -1;
    }
    strut_free(&s);
    return 0;
}

int test_append()
{
    printf("[TODO] Testing append\n");
    return 0;
}

int test_appendc()
{
    printf("[TODO] Testing appendc\n");
    return 0;
}

int test_get_remaining_ccap()
{
    printf("[TODO] Testing get_remaining_ccap\n");
    return 0;
}

int test_get_len()
{
    printf("[TODO] Testing get_len\n");
    return 0;
}

int test_get_cstr()
{
    printf("[TODO] Testing get_cstr\n");
    return 0;
}

// Internals

int test_strlen()
{
    printf("Testing _strlen\n");
    const char* s0 = "";
    const char* s1 = "one";
    const char* s2 = "thousand";
    if (_strut_strlen(s0) != 0 || _strut_strlen(s1) != 3 || _strut_strlen(s2) != 8) {
        perr("Incorrect length");
        return -1;
    }
    return 0;
}

int test_strcpy()
{
    printf("Testing _strcpy\n");
    const char* from = "abc";
    char to[4];
    _strut_strcpy(to, from);
    for (size_t i = 0; i < 4; i++) {
        if (to[i] != from[i]) {
            perr("Copy error");
            return -1;
        }
    }
    return 0;
}

int test_strcmp()
{
    printf("Testing _strcmp\n");
    const char* s0 = "";
    const char* s1 = "Hello";
    const char* s2 = "Hello!";
    const char* s3 = "Hello world!";
    if (_strut_strcmp(s0, s1) >= 0) {
        perr("Empty string should come before \"Hello\"");
        return -1;
    }
    if (_strut_strcmp(s1, s2) >= 0) {
        perr("\"Hello\" should come before \"Hello!\"");
        return -1;
    }
    if (_strut_strcmp(s2, s3) <= 0) {
        perr("\"Hello!\" should come after \"Hello world!\"");
        return -1;
    }
    if (_strut_strcmp(s3, s3) != 0) {
        perr("\"Hello world!\" should equal itself");
        return -1;
    }
    return 0;
}
