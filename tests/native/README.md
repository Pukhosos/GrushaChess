# Native Tests

Native C tests are run with pytest rather than a separate C testing framework.
This keeps test execution, reporting, and failure handling consistent across
Python and C tests without introducing another build or test orchestration
system.

## Writing Tests

Create a `test_*.c` file in this directory or one of its subdirectories. Define
each test as a `static void` function with a name starting with `test_`,
define `TEST_DISPATCH` macro, and make sure `stdlib.h` is included:

```c
#include <assert.h>
#include <stdlib.h>

#ifndef TEST_DISPATCH
#define TEST_DISPATCH
#endif

static void test_addition() {
    assert(1 + 2 == 3);
}

TEST_DISPATCH
```

The `TEST_DISPATCH` macro is required for the test framework to generate and
inject the `main()` function responsible for dispatching individual tests. It is
supplied automatically at the beginning of the test run.

Each function is discovered automatically and executed as a separate pytest test
case. A nonzero exit code, including one caused by a failed assertion,
compilation error, etc. marks the test as failed.

## Declaring Dependencies

If a test requires additional `.c` source files, declare them at the very
beginning of the test file:

```c
// depends-on: src/example.c

#include <assert.h>
#include <stdlib.h>

#include "include/example.h"

#ifndef TEST_DISPATCH
#define TEST_DISPATCH
#endif

static void test_example() {
    assert(example() == 42);
}

TEST_DISPATCH
```

Paths are relative to the repository root. All dependency declarations must be
consecutive and appear before any other lines.

## Running Tests

Run native tests using pytest:

```sh
pytest tests/native/ -n 8
```

**note:** Test executables are compiled automatically once per pytest session
with AddressSanitizer and UndefinedBehaviorSanitizer enabled. No manual
compilation or cleanup is required.
