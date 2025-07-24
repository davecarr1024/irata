#pragma once

#include <irata/asm/instruction.hpp>
#include <memory>
#include <vector>

namespace irata::sim::microcode::dsl {

class Instruction;

// InstructionSet is IR container for the instruction set of the CPU.
// This is intended to be used as a fluent declarative API for defining
// the instruction set of the CPU.
// Consumption and validation of the instruction set is done by the
// microcode compiler.
class InstructionSet {
public:
  InstructionSet() = default;

  InstructionSet(const InstructionSet &) = delete;
  InstructionSet &operator=(const InstructionSet &) = delete;

  // Returns the instruction set for the Irata CPU.
  static const InstructionSet &irata();

  // Adds an instruction to the instruction set with the given instruction
  // descriptor. The new instruction is owned by this instruction set.
  Instruction *create_instruction(const asm_::Instruction &descriptor);

  // Returns the instructions in the instruction set.
  const std::vector<std::unique_ptr<Instruction>> &instructions() const;

private:
  std::vector<std::unique_ptr<Instruction>> instructions_;
};

} // namespace irata::sim::microcode::dsl
