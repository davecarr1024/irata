#include <irata/sim/microcode/ir/instruction.hpp>
#include <irata/sim/microcode/ir/instruction_set.hpp>
#include <irata/sim/microcode/ir/step.hpp>
#include <stdexcept>

namespace irata::sim::microcode::ir {

Instruction::Instruction(const asm_::Instruction &instruction,
                         InstructionSet *instruction_set)
    : instruction_(instruction), instruction_set_(instruction_set) {
  if (instruction_set == nullptr) {
    throw std::invalid_argument("instruction_set cannot be null");
  }
}

const asm_::Instruction &Instruction::instruction() const {
  return instruction_;
}

InstructionSet *Instruction::instruction_set() const {
  return instruction_set_;
}

Instruction *
Instruction::create_instruction(const asm_::Instruction &instruction) {
  return instruction_set_->create_instruction(instruction);
}

Step *Instruction::create_step() {
  return steps_.emplace_back(std::make_unique<Step>(this)).get();
}

const std::vector<std::unique_ptr<Step>> &Instruction::steps() const {
  return steps_;
}

Instruction *Instruction::with_status(const hdl::StatusDecl &status,
                                      bool value) {
  if (auto it = statuses_.find(&status); it != statuses_.end()) {
    if (const auto &[_, current_value] = *it; current_value != value) {
      throw std::invalid_argument("status conflict replacing " + status.path() +
                                  " that has value " +
                                  std::to_string(current_value) +
                                  " with value " + std::to_string(value));
    }
  }
  statuses_[&status] = value;
  return this;
}

const std::map<const hdl::StatusDecl *, bool> &Instruction::statuses() const {
  return statuses_;
}

} // namespace irata::sim::microcode::ir
