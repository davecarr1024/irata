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

Instruction *Step::create_instruction(const asm_::Instruction &descriptor) {
  return instruction_->create_instruction(descriptor);
}

Step *Step::with_control(const hdl::WriteControlDecl &control) {
  controls_.insert(&control);
  write_controls_.insert(&control);
  return this;
}

Step *Step::with_control(const hdl::ReadControlDecl &control) {
  controls_.insert(&control);
  read_controls_.insert(&control);
  return this;
}

Step *Step::with_control(const hdl::ControlDecl &control) {
  controls_.insert(&control);
  return this;
}

const std::set<const hdl::ControlDecl *> &Step::controls() const {
  return controls_;
}

const std::set<const hdl::WriteControlDecl *> &Step::write_controls() const {
  return write_controls_;
}

const std::set<const hdl::ReadControlDecl *> &Step::read_controls() const {
  return read_controls_;
}

std::ostream &operator<<(std::ostream &os, const Step &step) {
  os << "Step(controls={";
  for (const auto &control : step.controls()) {
    os << *control << " ";
  }
  os << "})";
  return os;
}

} // namespace irata::sim::microcode::dsl
