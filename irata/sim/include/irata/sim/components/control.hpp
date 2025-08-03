#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/hdl/tick_phase.hpp>
#include <optional>

namespace irata::sim::components {

// Control is a component that represents a control line.
// It can be set to true or false, and by default will be cleared at the end of
// the tick. If auto_clear is false, the control line will not be cleared at the
// end of the tick and a clear child control line will be created for explicit
// clearing.
class Control final : public Component {
public:
  explicit Control(std::string_view name, hdl::TickPhase phase,
                   Component *parent = nullptr, bool auto_clear = true);

  // Does this control line auto-clear at the end of the tick?
  // If not, a clear child control line will be created for explicit clearing.
  // If yes, the control line will be cleared at the end of the tick.
  bool auto_clear() const;

  // Returns the phase of the control line.
  // The control value can only be accessed during this phase.
  hdl::TickPhase phase() const;

  // Returns the value of the control line.
  bool value() const;
  // Sets the value of the control line.
  void set_value(bool value);

  // Returns the value of the clear control line, if any.
  std::optional<bool> clear() const;
  // Sets the value of the clear control line, if any.
  void set_clear(bool value);

  // Handle manual clearing of the control line.
  void tick_process(Logger &logger) override;

  // Auto-clear the control line, if auto clear is enabled.
  void tick_clear(Logger &logger) override;

  // Returns a list of all controls in the component tree.
  std::vector<Control *> controls() override;

  hdl::ComponentType type() const override final;

private:
  // The phase of the control line.
  const hdl::TickPhase phase_;
  // The value of the control line.
  bool value_;
  // Whether the control line should be cleared at the end of the tick.
  const bool auto_clear_;
  // The clear control line, if any.
  const std::unique_ptr<Control> clear_;
};

} // namespace irata::sim::components
