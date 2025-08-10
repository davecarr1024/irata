#include <irata/sim/components/word_counter.hpp>

namespace irata::sim::components {

WordCounter::WordCounter(std::string_view name, Bus<Word> *bus,
                         Bus<Byte> *byte_bus, Component *parent)
    : WordRegister(name, bus, byte_bus, parent),
      increment_("increment", hdl::TickPhase::Process, this),
      decrement_("decrement", hdl::TickPhase::Process, this) {}

hdl::ComponentType WordCounter::type() const {
  return hdl::ComponentType::IncrementableWordRegister;
}

bool WordCounter::increment() const { return increment_.value(); }

void WordCounter::set_increment(bool increment) {
  increment_.set_value(increment);
}

Control &WordCounter::increment_control() { return increment_; }

const Control &WordCounter::increment_control() const { return increment_; }

bool WordCounter::decrement() const { return decrement_.value(); }

void WordCounter::set_decrement(bool decrement) {
  decrement_.set_value(decrement);
}

Control &WordCounter::decrement_control() { return decrement_; }

const Control &WordCounter::decrement_control() const { return decrement_; }

void WordCounter::tick_process(Logger &logger) {
  if (reset()) {
    logger << "Resetting to 0";
    set_value(Word(0));
  } else if (increment()) {
    logger << "Incrementing by 1";
    set_value(Word(value().value() + 1));
  } else if (decrement()) {
    logger << "Decrementing by 1";
    set_value(Word(value().value() - 1));
  }
}

} // namespace irata::sim::components
