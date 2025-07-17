#pragma once

#include <cstdint>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/controller/instruction_memory.hpp>
#include <irata/sim/components/counter.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>

namespace irata::sim::components::controller {

class Controller : public Component {
public:
  // Creates a new controller with the given instruction set.
  // The instruction set is used to build up the instruction memory for the
  // CPU, which is then used to control the CPU's execution.
  // The name is used to identify the controller in the component hierarchy.
  // The parent is the parent component in the component hierarchy.
  // The path prefix is used to prefix the paths of the controller's control and
  // status references. This allows the controller to be used in a sub-component
  // hierarchy.
  // Note that the path prefix, when combined with paths for controls
  // and statuses from the instruction set, must result in an absolute path
  // starting with a slash.
  // If bus is provided, the controller will connect it to the opcode and step
  // index registers.
  explicit Controller(const microcode::InstructionSet &instruction_set,
                      Bus<Byte> *bus = nullptr,
                      std::string_view name = "controller",
                      Component *parent = nullptr,
                      std::string_view path_prefix = "");
  virtual ~Controller() = default;

  // Entry point for the controller.
  // This is called once per tick to update the controller's state, reads the
  // current opcode, step index, and statuses, and sets the controls for the
  // rest of the tick.
  void tick_control(Logger &logger) override;

  // Returns the current opcode.
  Byte opcode() const;
  // Sets the current opcode.
  void set_opcode(Byte opcode);

  // Returns the current step index.
  Byte step_index() const;
  // Sets the current step index.
  void set_step_index(Byte step_index);

private:
  const InstructionMemory instruction_memory_;
  const std::string path_prefix_;
  Register opcode_;
  Counter step_index_;

  std::string prefix_path(std::string_view path) const;

  std::map<std::string, const Status *> statuses_by_path() const;

  std::map<std::string, bool> status_values() const;

  std::map<std::string, Control *> controls_by_path();

  // Sets the controls that are asserted in the current tick.
  // Note that the controls are un-prefixed and come straight from the
  // instruction memory, and must be prefixed to match the component hierarchy.
  void set_controls(std::set<std::string> unprefixed_controls);
};

} // namespace irata::sim::components::controller
