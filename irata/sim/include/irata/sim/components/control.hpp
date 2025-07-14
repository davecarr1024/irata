#pragma once

#include <irata/sim/components/component.hpp>
#include <optional>

namespace irata::sim {

// Control is a component that represents a control line.
// It can be set to true or false, and by default will be cleared at the end of
// the tick. If auto_clear is false, the control line will not be cleared at the
// end of the tick and a clear child control line will be created for explicit
// clearing.
class Control : public Component {
public:
  explicit Control(std::string_view name, bool auto_clear = true);
  virtual ~Control() = default;

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

private:
  // The value of the control line.
  bool value_;
  // Whether the control line should be cleared at the end of the tick.
  bool auto_clear_;
  // The clear control line, if any.
  std::unique_ptr<Control> clear_;
};

} // namespace irata::sim