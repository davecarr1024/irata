#include <irata/common/strings/strings.hpp>
#include <irata/sim/microcode/compiler/passes/fetch_stage_validator.hpp>

namespace irata::sim::microcode::compiler::passes {

namespace {

std::vector<ir::Step> fetch_stage(const ir::Instruction &instruction) {
  std::vector<ir::Step> steps;
  for (const auto &step : instruction.steps()) {
    if (step.stage() == 0) {
      steps.push_back(step);
    }
  }
  return steps;
}

std::ostream &operator<<(std::ostream &os, const std::vector<ir::Step> &steps) {
  std::vector<std::string> step_strings;
  for (const auto &step : steps) {
    std::ostringstream os;
    os << step;
    step_strings.push_back(os.str());
  }
  return os << "[" << common::strings::join(step_strings, ", ") << "]";
}

void validate(const ir::Instruction &instruction,
              const std::vector<ir::Step> &reference_fetch_stage) {
  const auto instruction_fetch_stage = fetch_stage(instruction);
  if (instruction_fetch_stage != reference_fetch_stage) {
    std::ostringstream os;
    os << "Instruction " << instruction.descriptor().name()
       << " has invalid fetch stage: expected " << reference_fetch_stage
       << " but got " << instruction_fetch_stage;
    throw std::invalid_argument(os.str());
  }
}

void validate(const ir::InstructionSet &instruction_set) {
  if (instruction_set.instructions().empty()) {
    return;
  }
  const auto reference_fetch_stage =
      fetch_stage(*instruction_set.instructions().begin());
  for (const auto &instruction : instruction_set.instructions()) {
    validate(instruction, reference_fetch_stage);
  }
}

} // namespace

ir::InstructionSet
FetchStageValidator::run(const ir::InstructionSet &instruction_set) {
  validate(instruction_set);
  return instruction_set;
}

} // namespace irata::sim::microcode::compiler::passes
