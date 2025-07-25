#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// A pass that validates that each instruction has a complete set of status
// values. This means that if an instruction has a status value, then another
// instruction with the same descriptor but the opposite status value must exist
// in the same instruction set.
class StatusCompletenessValidator : public Pass {
public:
  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes
