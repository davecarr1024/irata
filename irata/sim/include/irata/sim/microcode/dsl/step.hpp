#pragma once

#include <irata/asm/instruction.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <ostream>
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
  Instruction *create_instruction(const asm_::Instruction &descriptor);

  // Adds the given control line to the step.
  Step *with_control(const hdl::WriteControlDecl &control);
  Step *with_control(const hdl::ReadControlDecl &control);
  Step *with_control(const hdl::ControlDecl &control);

  // Returns the control lines for the step.
  // This includes all control lines, including read and write control lines.
  const std::set<const hdl::ControlDecl *> &controls() const;

  // Returns the write control lines for the step.
  // This is a subset of the control lines returned by controls().
  const std::set<const hdl::WriteControlDecl *> &write_controls() const;

  // Returns the read control lines for the step.
  // This is a subset of the control lines returned by controls().
  const std::set<const hdl::ReadControlDecl *> &read_controls() const;

private:
  Instruction *const instruction_;
  std::set<const hdl::ControlDecl *> controls_;
  std::set<const hdl::WriteControlDecl *> write_controls_;
  std::set<const hdl::ReadControlDecl *> read_controls_;
};

std::ostream &operator<<(std::ostream &os, const Step &step);

} // namespace irata::sim::microcode::dsl
