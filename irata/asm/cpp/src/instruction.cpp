#include <irata/asm/instruction.hpp>

namespace irata::asm_ {

std::ostream &operator<<(std::ostream &os, AddressingMode addressing_mode) {
  switch (addressing_mode) {
  case AddressingMode::IMMEDIATE:
    os << "IMMEDIATE";
    break;
  case AddressingMode::ABSOLUTE:
    os << "ABSOLUTE";
    break;
  case AddressingMode::NONE:
    os << "NONE";
    break;
  case AddressingMode::ZERO_PAGE:
    os << "ZERO_PAGE";
    break;
  }
  return os;
}

Instruction::Instruction(std::string_view name, Byte opcode,
                         AddressingMode addressing_mode,
                         std::string_view description)
    : name_(name), opcode_(opcode), addressing_mode_(addressing_mode),
      description_(description) {}

const std::string &Instruction::name() const { return name_; }

Byte Instruction::opcode() const { return opcode_; }

AddressingMode Instruction::addressing_mode() const { return addressing_mode_; }

const std::string &Instruction::description() const { return description_; }

bool Instruction::operator==(const Instruction &other) const {
  return std::tie(name_, opcode_, addressing_mode_, description_) ==
         std::tie(other.name_, other.opcode_, other.addressing_mode_,
                  other.description_);
}

bool Instruction::operator!=(const Instruction &other) const {
  return !(*this == other);
}

bool Instruction::operator<(const Instruction &other) const {
  return std::tie(name_, opcode_, addressing_mode_, description_) <
         std::tie(other.name_, other.opcode_, other.addressing_mode_,
                  other.description_);
}

std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
  os << "Instruction(name=" << instruction.name()
     << ", opcode=" << instruction.opcode()
     << ", addressing_mode=" << instruction.addressing_mode()
     << ", description=" << instruction.description() << ")";
  return os;
}

} // namespace irata::asm_
