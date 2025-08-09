#include <irata/sim/components/control.hpp>

namespace irata::sim::components {

Control::Control(std::string_view name, hdl::TickPhase phase, Component *parent,
                 bool auto_clear)
    : Component(name, parent), phase_(phase), value_(false),
      auto_clear_(auto_clear),
      clear_(auto_clear_ ? nullptr
                         : std::make_unique<Control>(
                               "clear", hdl::TickPhase::Process, this, true)) {
  if (!((auto_clear_ &&
         (phase_ == hdl::TickPhase::Write || phase_ == hdl::TickPhase::Read ||
          phase_ == hdl::TickPhase::Process)) ||
        (!auto_clear_ && (phase_ == hdl::TickPhase::Process ||
                          phase_ == hdl::TickPhase::Clear)))) {
    std::ostringstream os;
    os << "Control " << path() << " has invalid phase " << phase_
       << " and auto_clear " << auto_clear_;
    throw std::invalid_argument(os.str());
  }
}

bool Control::can_be_read_during_phase(
    std::optional<hdl::TickPhase> phase) const {
  // No phase set: this is outside of a tick.
  if (phase == std::nullopt) {
    return true;
  }
  // Reading during this control's phase.
  if (*phase == phase_) {
    return true;
  }
  return false;
}

bool Control::can_be_set_during_phase(
    std::optional<hdl::TickPhase> phase) const {
  // No phase set: this is outside of a tick.
  if (phase == std::nullopt) {
    return true;
  }
  // Setting during control phase: this is normal and when we would
  // expect the controller to be asserting all controls for the tick.
  if (*phase == hdl::TickPhase::Control) {
    return true;
  }
  // Setting during this control's phase is ok if this isn't an auto clear
  // control, since non auto clear controls need to be manually clear.
  if (!auto_clear_ && *phase == phase_) {
    return true;
  }
  return false;
}

bool Control::value() const {
  const auto active_tick_phase = root()->active_tick_phase();
  if (!can_be_read_during_phase(active_tick_phase)) {
    std::ostringstream os;
    os << "Control " << path() << " is being read in phase "
       << *active_tick_phase;
    throw std::runtime_error(os.str());
  }
  return value_;
}

void Control::set_value(bool value) {
  const auto active_tick_phase = root()->active_tick_phase();
  if (!can_be_set_during_phase(active_tick_phase)) {
    std::ostringstream os;
    os << "Control " << path() << " is being set in phase "
       << *active_tick_phase;
    throw std::runtime_error(os.str());
  }
  value_ = value;
}

std::optional<bool> Control::clear() const {
  return clear_ ? std::optional<bool>(clear_->value()) : std::nullopt;
}

void Control::set_clear(bool value) {
  if (clear_) {
    clear_->set_value(value);
  }
}

void Control::tick_process(Logger &logger) {
  if (clear_ && clear_->value() && value_) {
    value_ = false;
    logger << "manually cleared";
  }
}

void Control::tick_clear(Logger &logger) {
  if (auto_clear_ && value_) {
    value_ = false;
    logger << "auto cleared";
  }
}

std::vector<Control *> Control::controls() {
  std::vector<Control *> controls = Component::controls();
  controls.push_back(this);
  return controls;
}

hdl::TickPhase Control::phase() const { return phase_; }

bool Control::auto_clear() const { return auto_clear_; }

hdl::ComponentType Control::type() const { return hdl::ComponentType::Control; }

} // namespace irata::sim::components