#include "calculator.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

int failures = 0;

void expect_close(const std::string& name, double actual, double expected) {
  if (std::abs(actual - expected) > 1e-12) {
    std::cerr << "FAIL: " << name << " (expected " << expected << ", got "
              << actual << ")\n";
    ++failures;
  }
}

template <typename Exception, typename Function>
void expect_throws(const std::string& name, Function function) {
  try {
    function();
    std::cerr << "FAIL: " << name << " (no exception)\n";
    ++failures;
  } catch (const Exception&) {
  } catch (...) {
    std::cerr << "FAIL: " << name << " (wrong exception type)\n";
    ++failures;
  }
}

}  // namespace

int main() {
  expect_close("adds operands", calculate(2.0, '+', 3.0), 5.0);
  expect_close("subtracts operands", calculate(7.0, '-', 10.0), -3.0);
  expect_close("multiplies fractional operands", calculate(2.5, '*', 4.0),
               10.0);
  expect_close("divides operands", calculate(7.0, '/', 2.0), 3.5);
  expect_close("handles negative operands", calculate(-4.0, '-', -2.0),
               -2.0);

  expect_throws<std::invalid_argument>("rejects unsupported operators", [] {
    (void)calculate(1.0, '%', 2.0);
  });
  expect_throws<std::domain_error>("rejects positive zero division", [] {
    (void)calculate(1.0, '/', 0.0);
  });
  expect_throws<std::domain_error>("rejects negative zero division", [] {
    (void)calculate(1.0, '/', -0.0);
  });
  expect_throws<std::invalid_argument>("rejects non-finite left operand", [] {
    (void)calculate(std::numeric_limits<double>::infinity(), '+', 1.0);
  });
  expect_throws<std::invalid_argument>("rejects non-finite right operand", [] {
    (void)calculate(1.0, '+', std::numeric_limits<double>::quiet_NaN());
  });
  expect_throws<std::domain_error>("rejects non-finite results", [] {
    (void)calculate(std::numeric_limits<double>::max(), '*', 2.0);
  });

  if (failures != 0) {
    std::cerr << failures << " test(s) failed\n";
    return 1;
  }

  std::cout << "All calculator unit tests passed\n";
  return 0;
}
