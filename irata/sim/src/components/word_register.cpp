#include <irata/sim/components/word_register.hpp>
#include <stdexcept>

namespace irata::sim::components {

WordRegister::WordRegister(std::string_view name, Bus<Word> *bus,
                           Bus<Byte> *byte_bus, Component *parent)
    : Component(name, parent), bus_(bus),
      read_(bus_ != nullptr
                ? std::make_unique<Control>("read", hdl::TickPhase::Read, this)
                : nullptr),
      write_(bus_ != nullptr ? std::make_unique<Control>(
                                   "write", hdl::TickPhase::Write, this)
                             : nullptr),
      reset_("reset", hdl::TickPhase::Process, this),
      high_("high", byte_bus, this), low_("low", byte_bus, this) {}

Word WordRegister::value() const { return Word::from_bytes(high(), low()); }

void WordRegister::set_value(Word value) {
  const auto [high, low] = value.to_bytes();
  set_high(high);
  set_low(low);
}

Byte WordRegister::high() const { return high_.value(); }

void WordRegister::set_high(Byte value) { high_.set_value(value); }

Byte WordRegister::low() const { return low_.value(); }

void WordRegister::set_low(Byte value) { low_.set_value(value); }

Bus<Word> *WordRegister::bus() const { return bus_; }

bool WordRegister::has_read() const { return read_ != nullptr; }

std::optional<bool> WordRegister::read() const {
  return read_ ? std::optional<bool>(read_->value()) : std::nullopt;
}

void WordRegister::set_read(bool value) {
  if (!has_read()) {
    throw std::logic_error("register does not have read control");
  }
  read_->set_value(value);
}

bool WordRegister::has_write() const { return write_ != nullptr; }

std::optional<bool> WordRegister::write() const {
  return write_ ? std::optional<bool>(write_->value()) : std::nullopt;
}

void WordRegister::set_write(bool value) {
  if (!has_write()) {
    throw std::logic_error("register does not have write control");
  }
  write_->set_value(value);
}

bool WordRegister::reset() const { return reset_.value(); }

void WordRegister::set_reset(bool value) { reset_.set_value(value); }

void WordRegister::tick_write(Logger &logger) {
  if (const auto write = this->write(); write && *write) {
    logger << "writing " << value();
    bus_->set_value(value(), *this);
  }
}

void WordRegister::tick_read(Logger &logger) {
  if (const auto read = this->read(); read && *read) {
    if (const auto bus_value = bus_->value(); bus_value != std::nullopt) {
      logger << "reading " << *bus_value;
      set_value(*bus_value);
    } else {
      throw std::logic_error("reading open bus");
    }
  }
}

void WordRegister::tick_process(Logger &logger) {
  if (reset()) {
    logger << "resetting to 0";
    set_value(Word(0x0000));
  }
}

Register &WordRegister::high_register() { return high_; }

const Register &WordRegister::high_register() const { return high_; }

Register &WordRegister::low_register() { return low_; }

const Register &WordRegister::low_register() const { return low_; }

void WordRegister::serialize(Serializer &serializer) const {
  serializer.property("value", value());
}

} // namespace irata::sim::components
