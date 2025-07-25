#include <irata/common/strings/strings.hpp>
#include <irata/sim/microcode/compiler/passes/status_completeness_validator.hpp>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace irata::sim::microcode::compiler::passes {

namespace {

const auto
group_instructions_by_descriptor(const ir::InstructionSet &instruction_set) {
  std::map<asm_::Instruction, std::set<ir::Instruction>> result;
  for (const auto &instruction : instruction_set.instructions()) {
    result[instruction.descriptor()].insert(instruction);
  }
  return result;
}

void validate(const ir::InstructionSet &instruction_set) {
  const auto instructions_by_descriptor =
      group_instructions_by_descriptor(instruction_set);
  for (const auto &[descriptor, instructions] : instructions_by_descriptor) {
    for (const auto &instruction : instructions) {
      for (const auto &[status, value] : instruction.statuses()) {
        auto opposite_statuses = instruction.statuses();
        opposite_statuses[status] = !value;
        bool found = false;
        for (const auto &other_instruction : instructions) {
          if (other_instruction.statuses() == opposite_statuses) {
            found = true;
            break;
          }
        }
        if (!found) {
          std::ostringstream os;
          std::vector<std::string> status_strings;
          for (const auto &[status, value] : opposite_statuses) {
            status_strings.push_back(status->path() + "=" +
                                     std::to_string(value));
          }
          os << "Instruction " << descriptor << " has statuses "
             << common::strings::join(status_strings, ", ")
             << " but no corresponding instruction with opposite statuses was "
                "found in the instruction set";
          throw std::invalid_argument(os.str());
        }
      }
    }
  }
}

} // namespace

ir::InstructionSet
StatusCompletenessValidator::run(const ir::InstructionSet &instruction_set) {
  validate(instruction_set);
  return instruction_set;
}

} // namespace irata::sim::microcode::compiler::passes