#include <irata/sim/microcode/compiler/passes/instruction_coverage_validator.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::compiler::passes {

InstructionCoverageValidator::InstructionCoverageValidator(
    asm_::InstructionSet instruction_set)
    : instruction_set_(std::move(instruction_set)) {}

std::unique_ptr<InstructionCoverageValidator>
InstructionCoverageValidator::irata() {
  return std::make_unique<InstructionCoverageValidator>(
      asm_::InstructionSet::irata());
}

ir::InstructionSet
InstructionCoverageValidator::run(const ir::InstructionSet &instruction_set) {
  for (const auto &instruction : instruction_set_.instructions()) {
    bool found = false;
    for (const auto &ir_instruction : instruction_set.instructions()) {
      if (ir_instruction.descriptor() == instruction) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::ostringstream os;
      os << "Instruction " << instruction
         << " is not implemented in the microcode instruction set";
      throw std::invalid_argument(os.str());
    }
  }
  return instruction_set;
}

} // namespace irata::sim::microcode::compiler::passes
