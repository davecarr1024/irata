#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace irata::asm_ {

enum class AddressingMode {
  IMMEDIATE,
  ABSOLUTE,
};

struct Instruction {
  std::string name;
  uint8_t opcode;
  AddressingMode addressing_mode;
  std::string description;
};

extern const std::vector<Instruction> instruction_table;

} // namespace irata::asm_