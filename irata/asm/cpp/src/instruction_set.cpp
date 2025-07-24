#include <algorithm>
#include <cctype>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace irata::asm_ {

InstructionSet::InstructionSet(std::set<Instruction> instructions)
    : instructions_(std::move(instructions)) {}

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
  os << "InstructionSet{";
  for (const auto &instruction : instruction_set.instructions()) {
    os << instruction << ", ";
  }
  os << "}";
  return os;
}

const InstructionSet &InstructionSet::irata() {
  static const InstructionSet instruction_set(Instruction::irata());
  return instruction_set;
}

namespace {

std::string to_upper(const std::string &input) {
  std::string result = input;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

} // namespace

const Instruction &
InstructionSet::get_instruction(const std::string &name,
                                AddressingMode addressing_mode) const {
  const std::string upper_name = to_upper(name);
  for (const auto &instruction : instructions_) {
    if (instruction.name() == upper_name &&
        instruction.addressing_mode() == addressing_mode) {
      return instruction;
    }
  }
  std::ostringstream os;
  os << "Instruction not found with name " << name << " and addressing mode "
     << addressing_mode;
  throw std::invalid_argument(os.str());
}

const Instruction &InstructionSet::get_instruction(uint8_t opcode) const {
  for (const auto &instruction : instructions_) {
    if (instruction.opcode() == opcode) {
      return instruction;
    }
  }
  std::ostringstream os;
  os << "Instruction not found with opcode " << opcode;
  throw std::invalid_argument(os.str());
}

} // namespace irata::asm_
