// depends-on: testdata/native_test/example.c

#include <assert.h>
#include <stdlib.h>

#include "testdata/native_test/example.h"

#ifndef TEST_DISPATCH
#define TEST_DISPATCH
#endif

static void test_example() {
    assert(example() == 42);
}

TEST_DISPATCH
