#pragma once

#include "framework/test_framework.h"
#include "core/containers/darray.h"

TEST(darray, create_returns_non_null) {
    darray* d = darray_create(4, sizeof(int));
    ASSERT(d != NULL);
    darray_destroy(d);
    return TEST_PASS;
}

TEST(darray, initial_length_is_zero) {
    darray* d = darray_create(4, sizeof(int));
    ASSERT_EQ(d->length, 0);
    darray_destroy(d);
    return TEST_PASS;
}

TEST(darray, add_increases_length) {
    darray* d = darray_create(4, sizeof(int));
    int val = 42;
    darray_add(d, &val);
    ASSERT_EQ(d->length, 1);
    darray_destroy(d);
    return TEST_PASS;
}

TEST(darray, add_stores_correct_value) {
    darray* d = darray_create(4, sizeof(int));
    int val = 99;
    darray_add(d, &val);
    ASSERT_EQ(((int*)d->data)[0], 99);
    darray_destroy(d);
    return TEST_PASS;
}

TEST(darray, grows_past_initial_capacity) {
    darray* d = darray_create(2, sizeof(int));
    for (int i = 0; i < 8; i++) darray_add(d, &i);
    ASSERT_EQ(d->length, 8);
    ASSERT_EQ(((int*)d->data)[7], 7);
    darray_destroy(d);
    return TEST_PASS;
}

static inline void register_darray_tests(void) {
    REGISTER_TEST(darray, create_returns_non_null);
    REGISTER_TEST(darray, initial_length_is_zero);
    REGISTER_TEST(darray, add_increases_length);
    REGISTER_TEST(darray, add_stores_correct_value);
    REGISTER_TEST(darray, grows_past_initial_capacity);
}