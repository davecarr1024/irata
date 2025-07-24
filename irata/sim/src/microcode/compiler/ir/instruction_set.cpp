#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>

namespace irata::sim::microcode::compiler::ir {

InstructionSet::InstructionSet(std::set<Instruction> instructions)
    : instructions_(std::move(instructions)) {}

namespace {

std::set<Instruction> convert_instructions(
    const std::vector<std::unique_ptr<dsl::Instruction>> &instructions) {
  std::set<Instruction> result;
  for (const auto &instruction : instructions) {
    Instruction instruction_(*instruction);
    result.insert(instruction_);
  }
  return result;
}

} // namespace

InstructionSet::InstructionSet(const dsl::InstructionSet &instruction_set)
    : InstructionSet(convert_instructions(instruction_set.instructions())) {}

const std::set<Instruction> &InstructionSet::instructions() const {
  return instructions_;
}

bool InstructionSet::operator==(const InstructionSet &other) const {
  return instructions_ == other.instructions_;
}

bool InstructionSet::operator!=(const InstructionSet &other) const {
  return !(*this == other);
}

bool InstructionSet::operator<(const InstructionSet &other) const {
  return instructions_ < other.instructions_;
}

std::ostream &operator<<(std::ostream &os,
                         const InstructionSet &instruction_set) {
  os << "InstructionSet(";
  for (const auto &instruction : instruction_set.instructions()) {
    os << instruction << ", ";
  }
  os << ")";
  return os;
}

} // namespace irata::sim::microcode::compiler::ir
