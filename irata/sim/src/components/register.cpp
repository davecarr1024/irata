#include <irata/sim/components/control.hpp>
#include <irata/sim/components/register.hpp>
#include <stdexcept>

namespace irata::sim::components {

Register::Register(std::string_view name, Bus<Byte> *bus, Component *parent)
    : Component(name, parent), value_(Byte::from_unsigned(0)), bus_(bus),
      write_(bus ? std::make_unique<Control>("write", this) : nullptr),
      read_(bus ? std::make_unique<Control>("read", this) : nullptr),
      reset_("reset", this) {}

Byte Register::value() const { return value_; }

void Register::set_value(Byte value) { value_ = value; }

bool Register::has_bus() const { return bus_ != nullptr; }
Bus<Byte> *Register::bus() { return bus_; }
const Bus<Byte> *Register::bus() const { return bus_; }

bool Register::has_write() const { return write_ != nullptr; }
std::optional<bool> Register::write() const {
  return write_ ? std::optional<bool>(write_->value()) : std::nullopt;
}
void Register::set_write(bool value) {
  if (!write_) {
    throw std::logic_error(path() + " has no write control");
  }
  write_->set_value(value);
}

bool Register::has_read() const { return read_ != nullptr; }
std::optional<bool> Register::read() const {
  return read_ ? std::optional<bool>(read_->value()) : std::nullopt;
}
void Register::set_read(bool value) {
  if (!read_) {
    throw std::logic_error(path() + " has no read control");
  }
  read_->set_value(value);
}

bool Register::reset() const { return reset_.value(); }
void Register::set_reset(bool value) { reset_.set_value(value); }

void Register::tick_write(Logger &logger) {
  if (const auto write = this->write(); write && *write) {
    bus_->set_value(value_, *this);
    logger << "wrote " << value_ << " to bus";
  }
}

void Register::tick_read(Logger &logger) {
  if (const auto read = this->read(); read && *read) {
    if (const auto value = bus_->value(); value) {
      set_value(*value);
    } else {
      throw std::logic_error(path() + " reading from open bus");
    }
  }
}

void Register::tick_process(Logger &logger) {
  if (reset()) {
    set_value(Byte::from_unsigned(0));
  }
}

} // namespace irata::sim::components
