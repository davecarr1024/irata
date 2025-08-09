#include <irata/sim/microcode/compiler/compiler.hpp>
#include <irata/sim/microcode/compiler/passes/bus_validator.hpp>
#include <irata/sim/microcode/compiler/passes/fetch_stage_validator.hpp>
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
  std::set<Byte> opcodes;
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
      opcodes.insert(instruction.descriptor().opcode());
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

  // Phase 1: validators for raw microcode.
  passes.push_back(std::make_unique<passes::FetchStageValidator>());
  passes.push_back(std::make_unique<passes::BusValidator>());
  passes.push_back(std::make_unique<passes::StatusCompletenessValidator>());
  passes.push_back(passes::InstructionCoverageValidator::irata());

  // Phase 2: transformers
  // First step index pass: add step index controls.
  passes.push_back(std::make_unique<passes::StepIndexTransformer>());
  // Immediately validate the step index transformer's output.
  passes.push_back(std::make_unique<passes::StepIndexValidator>());
  // Merge compatible steps.
  passes.push_back(std::make_unique<passes::StepMerger>());
  // Second step index pass: clean up and remove extra merged step index
  // controls.
  passes.push_back(std::make_unique<passes::StepIndexTransformer>());
  // Immediately validate the step index transformer's output.
  passes.push_back(std::make_unique<passes::StepIndexValidator>());

  // Phase 3: validators for compiled microcode
  passes.push_back(std::make_unique<passes::FetchStageValidator>());
  passes.push_back(std::make_unique<passes::BusValidator>());
  passes.push_back(std::make_unique<passes::StatusCompletenessValidator>());
  passes.push_back(passes::InstructionCoverageValidator::irata());

  static const Compiler compiler(std::move(passes));
  return compiler;
}

table::Table Compiler::compile_irata() {
  return irata().compile(dsl::InstructionSet::irata());
}

} // namespace irata::sim::microcode::compiler
