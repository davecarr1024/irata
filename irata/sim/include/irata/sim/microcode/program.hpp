#pragma once

#include <irata/sim/microcode/statement.hpp>
#include <map>
#include <string>
#include <vector>

namespace irata::sim::microcode {

// A microcode program is a collection of microcode statements.
// These are executed sequentially.
struct Program {
  // The statements in this program.
  // These are all executed sequentially.
  const std::vector<Statement> statements;

  // The preconditions of the microcode program.
  // If these are provided, the corresponding status lines must match
  // the specified values for this program to be executed.
  const std::map<std::string, bool> preconditions;
};

} // namespace irata::sim::microcode
