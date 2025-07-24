#include <irata/sim/microcode/compiler/stages/bus_validator.hpp>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::compiler::stages {

namespace {

struct Controls {
  std::set<const hdl::WriteControlDecl *> write_controls;
  std::set<const hdl::ReadControlDecl *> read_controls;
};

std::map<const hdl::BusDecl *, Controls>
group_controls_by_bus(const ir::Step &step) {
  std::map<const hdl::BusDecl *, Controls> result;
  for (const auto &control : step.write_controls()) {
    result[&control->bus()].write_controls.insert(control);
  }
  for (const auto &control : step.read_controls()) {
    result[&control->bus()].read_controls.insert(control);
  }
  return result;
}

void validate(const ir::Step &step) {
  const auto controls_by_bus = group_controls_by_bus(step);
  for (const auto &[bus, controls] : controls_by_bus) {
    if (controls.write_controls.size() == 0) {
      std::ostringstream os;
      os << "Bus " << bus->path() << " is read but not written in step "
         << step;
      throw std::invalid_argument(os.str());
    }
  }
}

void validate(const ir::Instruction &instruction) {
  for (const auto &step : instruction.steps()) {
    validate(step);
  }
}

void validate(const ir::InstructionSet &instruction_set) {
  for (const auto &instruction : instruction_set.instructions()) {
    validate(instruction);
  }
}

} // namespace

ir::InstructionSet
BusValidator::run(const ir::InstructionSet &instruction_set) {
  validate(instruction_set);
  return instruction_set;
}

} // namespace irata::sim::microcode::compiler::stages