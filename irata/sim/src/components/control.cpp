#include <irata/sim/components/control.hpp>

namespace irata::sim::components {

Control::Control(std::string_view name, Component *parent, bool auto_clear)
    : Component(name, parent), value_(false), auto_clear_(auto_clear),
      clear_(auto_clear_ ? nullptr
                         : std::make_unique<Control>("clear", this, true)) {}

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

} // namespace irata::sim::components