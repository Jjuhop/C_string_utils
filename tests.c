#include "tests.h"

#define STRUT_IMPLEMENTATION
#include "strut.h"

#include <stdio.h>
#include <string.h>

void perr(const char* emsg) {
    fprintf(stderr, "\tFAIL: %s\n", emsg);
}

#define ASSERT_STACK(s) if(STRUT_IS_HEAP(s)) { fprintf(stderr, "\tFAIL (line %d): String should be in STACK\n", __LINE__); _strut_debug_str(&s); strut_free(&s); return -1; }
#define ASSERT_HEAP(s) if(!STRUT_IS_HEAP(s)) { fprintf(stderr, "\tFAIL (line %d): String should be in HEAP\n", __LINE__); _strut_debug_str(&s); strut_free(&s); return -1; }
#define ASSERT_COND(cond, msg) if(!(cond)) { fprintf(stderr, "\tFAIL (line %d): %s\n", __LINE__, msg); _strut_debug_str(&s); strut_free(&s); return -1; }
#define ASSERT_COND_S(cond, msg, s) if(!(cond)) { fprintf(stderr, "\tFAIL (line %d): %s\n", __LINE__, msg); _strut_debug_str(&s); strut_free(&s); return -1; }

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

    printf("TESTING INTERNAL FUNCTIONS\n");
    for (size_t i = 0; i < (sizeof(internal_test_funcs)/sizeof(internal_test_funcs[0])); i++) {
        err = internal_test_funcs[i]();
        if (err) break;
        printf("\tOK\n");
    }

    printf("TESTING BASIC FUNCTIONS\n");
    for (size_t i = 0; i < (sizeof(test_funcs)/sizeof(test_funcs[0])); i++) {
        err = test_funcs[i]();
        if (err) break;
        printf("\tOK\n");
    }

    return err;
}

int test_init_free()
{
    printf("Testing init & free\n");
    StrutStr s = {0};
    ASSERT_STACK(s);

    strut_init(&s, STRUT_MAX_STACKLEN);
    ASSERT_STACK(s);

    strut_init(&s, STRUT_MAX_STACKLEN + 64);
    ASSERT_HEAP(s);

    strut_free(&s);
    return 0;
}

int test_ensure_additional_ccap()
{
    printf("Testing ensure_additional_ccap\n");
    StrutStr s = {0};
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN);
    ASSERT_STACK(s);

    const char* cstr = "Hello World!";
    strcpy(s.ss, cstr);
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN - strlen(cstr));
    ASSERT_STACK(s);
    
    strut_ensure_additional_ccap(&s, STRUT_MAX_STACKLEN);
    ASSERT_HEAP(s);
    ASSERT_COND(strcmp(s.hs.data, cstr) == 0, "String does not have the correct contents");

    strut_free(&s);
    return 0;
}

int test_appendc()
{
    printf("Testing appendc\n");
    StrutStr s = {0};
    const char* cs1 = "Hello ";
    const char* cs2 = "World!";
    strut_appc(&s, cs1);
    ASSERT_STACK(s);
    ASSERT_COND(strcmp(s.ss, cs1) == 0, "String had incorrect contents, should have \"Hello \"");

    strut_appc(&s, cs2);
    ASSERT_COND(strcmp(strut_get_cstr(&s), "Hello World!") == 0, "String had incorrect contents, should have \"Hello World!\"");

    strut_free(&s);
    return 0;
}

int test_append()
{
    printf("Testing append\n");
    StrutStr s = {0};
    StrutStr s1 = {0};
    strut_appc(&s1, "Hello ");
    StrutStr s2 = {0};
    strut_appc(&s2, "World!");

    strut_app(&s, &s1);
    ASSERT_STACK(s);
    ASSERT_COND(strcmp(strut_get_cstr(&s), "Hello ") == 0, "String had incorrect contents, should have \"Hello \"");

    strut_app(&s, &s2);
    ASSERT_COND(strcmp(strut_get_cstr(&s), "Hello World!") == 0, "String had incorrect contents, should have \"Hello World!\"");

    strut_free(&s);
    strut_free(&s1);
    strut_free(&s2);
    return 0;
}

int test_get_remaining_ccap()
{
    printf("Testing get_remaining_ccap\n");
    StrutStr s = {0};
    const char* cs1 = "Hello ";
    const char* cs2 = "World!";
    strut_appc(&s, cs1);
    ASSERT_STACK(s);
    ASSERT_COND(strut_get_remaining_ccap(&s) == STRUT_MAX_STACKLEN - strut_get_len(&s), "Incorrect ccap left");

    strut_appc(&s, cs2);
    ASSERT_STACK(s);
    ASSERT_COND(strut_get_remaining_ccap(&s) == STRUT_MAX_STACKLEN - strut_get_len(&s), "Incorrect ccap left");

    strut_free(&s);
    return 0;
}

int test_get_len()
{
    printf("Testing get_len\n");
    StrutStr s = {0};
    const char* cs1 = "Hello ";
    const char* cs2 = "World!";
    strut_appc(&s, cs1);
    ASSERT_COND(strut_get_len(&s) == strlen(cs1), "Incorrect length");

    strut_appc(&s, cs2);
    ASSERT_COND(strut_get_len(&s) == strlen(cs1) + strlen(cs2), "Incorrect length");

    strut_free(&s);
    return 0;
}

int test_get_cstr()
{
    printf("Testing get_cstr\n");
    StrutStr s = {0};
    ASSERT_COND(strut_get_cstr(&s) == (const char*)&s, "Zero-init String ptr should be the struct itself");

    strut_appc(&s, "abc");
    ASSERT_STACK(s);
    ASSERT_COND(strut_get_cstr(&s) == (const char*)&s, "Small String ptr should be the struct itself");

    size_t big_len = 2 * STRUT_MAX_STACKLEN;
    char buf[big_len + 1];
    const char* stuff = "asdfghjkl";
    size_t l = strlen(stuff);
    for (size_t i = 0; i < big_len; i++) {
        buf[i] = stuff[i % l];
    }
    buf[big_len] = 0;
    strut_appc(&s, buf);
    ASSERT_HEAP(s);
    ASSERT_COND(strut_get_cstr(&s) == s.hs.data, "Large String ptr should be the heap data ptr");
    ASSERT_COND(strcmp(s.hs.data + 3, buf) == 0, "Heap of string has wrong content");

    strut_free(&s);
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
