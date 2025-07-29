#include <irata/sim/hdl/tick_phase.hpp>

namespace irata::sim::hdl {

std::ostream &operator<<(std::ostream &os, TickPhase phase) {
  switch (phase) {
  case TickPhase::Control:
    return os << "Control";
  case TickPhase::Write:
    return os << "Write";
  case TickPhase::Read:
    return os << "Read";
  case TickPhase::Process:
    return os << "Process";
  case TickPhase::Clear:
    return os << "Clear";
  default:
    return os << "Unknown";
  }
}

} // namespace irata::sim::hdl