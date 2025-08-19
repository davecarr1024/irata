#pragma once

#include <cstdint>
#include <irata/common/bytes/byte.hpp>
#include <ostream>
#include <set>
#include <string>

namespace irata::asm_ {

using Byte = common::bytes::Byte;

enum class AddressingMode {
  None,
  Immediate,
  Absolute,
  ZeroPage,
  ZeroPageX,
};

std::ostream &operator<<(std::ostream &os, AddressingMode addressing_mode);

class Instruction {
public:
  Instruction(std::string_view name, Byte opcode,
              AddressingMode addressing_mode, std::string_view description);

  const std::string &name() const;
  Byte opcode() const;
  AddressingMode addressing_mode() const;
  const std::string &description() const;

  bool operator==(const Instruction &other) const;
  bool operator!=(const Instruction &other) const;
  bool operator<(const Instruction &other) const;

  static const std::set<Instruction> &irata();

private:
  std::string name_;
  Byte opcode_;
  AddressingMode addressing_mode_;
  std::string description_;
};

std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

} // namespace irata::asm_
