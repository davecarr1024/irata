#include <irata/sim/microcode/ir/instruction.hpp>
#include <irata/sim/microcode/ir/instruction_set.hpp>
#include <irata/sim/microcode/ir/step.hpp>

namespace irata::sim::microcode::ir {

Instruction *
InstructionSet::create_instruction(const asm_::Instruction &instruction) {
  return instructions_
      .emplace_back(std::make_unique<Instruction>(instruction, this))
      .get();
}

const std::vector<std::unique_ptr<Instruction>> &
InstructionSet::instructions() const {
  return instructions_;
}

} // namespace irata::sim::microcode::ir
