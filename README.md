# Simple Calculator CLI

A dependency-free C++17 calculator that performs multiple calculations in one
interactive session. Each calculation is entered on one line as:

```text
<operand> <operator> <operand>
```

Supported operators are `+`, `-`, `*`, and `/`. Enter `q` or `Q` on its own
line to quit.

## Build and Run

The project requires Bash and a C++17-compatible `g++` compiler.

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  main.cpp calculator.cpp -o build/simple-calc
build/simple-calc
```

Example session:

```text
Simple Calculator
Enter: <operand> <operator> <operand>
Operators: + - * /
Enter q or Q to quit.
> 2 + 3
= 5
> 7 / 2
= 3.5
> 1 / 0
Error: division by zero
> 4 * 5
= 20
> q
```

The calculator accepts finite decimal numbers, including signed values,
fractions, and scientific notation. Examples include `2`, `-0.5`, `.25`,
`5.`, `+3`, and `6.02e23`. Multiplication does not need shell quoting because
`*` is entered after the program starts.

Malformed lines, invalid numbers, unsupported operators, division by zero, and
non-finite results print an error and return to the prompt. Reaching end of
input also exits successfully. Passing command-line arguments is invalid and
exits with status `2`.

## Test

Run the complete unit and interactive test suite from the repository root:

```bash
./test_runner.sh
```

The runner compiles with strict warnings, creates test executables and output
captures in a temporary directory, and removes those artifacts when it exits.

## Container

This repository is compatible with [cpp-container](https://github.com/ChicoState/cpp-container). If not already built on your machine, clone and build it.

Run the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container
```

Run the application interactively in a shell:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container sh
```

From that shell, use the build, run, and test commands shown above.

## Structure

- `main.cpp` — interactive input, parsing, formatting, and diagnostics
- `calculator.hpp`, `calculator.cpp` — separately testable arithmetic logic
- `test_runner.sh` — canonical validation workflow
- `specs/` — feature specifications and implementation plans
- `tests/` — unit tests and testing documentation
- `.agents/` — project agent configurations and engineering workflow skills
