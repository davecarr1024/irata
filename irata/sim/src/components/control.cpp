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

bool Control::value() const { return value_; }

void Control::set_value(bool value) { value_ = value; }

std::optional<bool> Control::clear() const {
  return clear_ ? std::optional<bool>(clear_->value()) : std::nullopt;
}

void Control::set_clear(bool value) {
  if (clear_) {
    clear_->set_value(value);
  }
}

void Control::tick_process(Logger &logger) {
  if (clear_ && clear_->value() && value()) {
    set_value(false);
    logger << "manually cleared";
  }
}

void Control::tick_clear(Logger &logger) {
  if (auto_clear_ && value()) {
    set_value(false);
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