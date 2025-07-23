#pragma once

#include <irata/asm/asm.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <set>

namespace irata::sim::microcode::dsl {

class Instruction;
class InstructionSet;

class Step {
public:
  // Constructs a new step with the given instruction and instruction set.
  // The step is owned by the instruction set.
  // This should only be called by Instruction::create_step.
  Step(Instruction *instruction);

  Step(const Step &) = delete;
  Step &operator=(const Step &) = delete;

  // Returns the instruction that owns this step.
  Instruction *instruction() const;

  // Returns the instruction set that owns this step.
  InstructionSet *instruction_set() const;

  // Creates a new step that is owned by the same instruction as this step.
  Step *create_step();

  // Creates a new instruction that is owned by the same instruction set as
  // this step.
  Instruction *create_instruction(const asm_::Instruction &instruction);

  // Adds the given control line to the step.
  Step *with_control(const hdl::ControlDecl &control);

  // Returns the control lines for the step.
  const std::set<const hdl::ControlDecl *> &controls() const;

  // Adds the given bus to the step.
  Step *with_bus(const hdl::BusDecl &bus);

  // Returns the buses for the step.
  const std::set<const hdl::BusDecl *> &buses() const;

private:
  Instruction *const instruction_;
  std::set<const hdl::ControlDecl *> controls_;
  std::set<const hdl::BusDecl *> buses_;
};

} // namespace irata::sim::microcode::dsl
