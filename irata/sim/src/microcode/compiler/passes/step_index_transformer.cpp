#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_transformer.hpp>

namespace irata::sim::microcode::compiler::passes {

namespace {

ir::Step transform(const ir::Step &step, bool last) {
  const hdl::ProcessControlDecl &increment_control =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_control =
      hdl::irata().cpu().controller().step_counter().reset();
  const hdl::ProcessControlDecl &control =
      last ? reset_control : increment_control;
  std::set<const hdl::ControlDecl *> transformed_controls = step.controls();
  transformed_controls.insert(&control);
  return ir::Step(transformed_controls, step.write_controls(),
                  step.read_controls());
}

ir::Instruction transform(const ir::Instruction &instruction) {
  std::vector<ir::Step> transformed_steps;
  for (size_t i = 0; i < instruction.steps().size(); ++i) {
    transformed_steps.push_back(
        transform(instruction.steps()[i], i == instruction.steps().size() - 1));
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