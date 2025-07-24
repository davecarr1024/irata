#pragma once

#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>

namespace irata::sim::microcode::compiler::stages {

class Stage {
public:
  virtual ~Stage() = default;
  Stage(const Stage &) = delete;
  Stage &operator=(const Stage &) = delete;

  virtual ir::InstructionSet run(const ir::InstructionSet &instruction_set) = 0;
};

} // namespace irata::sim::microcode::compiler::stages
