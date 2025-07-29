#pragma once

#include <irata/sim/hdl/status_decl.hpp>
#include <map>

namespace irata::sim::components::controller {

class StatusEncoder;

// CompleteStatuses is a helper class for encoding and decoding complete
// statuses. It is used by the StatusEncoder and InstructionEncoder classes.
// This class holds a complete status vector - it covers the full set of
// statuses in the microcode table and is therefore directly usable for
// encoding or decoding.
class CompleteStatuses {
public:
  // Constructs a new CompleteStatuses object from the statuses and uses the
  // given coder to verify that the statuses are complete.
  // Note that this doesn't permute the statuses in any way and they must be
  // complete.
  CompleteStatuses(const StatusEncoder &encoder,
                   const std::map<const hdl::StatusDecl *, bool> &statuses);

  const std::map<const hdl::StatusDecl *, bool> &statuses() const;

  bool operator==(const CompleteStatuses &other) const;
  bool operator!=(const CompleteStatuses &other) const;

private:
  const std::map<const hdl::StatusDecl *, bool> statuses_;
};

std::ostream &operator<<(std::ostream &os, const CompleteStatuses &statuses);

} // namespace irata::sim::components::controller
