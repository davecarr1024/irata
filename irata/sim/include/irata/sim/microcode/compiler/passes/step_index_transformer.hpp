#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// Adds a step index reset to the last step of each instruction, and adds a
// step index increment to all other steps.
class StepIndexTransformer : public Pass {
public:
  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes
