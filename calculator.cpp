#include "calculator.hpp"

#include <cmath>
#include <stdexcept>

double calculate(double left_operand, char operation, double right_operand) {
  if (!std::isfinite(left_operand) || !std::isfinite(right_operand)) {
    throw std::invalid_argument("operands must be finite");
  }

  double result = 0.0;
  switch (operation) {
    case '+':
      result = left_operand + right_operand;
      break;
    case '-':
      result = left_operand - right_operand;
      break;
    case '*':
      result = left_operand * right_operand;
      break;
    case '/':
      if (right_operand == 0.0) {
        throw std::domain_error("division by zero");
      }
      result = left_operand / right_operand;
      break;
    default:
      throw std::invalid_argument("unsupported operator");
  }

  if (!std::isfinite(result)) {
    throw std::domain_error("result is not finite");
  }

  return result;
}
