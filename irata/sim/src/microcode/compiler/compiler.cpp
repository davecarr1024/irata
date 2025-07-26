#include <irata/sim/microcode/compiler/compiler.hpp>
#include <irata/sim/microcode/compiler/passes/bus_validator.hpp>
#include <irata/sim/microcode/compiler/passes/instruction_coverage_validator.hpp>
#include <irata/sim/microcode/compiler/passes/status_completeness_validator.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_transformer.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_validator.hpp>
#include <irata/sim/microcode/compiler/passes/step_merger.hpp>
#include <stdexcept>

namespace irata::sim::microcode::compiler {

namespace {

ir::InstructionSet convert_input(const dsl::InstructionSet &instruction_set) {
  return ir::InstructionSet(instruction_set);
}

table::Table convert_output(const ir::InstructionSet &instruction_set) {
  std::vector<table::Entry> entries;
  for (const auto &instruction : instruction_set.instructions()) {
    for (size_t step_index = 0; step_index < instruction.steps().size();
         ++step_index) {
      const auto &step = instruction.steps()[step_index];
      entries.push_back({
          .instruction = instruction.descriptor(),
          .step_index = Byte(step_index),
          .statuses = instruction.statuses(),
          .controls = step.controls(),
      });
    }
  }
  return table::Table{std::set<table::Entry>(entries.begin(), entries.end())};
}

} // namespace

Compiler::Compiler(std::vector<std::unique_ptr<passes::Pass>> passes)
    : passes_(std::move(passes)) {
  for (const auto &pass : passes_) {
    if (pass == nullptr) {
      throw std::invalid_argument("Passes must not be null");
    }
  }
}

table::Table
Compiler::compile(const dsl::InstructionSet &instruction_set) const {
  ir::InstructionSet ir = convert_input(instruction_set);
  for (const auto &pass : passes_) {
    ir = pass->run(ir);
  }
  return convert_output(ir);
}

const Compiler &Compiler::irata() {
  std::vector<std::unique_ptr<passes::Pass>> passes;
  passes.push_back(std::make_unique<passes::BusValidator>());
  passes.push_back(std::make_unique<passes::StatusCompletenessValidator>());
  passes.push_back(passes::InstructionCoverageValidator::irata());
  passes.push_back(std::make_unique<passes::StepIndexTransformer>());
  passes.push_back(std::make_unique<passes::StepIndexValidator>());
  passes.push_back(std::make_unique<passes::StepMerger>());
  passes.push_back(std::make_unique<passes::BusValidator>());
  passes.push_back(std::make_unique<passes::StatusCompletenessValidator>());
  passes.push_back(std::make_unique<passes::StepIndexValidator>());
  passes.push_back(passes::InstructionCoverageValidator::irata());
  static const Compiler compiler(std::move(passes));
  return compiler;
}

table::Table Compiler::compile_irata() {
  return irata().compile(dsl::InstructionSet::irata());
}

} // namespace irata::sim::microcode::compiler
