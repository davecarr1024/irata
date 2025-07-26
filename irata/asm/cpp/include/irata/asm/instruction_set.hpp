#pragma once

#include <irata/asm/instruction.hpp>
#include <ostream>
#include <set>

namespace irata::asm_ {

class InstructionSet {
public:
  explicit InstructionSet(std::set<Instruction> instructions);

  const std::set<Instruction> &instructions() const;

  bool operator==(const InstructionSet &other) const;
  bool operator!=(const InstructionSet &other) const;
  bool operator<(const InstructionSet &other) const;

  static const InstructionSet &irata();

  const Instruction &get_instruction(std::string_view name,
                                     AddressingMode addressing_mode) const;
  const Instruction &get_instruction(Byte opcode) const;

private:
  std::set<Instruction> instructions_;
};

std::ostream &operator<<(std::ostream &os,
                         const InstructionSet &instruction_set);

} // namespace irata::asm_
