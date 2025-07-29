#include <irata/sim/components/word_counter.hpp>

namespace irata::sim::components {

WordCounter::WordCounter(std::string_view name, Bus<Word> *bus,
                         Bus<Byte> *byte_bus, Component *parent)
    : WordRegister(name, bus, byte_bus, parent),
      increment_("increment", hdl::TickPhase::Process, this) {}

bool WordCounter::increment() const { return increment_.value(); }

void WordCounter::set_increment(bool increment) {
  increment_.set_value(increment);
}

void WordCounter::tick_process(Logger &logger) {
  if (reset()) {
    logger << "Resetting to 0";
    set_value(Word(0));
  } else if (increment()) {
    logger << "Incrementing by 1";
    set_value(Word(value().value() + 1));
  }
}

} // namespace irata::sim::components