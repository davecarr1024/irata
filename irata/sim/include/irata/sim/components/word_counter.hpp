#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/word_register.hpp>
#include <string_view>

namespace irata::sim::components {

// A word counter is a word register that can be incremented.
class WordCounter final : public WordRegister {
public:
  // Constructs a word counter with the given name.
  // If bus is non-nullptr, the word counter will be connected to the bus and
  // read and write control lines will be created.
  // If byte_bus is non-nullptr, the high and low byte registers will be
  // connected to the byte bus and read and write control lines will be created.
  // If parent is non-nullptr, the word counter will be added to the parent
  // component.
  // An increment control line will be created.
  explicit WordCounter(std::string_view name, Bus<Word> *bus = nullptr,
                       Bus<Byte> *byte_bus = nullptr,
                       Component *parent = nullptr);
  virtual ~WordCounter() = default;

  // Returns the value of the increment control line.
  bool increment() const;
  // Sets the value of the increment control line.
  void set_increment(bool increment);

protected:
  void tick_process(Logger &logger) override;

private:
  Control increment_;
};

} // namespace irata::sim::components