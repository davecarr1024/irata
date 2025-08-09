#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_transformer.hpp>

namespace irata::sim::microcode::compiler::passes {

namespace {

void add_control(std::set<const hdl::ControlDecl *> &controls,
                 const hdl::ControlDecl &control) {
  controls.insert(&control);
}

void remove_control(std::set<const hdl::ControlDecl *> &controls,
                    const hdl::ControlDecl &control) {
  if (const auto it = controls.find(&control); it != controls.end()) {
    controls.erase(it);
  }
}

ir::Step transform_increment_step(const ir::Step &step) {
  const hdl::ProcessControlDecl &increment_control =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_control =
      hdl::irata().cpu().controller().step_counter().reset();
  std::set<const hdl::ControlDecl *> transformed_controls = step.controls();
  add_control(transformed_controls, increment_control);
  remove_control(transformed_controls, reset_control);
  return ir::Step(transformed_controls, step.write_controls(),
                  step.read_controls(), step.stage());
}

ir::Step transform_reset_step(const ir::Step &step) {
  const hdl::ProcessControlDecl &increment_control =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_control =
      hdl::irata().cpu().controller().step_counter().reset();
  std::set<const hdl::ControlDecl *> transformed_controls = step.controls();
  remove_control(transformed_controls, increment_control);
  add_control(transformed_controls, reset_control);
  return ir::Step(transformed_controls, step.write_controls(),
                  step.read_controls(), step.stage());
}

ir::Instruction transform(const ir::Instruction &instruction) {
  std::vector<ir::Step> transformed_steps;
  for (size_t i = 0; i < instruction.steps().size(); ++i) {
    const auto &step = instruction.steps()[i];
    if (i == instruction.steps().size() - 1) {
      transformed_steps.push_back(transform_reset_step(step));
    } else {
      transformed_steps.push_back(transform_increment_step(step));
    }
  }
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

ir::InstructionSet
StepIndexTransformer::run(const ir::InstructionSet &instruction_set) {
  return transform(instruction_set);
}

} // namespace irata::sim::microcode::compiler::passes