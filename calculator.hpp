#pragma once

// Evaluates one arithmetic operation using finite operands.
//
// Throws std::invalid_argument when an operand is non-finite or the operator
// is unsupported. Throws std::domain_error for division by zero or when the
// calculated result is non-finite.
double calculate(double left_operand, char operation, double right_operand);
