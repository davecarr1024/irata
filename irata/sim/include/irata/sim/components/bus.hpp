#pragma once

#include <irata/sim/components/component.hpp>
#include <optional>

namespace irata::sim::components {

// A bus is a component that represents a data bus.
// It is used to transfer data between components.
// The bus clears its value at the end of the tick.
// It is an error if more than one component tries to set the value of the bus
// in the same tick.
template <typename T> class Bus : public Component {
public:
  // Constructs a bus with the given name and parent.
  explicit Bus(std::string_view name = "bus", Component *parent = nullptr);
  virtual ~Bus() = default;

  // Returns the current value on the bus during this tick, if any.
  std::optional<T> value() const;
  // Sets the value on the bus for this tick.
  void set_value(T value, Component &setter);

  std::optional<std::string> setter() const;

  void tick_clear(Logger &logger) override;

private:
  std::optional<T> value_;
  std::optional<std::string> setter_;
};

using ByteBus = Bus<Byte>;
using WordBus = Bus<Word>;

} // namespace irata::sim::components
