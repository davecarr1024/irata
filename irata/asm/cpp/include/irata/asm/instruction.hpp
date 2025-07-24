#pragma once

#include <cstdint>
#include <ostream>
#include <set>
#include <string>

namespace irata::asm_ {

enum class AddressingMode {
  IMMEDIATE,
  ABSOLUTE,
};

std::ostream &operator<<(std::ostream &os, AddressingMode addressing_mode);

class Instruction {
public:
  Instruction(const std::string &name, uint8_t opcode,
              AddressingMode addressing_mode, const std::string &description);

  const std::string &name() const;
  uint8_t opcode() const;
  AddressingMode addressing_mode() const;
  const std::string &description() const;

  bool operator==(const Instruction &other) const;
  bool operator!=(const Instruction &other) const;
  bool operator<(const Instruction &other) const;

  static const std::set<Instruction> &irata();

private:
  std::string name_;
  uint8_t opcode_;
  AddressingMode addressing_mode_;
  std::string description_;
};

std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

} // namespace irata::asm_
