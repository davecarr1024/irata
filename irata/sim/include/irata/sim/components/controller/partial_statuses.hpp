#pragma once

#include <irata/sim/hdl/status_decl.hpp>
#include <map>

namespace irata::sim::components::controller {

// PartialStatuses is a helper class for encoding and decoding partial statuses.
// It is used by the StatusEncoder and InstructionEncoder classes.
// This class holds a partial status vector - it might not cover the full set of
// statuses in the microcode table and is therefore not directly usable for
// encoding or decoding.
class PartialStatuses {
public:
  explicit PartialStatuses(
      const std::map<const hdl::StatusDecl *, bool> &statuses);

  const std::map<const hdl::StatusDecl *, bool> &statuses() const;

private:
  const std::map<const hdl::StatusDecl *, bool> statuses_;
};

} // namespace irata::sim::components::controller
