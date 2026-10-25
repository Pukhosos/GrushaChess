# Infinite Tests

Infinite are tests that do not finish in a reasonable time.

## Running Infinite Tests

By default, infinite tests run without an iteration limit:

```bash
pytest path/to/infinite.py::test_function
```

Each infinite test defines its own stopping point when the
`--limit-infinite-tests` flag is enabled. To run infinite tests with a limited
number of iterations:

```bash
pytest path/to/infinite.py::test_function --limit-infinite-tests
```

## Design Decision

Each test is responsible for defining and enforcing its own stopping condition.
There is no global iteration limit.
