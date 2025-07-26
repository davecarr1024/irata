#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/controller.hpp>
#include <irata/sim/components/status.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::components::controller {

Controller::Controller(const microcode::table::Table &table,
                       Bus<Byte> &data_bus, std::string_view name,
                       Component *parent)
    : Component(name, parent),
      instruction_memory_(table, "instruction_memory", this),
      opcode_("opcode", &data_bus, this),
      step_counter_("step_counter", &data_bus, this) {}

const InstructionMemory &Controller::instruction_memory() const {
  return instruction_memory_;
}

InstructionMemory &Controller::instruction_memory() {
  return instruction_memory_;
}

const Register &Controller::opcode_register() const { return opcode_; }

Register &Controller::opcode_register() { return opcode_; }

Byte Controller::opcode() const { return opcode_.value(); }

void Controller::set_opcode(Byte opcode) { opcode_.set_value(opcode); }

const Counter &Controller::step_counter_register() const {
  return step_counter_;
}

Counter &Controller::step_counter_register() { return step_counter_; }

Byte Controller::step_counter() const { return step_counter_.value(); }

void Controller::set_step_counter(Byte step_counter) {
  step_counter_.set_value(step_counter);
}

CompleteStatuses Controller::status_values() const {
  std::map<std::string, bool> status_values;
  for (const auto &status : root()->statuses()) {
    status_values[status->name()] = status->value();
  }
  std::map<const hdl::StatusDecl *, bool> complete_status_values;
  for (const auto &status :
       instruction_memory_.encoder().status_encoder().statuses()) {
    if (const auto it = status_values.find(status->name());
        it != status_values.end()) {
      complete_status_values[status] = it->second;
    } else {
      std::ostringstream os;
      os << "status " << status->name()
         << " is required by the controller but was not found";
      throw std::invalid_argument(os.str());
    }
  }
  return CompleteStatuses(instruction_memory_.encoder().status_encoder(),
                          complete_status_values);
}

void Controller::tick_control(Logger &logger) {
  const auto complete_statuses = status_values();
  const auto controls =
      instruction_memory_.read(opcode().unsigned_value(), complete_statuses,
                               step_counter().unsigned_value());
  set_control_values(controls);
}

void Controller::set_control_values(
    const std::set<const hdl::ControlDecl *> &controls) {
  std::map<std::string, Control *> controls_by_path;
  for (const auto &control : root()->controls()) {
    controls_by_path[control->path()] = control;
  }
  for (const auto &control : controls) {
    if (const auto it = controls_by_path.find(control->path());
        it != controls_by_path.end()) {
      it->second->set_value(true);
    } else {
      std::ostringstream os;
      os << "control " << control->path()
         << " is required by the controller but was not found";
      throw std::invalid_argument(os.str());
    }
  }
}

} // namespace irata::sim::components::controller
