#pragma once

#include <irata/sim/microcode/instruction.hpp>
#include <vector>

namespace irata::sim::microcode {

// The instruction set is a collection of microcode instruction implementations.
// This is used to build up the instruction memory for the CPU, which is then
// used to control the CPU's execution.
struct InstructionSet {
  const std::vector<Instruction> instructions;
};

} // namespace irata::sim::microcode
