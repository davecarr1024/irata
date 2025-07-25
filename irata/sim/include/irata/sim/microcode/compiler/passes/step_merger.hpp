#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// Attempts to merge adjacent steps within each instruction to reduce overall
// step count. Two steps are considered compatible if their control signals are
// consumed in separate, non-overlapping tick phases, allowing them to execute
// in the same clock cycle. This pass improves microcode efficiency without
// altering behavior.
// This is valid because all steps in a given tick phase are assumed to be
// executed in parallel.
class StepMerger : public Pass {
public:
  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes