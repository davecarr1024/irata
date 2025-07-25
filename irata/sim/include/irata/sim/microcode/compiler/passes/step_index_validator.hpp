#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// Verifies that the last step in each instruction resets the step index
// counter, and all other steps increment it.
class StepIndexValidator : public Pass {
public:
  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes
