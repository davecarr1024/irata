#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_validator.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::compiler::passes {

namespace {

void validate(const ir::Instruction &instruction) {
  if (instruction.steps().empty()) {
    return;
  }

  const auto step_contains_control = [](const ir::Step &step, const auto &ctl) {
    return step.controls().find(&ctl) != step.controls().end();
  };

  const hdl::ControlDecl &increment_step_index =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ControlDecl &reset_step_index =
      hdl::irata().cpu().controller().step_counter().reset();

  for (int i = 0; i < instruction.steps().size() - 1; ++i) {
    const auto &step = instruction.steps()[i];
    if (!step_contains_control(step, increment_step_index)) {
      std::ostringstream os;
      os << "Step " << i << " of instruction " << instruction << ": " << step
         << " does not increment step index";
      throw std::invalid_argument(os.str());
    }
  }

  const auto &last_step = instruction.steps().back();
  if (!step_contains_control(last_step, reset_step_index)) {
    std::ostringstream os;
    os << "Last step of instruction " << instruction << ": " << last_step
       << " does not reset step index";
    throw std::invalid_argument(os.str());
  }
  if (step_contains_control(last_step, increment_step_index)) {
    std::ostringstream os;
    os << "Last step of instruction " << instruction << ": " << last_step
       << " increments step index";
    throw std::invalid_argument(os.str());
  }
}

void validate(const ir::InstructionSet &instruction_set) {
  for (const auto &instruction : instruction_set.instructions()) {
    validate(instruction);
  }
}

} // namespace

ir::InstructionSet
StepIndexValidator::run(const ir::InstructionSet &instruction_set) {
  validate(instruction_set);
  return instruction_set;
}

} // namespace irata::sim::microcode::compiler::passes
