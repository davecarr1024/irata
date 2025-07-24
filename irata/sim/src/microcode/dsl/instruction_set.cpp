#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

namespace irata::sim::microcode::dsl {

Instruction *
InstructionSet::create_instruction(const asm_::Instruction &descriptor) {
  return instructions_
      .emplace_back(std::make_unique<Instruction>(descriptor, this))
      .get();
}

const std::vector<std::unique_ptr<Instruction>> &
InstructionSet::instructions() const {
  return instructions_;
}

} // namespace irata::sim::microcode::dsl
