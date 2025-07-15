#include <irata/sim/components/bus.hpp>

namespace irata::sim {

Bus::Bus(std::string_view name, Component *parent) : Component(name, parent) {}

std::optional<Byte> Bus::value() const { return value_; }

void Bus::set_value(Byte value, Component &setter) {
  const auto setter_name = setter.path();
  if (setter_ != std::nullopt && setter_ != setter_name) {
    throw std::logic_error("write conflict: trying to write to bus by " +
                           setter_name + " but bus was already written by " +
                           *setter_);
  }
  setter_ = setter_name;
  value_ = value;
}

std::optional<std::string> Bus::setter() const { return setter_; }

void Bus::tick_clear(Logger &logger) {
  if (setter_ != std::nullopt) {
    logger << "clearing bus value set by " << *setter_;
    setter_ = std::nullopt;
    value_ = std::nullopt;
  }
}

} // namespace irata::sim