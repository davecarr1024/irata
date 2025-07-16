#pragma once

#include <cstdint>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace irata::sim::components::controller {

// The control encoder is used to encode the controls of a microcode program
// into a row of instruction memory.
class ControlEncoder {
public:
  // Creates a new control encoder for the given instruction set.
  explicit ControlEncoder(const microcode::InstructionSet &instruction_set);
  virtual ~ControlEncoder() = default;

  // Encodes the given controls into a row of instruction memory.
  uint32_t encode(const std::vector<std::string> &controls) const;

  // Decodes the given row of instruction memory into a vector of controls.
  std::vector<std::string> decode(uint32_t encoded) const;

  // Returns the set of all controls in the instruction set.
  std::set<std::string> controls() const;

  // Returns a map of control names to their bit indices.
  std::map<std::string, uint8_t> control_indices() const;

  // Returns the number of controls in the instruction set.
  size_t control_count() const;

  // Returns the bit index of the given control.
  uint8_t control_index(const std::string &control) const;

private:
  const std::map<std::string, uint8_t> control_indices_;
};

} // namespace irata::sim::components::controller