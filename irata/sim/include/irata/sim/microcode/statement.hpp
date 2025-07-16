#pragma once

#include <string>
#include <vector>

namespace irata::sim::microcode {

// A microcode statement is a collection of microcode controls that are asserted
// in parallel during one tick.
struct Statement {
  // The microcode controls that are asserted in parallel during one tick.
  const std::vector<std::string> controls;
};

} // namespace irata::sim::microcode
