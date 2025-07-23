#include <irata/sim/microcode/table/table.hpp>

namespace irata::sim::microcode::table {

bool Step::operator==(const Step &other) const {
  return controls == other.controls;
}

bool Step::operator!=(const Step &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &os, const Step &step) {
  os << "Step(";
  os << "controls={";
  for (const auto &control : step.controls) {
    os << control << " ";
  }
  os << "})";
  return os;
}

bool InstructionVariant::operator==(const InstructionVariant &other) const {
  return steps == other.steps && statuses == other.statuses;
}

bool InstructionVariant::operator!=(const InstructionVariant &other) const {
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const InstructionVariant &variant) {
  os << "InstructionVariant(";
  os << "steps={";
  for (const auto &step : variant.steps) {
    os << step << " ";
  }
  os << "}, statuses={";
  for (const auto &status : variant.statuses) {
    os << status.first << "=" << status.second << " ";
  }
  os << "})";
  return os;
}

bool Instruction::operator==(const Instruction &other) const {
  return descriptor == other.descriptor && variants == other.variants;
}

bool Instruction::operator!=(const Instruction &other) const {
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
  os << "Instruction(";
  os << "descriptor=" << instruction.descriptor;
  os << ", variants={";
  for (const auto &variant : instruction.variants) {
    os << variant << " ";
  }
  os << "})";
  return os;
}

bool Table::operator==(const Table &other) const {
  return instructions == other.instructions;
}

bool Table::operator!=(const Table &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &os, const Table &table) {
  os << "Table(";
  os << "instructions={";
  for (const auto &instruction : table.instructions) {
    os << instruction << " ";
  }
  os << "})";
  return os;
}

} // namespace irata::sim::microcode::table
