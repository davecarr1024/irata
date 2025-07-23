#pragma once

#include <irata/asm/asm.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace irata::sim::microcode::table {

struct Step {
  const std::set<std::string> controls;
};

struct InstructionVariant {
  const std::vector<Step> steps;
  const std::map<std::string, bool> statuses;
};

struct Instruction {
  const asm_::Instruction descriptor;
  const std::set<InstructionVariant> variants;
};

struct Table {
  const std::set<Instruction> instructions;
};

} // namespace irata::sim::microcode::table
