#include <irata/sim/components/controller/controller.hpp>

namespace irata::sim::components::controller {

Controller::Controller(const microcode::InstructionSet &instruction_set,
                       Bus<Byte> &bus, std::string_view name, Component *parent,
                       std::string_view path_prefix)
    : Component(name, parent),
      instruction_memory_(instruction_set, "instruction_memory", this),
      path_prefix_(path_prefix), opcode_("opcode", bus, this),
      step_index_("step_index", bus, this) {}

std::string Controller::prefix_path(std::string_view path) const {
  return path_prefix_ + std::string(path);
}

std::map<std::string, const Status *> Controller::statuses_by_path() const {
  std::map<std::string, const Status *> result;
  for (const auto *status : root()->statuses()) {
    result[status->path()] = status;
  }
  return result;
}

std::map<std::string, bool> Controller::status_values() const {
  // Get the statuses in the instruction memory and match them to the statuses
  // in the component tree.
  const auto statuses = statuses_by_path();
  std::map<std::string, bool> result;
  for (const auto &status : instruction_memory_.statuses()) {
    const auto status_path = prefix_path(status);
    if (const auto it = statuses.find(status_path); it != statuses.end()) {
      result[status] = it->second->value();
    } else {
      throw std::runtime_error("status not found: " + status_path);
    }
  }
  return result;
}

std::map<std::string, Control *> Controller::controls_by_path() {
  std::map<std::string, Control *> result;
  for (auto *control : root()->controls()) {
    result[control->path()] = control;
  }
  return result;
}

void Controller::set_controls(std::set<std::string> unprefixed_controls) {
  // Get the controls in the instruction memory and match them to the
  // controls in the component tree.
  const auto controls_by_path = this->controls_by_path();
  for (const auto &unprefixed_control : unprefixed_controls) {
    const auto control_path = prefix_path(unprefixed_control);
    if (const auto it = controls_by_path.find(control_path);
        it != controls_by_path.end()) {
      it->second->set_value(true);
    } else {
      throw std::runtime_error("control not found: " + control_path);
    }
  }
}

Byte Controller::opcode() const { return opcode_.value(); }

void Controller::set_opcode(Byte opcode) { opcode_.set_value(opcode); }

Byte Controller::step_index() const { return step_index_.value(); }

void Controller::set_step_index(Byte step_index) {
  step_index_.set_value(step_index);
}

void Controller::tick_control(Logger &logger) {
  const auto opcode = this->opcode().unsigned_value();
  const auto step_index = this->step_index().unsigned_value();
  const auto status_values = this->status_values();
  const auto controls =
      instruction_memory_.get_controls(opcode, step_index, status_values);
  set_controls(controls);
}

} // namespace irata::sim::components::controller
