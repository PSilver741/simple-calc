#include "calculator.hpp"

#include <charconv>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

bool is_quit_command(const std::string& line) {
  const std::size_t first = line.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return false;
  }

  const std::size_t last = line.find_last_not_of(" \t\r\n");
  return first == last && (line[first] == 'q' || line[first] == 'Q');
}

double parse_operand(const std::string& token) {
  std::string_view input(token);
  if (!input.empty() && input.front() == '+') {
    input.remove_prefix(1);
  }
  if (input.empty()) {
    throw std::invalid_argument("invalid operand");
  }

  double value = 0.0;
  const char* const begin = input.data();
  const char* const end = begin + input.size();
  const auto result =
      std::from_chars(begin, end, value, std::chars_format::general);

  if (result.ec != std::errc{} || result.ptr != end ||
      !std::isfinite(value)) {
    throw std::invalid_argument("invalid operand");
  }

  return value;
}

void evaluate_line(const std::string& line) {
  std::istringstream input(line);
  std::string left_token;
  std::string operator_token;
  std::string right_token;
  std::string extra_token;

  if (!(input >> left_token >> operator_token >> right_token) ||
      (input >> extra_token)) {
    throw std::invalid_argument(
        "expected <operand> <operator> <operand>");
  }
  if (operator_token.size() != 1 ||
      std::string("+-*/").find(operator_token.front()) == std::string::npos) {
    throw std::invalid_argument("unsupported operator");
  }

  double result = calculate(parse_operand(left_token), operator_token.front(),
                            parse_operand(right_token));
  if (result == 0.0) {
    result = 0.0;
  }
  std::cout << "= " << result << '\n';
}

}  // namespace

int main(int argc, char*[]) {
  if (argc != 1) {
    std::cerr << "Usage: simple-calc\n";
    return 2;
  }

  std::cout << "Simple Calculator\n"
            << "Enter: <operand> <operator> <operand>\n"
            << "Operators: + - * /\n"
            << "Enter q or Q to quit.\n"
            << std::defaultfloat
            << std::setprecision(std::numeric_limits<double>::max_digits10);

  std::string line;
  while (true) {
    std::cout << "> " << std::flush;
    if (!std::getline(std::cin, line)) {
      break;
    }
    if (is_quit_command(line)) {
      break;
    }

    try {
      evaluate_line(line);
    } catch (const std::exception& error) {
      std::cerr << "Error: " << error.what() << '\n';
    }
  }

  return 0;
}
