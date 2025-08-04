#include <irata/common/strings/strings.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::dsl {

Step::Step(Instruction *instruction, int stage)
    : instruction_(instruction), stage_(stage) {
  if (instruction == nullptr) {
    throw std::invalid_argument("instruction cannot be null");
  }
}

Instruction *Step::instruction() const { return instruction_; }

InstructionSet *Step::instruction_set() const {
  return instruction_->instruction_set();
}

int Step::stage() const { return stage_; }

Step *Step::create_step() { return instruction_->create_step(); }

Instruction *Step::create_instruction(const asm_::Instruction &descriptor) {
  return instruction_->create_instruction(descriptor);
}

namespace {

void validate_hdl_components_in_same_tree(const hdl::ComponentDecl &a,
                                          const hdl::ComponentDecl &b) {
  if (a.root() != b.root()) {
    std::ostringstream os;
    os << "components must be in the same tree: " << a.path() << " and "
       << b.path();
    throw std::invalid_argument(os.str());
  }
}

} // namespace

Step *Step::with_control(const hdl::WriteControlDecl &control) {
  if (!controls_.empty()) {
    validate_hdl_components_in_same_tree(**controls_.begin(), control);
  }
  controls_.insert(&control);
  write_controls_.insert(&control);
  return this;
}

Step *Step::with_control(const hdl::ReadControlDecl &control) {
  if (!controls_.empty()) {
    validate_hdl_components_in_same_tree(**controls_.begin(), control);
  }
  controls_.insert(&control);
  read_controls_.insert(&control);
  return this;
}

Step *Step::with_control(const hdl::ControlDecl &control) {
  if (!controls_.empty()) {
    validate_hdl_components_in_same_tree(**controls_.begin(), control);
  }
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
  std::vector<std::string> control_paths;
  for (const auto *control : step.controls()) {
    control_paths.push_back(control->path());
  }
  os << "Step(controls={" << common::strings::join(control_paths, ", ") << "})";
  return os;
}

} // namespace irata::sim::microcode::dsl
