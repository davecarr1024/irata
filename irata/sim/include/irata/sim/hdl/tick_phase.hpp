#pragma once

#include <ostream>

namespace irata::sim::hdl {

// Defines the ordered phases of a single tick in the CPU.
// Used by the microcode compiler to:
//   - enforce correct execution order
//   - identify when control lines are consumed
//   - support collapsing compatible operations within a tick
enum class TickPhase {
  Control, // Control lines are asserted
  Write,   // Values are written onto buses
  Read,    // Values are read from buses
  Process, // Local, internal component updates
  Clear,   // Cleanup of latched or asserted control signals
};

std::ostream &operator<<(std::ostream &os, TickPhase phase);

} // namespace irata::sim::hdl
