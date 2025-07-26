#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <memory>
#include <vector>

namespace irata::sim::microcode::compiler {

class Compiler {
public:
  explicit Compiler(std::vector<std::unique_ptr<passes::Pass>> passes);
  Compiler(const Compiler &) = delete;
  Compiler &operator=(const Compiler &) = delete;

  static const Compiler &irata();

  table::Table compile(const dsl::InstructionSet &instruction_set) const;

  static table::Table compile_irata();

private:
  const std::vector<std::unique_ptr<passes::Pass>> passes_;
};

} // namespace irata::sim::microcode::compiler
