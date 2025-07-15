#include <irata/sim/components/register.hpp>
#include <stdexcept>

namespace irata::sim {

Register::Register(std::string_view name, Bus &bus, Component *parent)
    : Component(name, parent), bus_(bus), write_("write", this),
      read_("read", this), reset_("reset", this) {}

Byte Register::value() const { return value_; }

void Register::set_value(Byte value) { value_ = value; }

bool Register::write() const { return write_.value(); }

void Register::set_write(bool value) { write_.set_value(value); }

bool Register::read() const { return read_.value(); }

void Register::set_read(bool value) { read_.set_value(value); }

bool Register::reset() const { return reset_.value(); }

void Register::set_reset(bool value) { reset_.set_value(value); }

void Register::tick_write(Logger &logger) {
  if (write()) {
    bus_.set_value(value_, *this);
    logger << "wrote " << value_ << " to bus";
  }
}

void Register::tick_read(Logger &logger) {
  if (read()) {
    const auto value = bus_.value();
    if (value == std::nullopt) {
      throw std::logic_error(path() + " reading from open bus");
    }
    set_value(*value);
    logger << "read " << *value << " from bus";
  }
}

void Register::tick_process(Logger &logger) {
  if (reset()) {
    set_value(Byte::from_unsigned(0));
    logger << "reset to " << value_;
  }
}

} // namespace irata::sim