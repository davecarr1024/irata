#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <map>
#include <set>
#include <stdexcept>

namespace irata::sim::microcode::compiler {

namespace {

std::map<hdl::TickPhase, std::set<const hdl::ControlDecl *>>
group_step_controls_by_phase(const dsl::Step &step) {
  std::map<hdl::TickPhase, std::set<const hdl::ControlDecl *>>
      controls_by_phase;
  for (const auto &control : step.controls()) {
    controls_by_phase[control->phase()].insert(control);
  }
  return controls_by_phase;
}

std::map<std::pair<hdl::TickPhase, const hdl::BusDecl *>,
         std::set<const hdl::ControlDecl *>>
group_step_controls_by_phase_and_bus(const dsl::Step &step) {
  std::map<std::pair<hdl::TickPhase, const hdl::BusDecl *>,
           std::set<const hdl::ControlDecl *>>
      controls_by_phase_and_bus;
  const auto controls_by_phase = group_step_controls_by_phase(step);
  for (const auto &[phase, controls] : controls_by_phase) {
    for (const auto &control : controls) {
      // wait... controls aren't connected to buses directly, they belong to
      // registers which are connected to buses. So we need to find the register
      // that the control belongs to, and then find the bus that the register is
      // connected to.
    }
  }
}

void validate_step(const dsl::Step &step) {}

} // namespace

void validate(const dsl::InstructionSet &instruction_set) {}

table::Table compile(const dsl::InstructionSet &instruction_set) {
  throw std::runtime_error("not implemented");
}

} // namespace irata::sim::microcode::compiler
