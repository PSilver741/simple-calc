#!/usr/bin/env bash

set -euo pipefail

repo_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
test_tmp_dir=$(mktemp -d "${TMPDIR:-/tmp}/simple-calc-tests.XXXXXX")

cleanup() {
  if [[ -n ${test_tmp_dir:-} && -d $test_tmp_dir ]]; then
    rm -r -- "$test_tmp_dir"
  fi
}
trap cleanup EXIT

app="$test_tmp_dir/simple-calc"
unit_tests="$test_tmp_dir/calculator-tests"
stdout_file="$test_tmp_dir/stdout"
stderr_file="$test_tmp_dir/stderr"

compile_flags=(-std=c++17 -Wall -Wextra -Wpedantic -Werror)

g++ "${compile_flags[@]}" -I"$repo_dir" \
  "$repo_dir/tests/calculator_tests.cpp" "$repo_dir/calculator.cpp" \
  -o "$unit_tests"
"$unit_tests"

g++ "${compile_flags[@]}" "$repo_dir/main.cpp" "$repo_dir/calculator.cpp" \
  -o "$app"

fail() {
  printf 'FAIL: %s\n' "$1" >&2
  exit 1
}

assert_contains() {
  local file=$1
  local expected=$2
  local description=$3
  grep -Fq -- "$expected" "$file" || fail "$description"
}

assert_line() {
  local file=$1
  local expected=$2
  local description=$3
  grep -Fxq -- "$expected" "$file" || fail "$description"
}

run_session() {
  local input=$1
  if printf '%s' "$input" | "$app" >"$stdout_file" 2>"$stderr_file"; then
    session_status=0
  else
    session_status=$?
  fi
}

run_session $'2 + 3\n7 - 10\n2.5 * 4\n7 / 2\nq\n'
[[ $session_status -eq 0 ]] || fail "valid session should exit successfully"
assert_contains "$stdout_file" "Enter: <operand> <operator> <operand>" \
  "startup should explain the input format"
assert_contains "$stdout_file" "Enter q or Q to quit." \
  "startup should explain how to quit"
assert_line "$stdout_file" "> = 5" "addition result should be printed"
assert_line "$stdout_file" "> = -3" "subtraction result should be printed"
assert_line "$stdout_file" "> = 10" "multiplication result should be printed"
assert_line "$stdout_file" "> = 3.5" "division result should be printed"
[[ ! -s $stderr_file ]] || fail "valid session should not write to stderr"

run_session $'\n1 +\n1 + 2 extra\nbad + 2\n12abc + 2\n0x10 + 1\n1_000 + 1\nnan + 1\ninf + 1\n1e9999 + 1\n1 ++ 2\n1 % 2\n1 / -0.0\n1e308 * 1e308\n4 * 5\nQ\n'
[[ $session_status -eq 0 ]] || fail "error recovery session should exit successfully"
assert_contains "$stderr_file" "Error: expected <operand> <operator> <operand>" \
  "wrong token counts should be rejected"
assert_contains "$stderr_file" "Error: invalid operand" \
  "malformed operands should be rejected"
assert_contains "$stderr_file" "Error: unsupported operator" \
  "unsupported operators should be rejected"
assert_contains "$stderr_file" "Error: division by zero" \
  "zero division should be rejected"
assert_contains "$stderr_file" "Error: result is not finite" \
  "non-finite results should be rejected"
assert_contains "$stdout_file" "= 20" \
  "a valid calculation after errors should still run"

run_session $'+3 + .25\n5. - 2\n6.02e2 / 2\n0.1 + 0.2\nq\n'
[[ $session_status -eq 0 ]] || fail "documented number forms should succeed"
assert_line "$stdout_file" "> = 3.25" \
  "leading plus and decimal point should work"
assert_line "$stdout_file" "> = 3" "trailing decimal point should work"
assert_line "$stdout_file" "> = 301" "scientific notation should work"
assert_line "$stdout_file" "> = 0.30000000000000004" \
  "results should use round-trip precision"

run_session $'-0.0 * 2\nq\n'
assert_line "$stdout_file" "> = 0" "signed zero should be normalized"

run_session $'  q  \n'
[[ $session_status -eq 0 ]] || fail "trimmed lowercase q should exit successfully"

run_session $''
[[ $session_status -eq 0 ]] || fail "EOF should exit successfully"

if "$app" unexpected >"$stdout_file" 2>"$stderr_file"; then
  invocation_status=0
else
  invocation_status=$?
fi
[[ $invocation_status -eq 2 ]] || fail "arguments should exit with status 2"
[[ ! -s $stdout_file ]] || fail "invalid invocation should not start the REPL"
assert_contains "$stderr_file" "Usage: simple-calc" \
  "invalid invocation should print usage"

printf 'All interactive calculator tests passed\n'
