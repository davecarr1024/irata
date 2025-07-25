#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/compiler/ir/step.hpp>
#include <map>
#include <ostream>
#include <vector>

namespace irata::sim::microcode::compiler::ir {

// Instruction is the intermediate representation of a single instruction
// in the microcode compiler. An instruction is a sequence of steps that
// are executed in order.
// Note that this is flattened and each IR instruciton corresponds to an opcode
// AND an associated set of statuses. The complementary variant is not
// represented here, but must be present in the instruction set.
// Trivially copyable.
class Instruction {
public:
  Instruction(const asm_::Instruction &descriptor, std::vector<Step> steps,
              std::map<const hdl::StatusDecl *, bool> statuses);

  // Creates a new instruction based on the given dsl instruction.
  explicit Instruction(const dsl::Instruction &instruction);

  // Returns the instruction descriptor for this instruction.
  const asm_::Instruction &descriptor() const;

  // Returns the steps in this instruction.
  const std::vector<Step> &steps() const;

  // Returns the statuses that predicate this instruction.
  const std::map<const hdl::StatusDecl *, bool> &statuses() const;

  bool operator==(const Instruction &other) const;
  bool operator!=(const Instruction &other) const;
  bool operator<(const Instruction &other) const;

private:
  asm_::Instruction descriptor_;
  std::vector<Step> steps_;
  std::map<const hdl::StatusDecl *, bool> statuses_;
};

std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

} // namespace irata::sim::microcode::compiler::ir
