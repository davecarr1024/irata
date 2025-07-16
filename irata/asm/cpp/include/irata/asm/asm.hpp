#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace irata::asm_ {

enum class AddressingMode {
  IMMEDIATE,
  ABSOLUTE,
};

inline std::ostream &operator<<(std::ostream &os,
                                AddressingMode addressing_mode) {
  switch (addressing_mode) {
  case AddressingMode::IMMEDIATE:
    os << "IMMEDIATE";
    break;
  case AddressingMode::ABSOLUTE:
    os << "ABSOLUTE";
    break;
  }
  return os;
}

struct Instruction {
  std::string name;
  uint8_t opcode;
  AddressingMode addressing_mode;
  std::string description;

  bool operator==(const Instruction &other) const {
    return std::tie(name, opcode, addressing_mode, description) ==
           std::tie(other.name, other.opcode, other.addressing_mode,
                    other.description);
  }

  bool operator!=(const Instruction &other) const { return !(*this == other); }
};

inline std::ostream &operator<<(std::ostream &os,
                                const Instruction &instruction) {
  os << "Instruction{name=" << instruction.name
     << ", opcode=" << instruction.opcode
     << ", addressing_mode=" << instruction.addressing_mode
     << ", description=" << instruction.description << "}";
  return os;
}

extern const std::vector<Instruction> instruction_table;

} // namespace irata::asm_