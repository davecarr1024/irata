#include <irata/sim/components/memory/address.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::components::memory {

Address::Address(Component &parent, WordBus &address_bus, ByteBus &data_bus,
                 const Status &carry)
    : Component("address", &parent), address_bus_(address_bus),
      data_bus_(data_bus), read_("read", hdl::TickPhase::Read, this),
      write_("write", hdl::TickPhase::Write, this),
      reset_("reset", hdl::TickPhase::Process, this),
      high_("high", &data_bus, this), low_("low", &data_bus, this),
      increment_("increment", hdl::TickPhase::Process, this),
      decrement_("decrement", hdl::TickPhase::Process, this), carry_(carry),
      carry_increment_("carry_increment", hdl::TickPhase::Process, this) {}

hdl::ComponentType Address::type() const {
  return hdl::ComponentType::MemoryAddress;
}

const WordBus &Address::address_bus() const { return address_bus_; }
WordBus &Address::address_bus() { return address_bus_; }

const ByteBus &Address::data_bus() const { return data_bus_; }
ByteBus &Address::data_bus() { return data_bus_; }

bool Address::read() const { return read_.value(); }
void Address::set_read(bool value) { read_.set_value(value); }
Control &Address::read_control() { return read_; }
const Control &Address::read_control() const { return read_; }

bool Address::write() const { return write_.value(); }
void Address::set_write(bool value) { write_.set_value(value); }
Control &Address::write_control() { return write_; }
const Control &Address::write_control() const { return write_; }

bool Address::reset() const { return reset_.value(); }
void Address::set_reset(bool value) { reset_.set_value(value); }
Control &Address::reset_control() { return reset_; }
const Control &Address::reset_control() const { return reset_; }

Byte Address::high() const { return high_.value(); }
void Address::set_high(Byte value) { high_.set_value(value); }
Counter &Address::high_register() { return high_; }
const Counter &Address::high_register() const { return high_; }

Byte Address::low() const { return low_.value(); }
void Address::set_low(Byte value) { low_.set_value(value); }
Counter &Address::low_register() { return low_; }
const Counter &Address::low_register() const { return low_; }

bool Address::increment() const { return increment_.value(); }
void Address::set_increment(bool value) { increment_.set_value(value); }
Control &Address::increment_control() { return increment_; }
const Control &Address::increment_control() const { return increment_; }

bool Address::decrement() const { return decrement_.value(); }
void Address::set_decrement(bool value) { decrement_.set_value(value); }
Control &Address::decrement_control() { return decrement_; }
const Control &Address::decrement_control() const { return decrement_; }

bool Address::carry_increment() const { return carry_increment_.value(); }
void Address::set_carry_increment(bool value) {
  carry_increment_.set_value(value);
}
Control &Address::carry_increment_control() { return carry_increment_; }
const Control &Address::carry_increment_control() const {
  return carry_increment_;
}

bool Address::carry() const { return carry_.value(); }
const Status &Address::carry_status() const { return carry_; }

Word Address::value() const { return Word::from_bytes(high(), low()); }

void Address::set_value(Word value) {
  const auto [high, low] = value.to_bytes();
  set_high(high);
  set_low(low);
}

void Address::tick_write(Logger &logger) {
  if (write()) {
    const auto value = this->value();
    address_bus_.set_value(value, *this);
    logger << "Wrote " << value << " to " << address_bus_.path();
  }
}

void Address::tick_read(Logger &logger) {
  if (read()) {
    if (const auto value = address_bus_.value(); value != std::nullopt) {
      set_value(*value);
      logger << "Read " << *value << " from " << address_bus_.path();
    } else {
      std::ostringstream os;
      os << path() << " reading from open bus " << address_bus_.path();
      throw std::runtime_error(os.str());
    }
  }
}

void Address::tick_process(Logger &logger) {
  if (reset()) {
    set_value(0);
    logger << "Reset to 0";
  }
  if (increment()) {
    set_value(value() + Word(0x0001));
    logger << "Incremented to " << value();
  }
  if (decrement()) {
    set_value(value() - Word(0x0001));
    logger << "Decremented to " << value();
  }
  if (carry_increment()) {
    if (carry()) {
      set_high(high() + 0x01);
      logger << "Carry incremented to " << value();
    }
  }
}

void Address::serialize(Serializer &serializer) const {
  serializer.property("value", value());
}

} // namespace irata::sim::components::memory
