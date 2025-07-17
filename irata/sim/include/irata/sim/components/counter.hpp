#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/register.hpp>
#include <string_view>

namespace irata::sim::components {

// A counter is a register that can be incremented.
// It has an increment control line that, when asserted, will increment the
// counter by one during tick_process.
class Counter : public Register {
public:
  // Constructs a counter with the given name.
  // If bus is non-nullptr, the counter will be connected to the bus and read
  // and write control lines will be created.
  // If parent is non-nullptr, the counter will be added to the parent
  // component.
  explicit Counter(std::string_view name, Bus<Byte> *bus = nullptr,
                   Component *parent = nullptr);
  virtual ~Counter() = default;

  bool increment() const;
  void set_increment(bool increment);

  void tick_process(Logger &logger) override;

private:
  Control increment_;
};

} // namespace irata::sim::components
