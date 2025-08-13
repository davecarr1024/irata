#include <iostream>
#include <irata/common/strings/strings.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>
#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/ir/step.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

namespace irata::sim::microcode::compiler {

using Instructions = std::set<const ir::Instruction *>;
using Statuses = std::map<const hdl::StatusDecl *, bool>;

std::ostream &operator<<(std::ostream &os, const Statuses &statuses) {
  std::vector<std::string> status_strs;
  for (const auto &[status, value] : statuses) {
    status_strs.push_back(status->path() + "=" + std::to_string(value));
  }
  return os << "{" << common::strings::join(status_strs, ", ") << "}";
}

const auto group_instructions_by_opcode(const Instructions &instructions) {
  std::map<Byte, Instructions> grouped_instructions;
  for (const auto &instruction : instructions) {
    grouped_instructions[instruction->descriptor().opcode()].insert(
        instruction);
  }
  return grouped_instructions;
}

const auto group_instructions_by_statuses(const Instructions &instructions) {
  std::map<Statuses, Instructions> grouped_instructions;
  for (const auto &instruction : instructions) {
    grouped_instructions[instruction->statuses()].insert(instruction);
  }
  return grouped_instructions;
}

const auto all_instructions(const ir::InstructionSet &instruction_set) {
  Instructions instructions;
  for (const auto &instruction : instruction_set.instructions()) {
    instructions.insert(&instruction);
  }
  return instructions;
}

void report(const ir::InstructionSet &instruction_set) {
  std::cout << "  " << instruction_set.instructions().size()
            << " instructions\n";
  const auto instructions = all_instructions(instruction_set);
  std::map<hdl::TickPhase, size_t> phase_counts;
  size_t total_steps = 0, total_controls = 0,
         total_non_step_counter_controls = 0;
  const ::irata::sim::hdl::ControlDecl &increment_step_counter =
      hdl::irata().cpu().controller().step_counter().increment();
  const ::irata::sim::hdl::ControlDecl &reset_step_counter =
      hdl::irata().cpu().controller().step_counter().reset();
  for (const auto &instruction : instructions) {
    for (const auto &step : instruction->steps()) {
      if (step.stage() != 0) {
        total_steps++;
        total_controls += step.controls().size();
        for (const auto &control : step.controls()) {
          phase_counts[control->phase()]++;
          if (control != &increment_step_counter &&
              control != &reset_step_counter) {
            total_non_step_counter_controls++;
          }
        }
      }
    }
  }
  std::cout << "  " << total_steps << " total steps, "
            << total_steps / float(instructions.size())
            << " average steps per instruction\n"
            << "  " << total_controls << " total controls, "
            << total_controls / float(total_steps)
            << " average controls per step\n"
            << "  " << total_non_step_counter_controls
            << " total non-step-counter controls, "
            << total_non_step_counter_controls / float(total_steps)
            << " average non-step-counter controls per step\n";
  for (const auto &[phase, count] : phase_counts) {
    std::cout << "  " << count << " " << phase << " controls\n";
  }
}

void report() {
  const auto &dsl_instruction_set = dsl::InstructionSet::irata();
  const auto uncompiled_instruction_set =
      compiler::ir::InstructionSet(dsl_instruction_set);
  const auto compiled_instruction_set =
      compiler::Compiler::irata().compile_to_ir(dsl_instruction_set);
  std::cout << "Uncompiled instruction set:\n";
  report(uncompiled_instruction_set);
  std::cout << "Compiled instruction set:\n";
  report(compiled_instruction_set);
}

} // namespace irata::sim::microcode::compiler

int main() {
  irata::sim::microcode::compiler::report();
  return 0;
}
