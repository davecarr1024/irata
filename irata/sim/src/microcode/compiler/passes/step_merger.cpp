#include <irata/sim/microcode/compiler/passes/step_merger.hpp>

namespace irata::sim::microcode::compiler::passes {

namespace {

hdl::TickPhase min_phase(const ir::Step &step) {
  hdl::TickPhase min_phase = hdl::TickPhase::Clear;
  for (const auto &control : step.controls()) {
    min_phase = std::min(min_phase, control->phase());
  }
  return min_phase;
}

hdl::TickPhase max_phase(const ir::Step &step) {
  hdl::TickPhase max_phase = hdl::TickPhase::Control;
  for (const auto &control : step.controls()) {
    max_phase = std::max(max_phase, control->phase());
  }
  return max_phase;
}

bool can_merge(const ir::Step &lhs, const ir::Step &rhs) {
  return max_phase(lhs) <= min_phase(rhs) && lhs.stage() == rhs.stage();
}

ir::Step merge(const ir::Step &lhs, const ir::Step &rhs) {
  std::set<const hdl::ControlDecl *> controls = lhs.controls();
  controls.insert(rhs.controls().begin(), rhs.controls().end());
  std::set<const hdl::WriteControlDecl *> write_controls = lhs.write_controls();
  write_controls.insert(rhs.write_controls().begin(),
                        rhs.write_controls().end());
  std::set<const hdl::ReadControlDecl *> read_controls = lhs.read_controls();
  read_controls.insert(rhs.read_controls().begin(), rhs.read_controls().end());
  return ir::Step(controls, write_controls, read_controls, lhs.stage());
}

ir::Instruction transform(const ir::Instruction &instruction) {
  if (instruction.steps().size() <= 1) {
    return instruction;
  }
  std::vector<ir::Step> transformed_steps;
  std::vector<ir::Step> pending_steps = instruction.steps();
  ir::Step current_step = std::move(pending_steps.front());
  pending_steps.erase(pending_steps.begin());
  while (!pending_steps.empty()) {
    ir::Step next_step = std::move(pending_steps.front());
    pending_steps.erase(pending_steps.begin());
    if (can_merge(current_step, next_step)) {
      current_step = merge(current_step, next_step);
    } else {
      transformed_steps.push_back(current_step);
      current_step = next_step;
    }
  }
  transformed_steps.push_back(current_step);
  return ir::Instruction(instruction.descriptor(), transformed_steps,
                         instruction.statuses());
}

ir::InstructionSet transform(const ir::InstructionSet &instruction_set) {
  std::set<ir::Instruction> transformed_instructions;
  for (const auto &instruction : instruction_set.instructions()) {
    transformed_instructions.insert(transform(instruction));
  }
  return ir::InstructionSet(transformed_instructions);
}

} // namespace

ir::InstructionSet StepMerger::run(const ir::InstructionSet &instruction_set) {
  return transform(instruction_set);
}

} // namespace irata::sim::microcode::compiler::passes
