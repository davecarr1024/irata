#pragma once

#include <irata/asm/asm.hpp>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace irata::sim::microcode::table {

struct Step {
  const std::set<std::string> controls;

  bool operator==(const Step &other) const;
  bool operator!=(const Step &other) const;
};

std::ostream &operator<<(std::ostream &os, const Step &step);

struct InstructionVariant {
  const std::vector<Step> steps;
  const std::map<std::string, bool> statuses;

  bool operator==(const InstructionVariant &other) const;
  bool operator!=(const InstructionVariant &other) const;
};

std::ostream &operator<<(std::ostream &os, const InstructionVariant &variant);

struct Instruction {
  const asm_::Instruction descriptor;
  const std::vector<InstructionVariant> variants;

  bool operator==(const Instruction &other) const;
  bool operator!=(const Instruction &other) const;
};

std::ostream &operator<<(std::ostream &os, const Instruction &instruction);

struct Table {
  const std::vector<Instruction> instructions;

  bool operator==(const Table &other) const;
  bool operator!=(const Table &other) const;
};

std::ostream &operator<<(std::ostream &os, const Table &table);

} // namespace irata::sim::microcode::table
