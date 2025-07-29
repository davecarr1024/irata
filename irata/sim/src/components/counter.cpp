#include <irata/sim/bytes/byte_result.hpp>
#include <irata/sim/components/counter.hpp>

namespace irata::sim::components {

Counter::Counter(std::string_view name, Bus<Byte> *bus, Component *parent)
    : Register(name, bus, parent),
      increment_("increment", hdl::TickPhase::Process, this) {}

bool Counter::increment() const { return increment_.value(); }

void Counter::set_increment(bool value) { increment_.set_value(value); }

void Counter::tick_process(Logger &logger) {
  if (reset()) {
    logger << "Resetting to 0";
    set_value(Byte::from_unsigned(0));
  } else if (increment()) {
    logger << "Incrementing by 1";
    set_value(value().add(Byte::from_unsigned(1)).result);
  }
}

} // namespace irata::sim::components
