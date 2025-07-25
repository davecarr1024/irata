#pragma once

#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>

namespace irata::sim::microcode::compiler::passes {

class Pass {
public:
  Pass() = default;
  virtual ~Pass() = default;
  Pass(const Pass &) = delete;
  Pass &operator=(const Pass &) = delete;

  virtual ir::InstructionSet run(const ir::InstructionSet &instruction_set) = 0;
};

} // namespace irata::sim::microcode::compiler::passes
