#include <irata/sim/components/counter.hpp>

namespace irata::sim::components {

Counter::Counter(std::string_view name, Bus<Byte> *bus, Component *parent)
    : Register(name, bus, parent),
      increment_("increment", hdl::TickPhase::Process, this),
      decrement_("decrement", hdl::TickPhase::Process, this) {}

hdl::ComponentType Counter::type() const {
  return hdl::ComponentType::IncrementableRegister;
}

bool Counter::increment() const { return increment_.value(); }

void Counter::set_increment(bool value) { increment_.set_value(value); }

Control &Counter::increment_control() { return increment_; }

const Control &Counter::increment_control() const { return increment_; }

bool Counter::decrement() const { return decrement_.value(); }

void Counter::set_decrement(bool value) { decrement_.set_value(value); }

Control &Counter::decrement_control() { return decrement_; }

const Control &Counter::decrement_control() const { return decrement_; }

void Counter::tick_process(Logger &logger) {
  if (reset()) {
    logger << "Resetting to 0";
    set_value(Byte::from_unsigned(0));
  } else if (increment()) {
    logger << "Incrementing by 1";
    set_value(value() + 1);
  } else if (decrement()) {
    logger << "Decrementing by 1";
    set_value(value() - 1);
  }
}

} // namespace irata::sim::components
