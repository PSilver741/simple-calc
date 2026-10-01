# Tests

`calculator_tests.cpp` directly tests the arithmetic API, including all four
operators and every documented validation branch. Interactive behavior is
tested by `test_runner.sh`, which feeds complete sessions to the executable and
checks stdout, stderr, and process status independently.

Run every unit and interactive check from the repository root:

```bash
./test_runner.sh
```

The runner uses temporary build and capture files and cleans them up on both
success and failure. Do not commit generated executables.
