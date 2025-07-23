#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

namespace irata::sim::microcode::dsl {

Step::Step(Instruction *instruction) : instruction_(instruction) {
  if (instruction == nullptr) {
    throw std::invalid_argument("instruction cannot be null");
  }
}

Instruction *Step::instruction() const { return instruction_; }

InstructionSet *Step::instruction_set() const {
  return instruction_->instruction_set();
}

Step *Step::create_step() { return instruction_->create_step(); }

Instruction *Step::create_instruction(const asm_::Instruction &instruction) {
  return instruction_->create_instruction(instruction);
}

Step *Step::with_control(const hdl::ControlDecl &control) {
  controls_.insert(&control);
  return this;
}

const std::set<const hdl::ControlDecl *> &Step::controls() const {
  return controls_;
}

Step *Step::with_bus(const hdl::BusDecl &bus) {
  buses_.insert(&bus);
  return this;
}

const std::set<const hdl::BusDecl *> &Step::buses() const { return buses_; }

} // namespace irata::sim::microcode::dsl
