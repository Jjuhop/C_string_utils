#ifndef _TEST_H
#define _TEST_H

int run_all_tests();

int test_init_free();
int test_ensure_additional_ccap();
int test_append();
int test_appendc();
int test_get_remaining_ccap();
int test_get_len();
int test_get_cstr();

// Internals

int test_strlen();
int test_strcpy();
int test_strcmp();

#endif //_TEST_H