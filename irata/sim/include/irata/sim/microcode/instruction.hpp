#pragma once

#include <irata/asm/asm.hpp>
#include <irata/sim/microcode/program.hpp>
#include <vector>

namespace irata::sim::microcode {

// An instruction is all of the microcode programs that can be executed for a
// given assembly instruction.
// The program executed is determined by the program's preconditions and the
// current state of the corresponding status lines.
struct Instruction {
  // The assembly instruction that this microcode instruction corresponds to.
  const asm_::Instruction instruction;

  // The microcode programs that can be executed for this instruction.
  const std::vector<Program> programs;
};

} // namespace irata::sim::microcode
