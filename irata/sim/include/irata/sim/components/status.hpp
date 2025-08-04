#pragma once

#include <irata/sim/components/component.hpp>

namespace irata::sim::components {

// A status line that can be set to true or false.
// Status lines are used to indicate the state of a component and can
// be read by other components.
class Status : public Component {
public:
  // Constructs a status line with the given name.
  explicit Status(std::string_view name, Component *parent = nullptr);
  virtual ~Status() = default;

  // Returns the current value of this status line.
  bool value() const;
  // Sets the current value of this status line.
  void set_value(bool value);

  // Returns all status lines in this component and its children.
  std::vector<Status *> statuses() override;

  hdl::ComponentType type() const override final;

private:
  bool value_ = false;
};

} // namespace irata::sim::components
