#pragma once

#include <cstdint>
#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <set>

namespace irata::sim::components::controller {

// ControlEncoder is responsible for encoding the controls of the CPU into
// a value that can be used as the value of the encoded instruction memory.
class ControlEncoder {
public:
  explicit ControlEncoder(const microcode::table::Table &table);
  ControlEncoder(const ControlEncoder &) = delete;
  ControlEncoder &operator=(const ControlEncoder &) = delete;

  // Encodes the given controls into a value.
  uint32_t encode(const std::set<const hdl::ControlDecl *> &controls) const;

  // Decodes the given value into a set of controls.
  std::set<const hdl::ControlDecl *> decode(uint32_t encoded_controls) const;

  // Returns the number of controls in the table.
  size_t num_controls() const;

private:
  const std::map<const hdl::ControlDecl *, size_t> indices_;
};

} // namespace irata::sim::components::controller