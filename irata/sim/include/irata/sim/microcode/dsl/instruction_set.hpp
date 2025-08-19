#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/sim/hdl/status_decl.hpp>
#include <map>
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

  // Returns true if the instruction set contains an instruction
  // with the given instruction descriptor.
  bool contains_instruction(const asm_::Instruction &descriptor) const;

  // Adds an instruction to the instruction set with the given instruction
  // descriptor. The new instruction is owned by this instruction set.
  Instruction *create_instruction(const asm_::Instruction &descriptor);

  // Creates an instruction for the declaration loaded from irata-asm with the
  // given name and addressing mode.
  Instruction *create_instruction(std::string_view name,
                                  asm_::AddressingMode mode);

  // Returns the instructions in the instruction set.
  const std::vector<std::unique_ptr<Instruction>> &instructions() const;

  // Returns the instruction with the given instruction descriptor.
  // If statuses are provided, the instruction must have the given statuses.
  // If no instruction is found, throws an exception.
  const Instruction &
  get_instruction(const asm_::Instruction &descriptor,
                  std::map<const hdl::StatusDecl *, bool> statuses = {}) const;

  // Returns the instruction with the given name and addressing mode.
  // If statuses are provided, the instruction must have the given statuses.
  // If no instruction is found, throws an exception.
  const Instruction &
  get_instruction(std::string_view name, asm_::AddressingMode mode,
                  std::map<const hdl::StatusDecl *, bool> statuses = {}) const;

private:
  std::vector<std::unique_ptr<Instruction>> instructions_;
};

} // namespace irata::sim::microcode::dsl
