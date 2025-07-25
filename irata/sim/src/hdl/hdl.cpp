#include <irata/sim/hdl/hdl.hpp>

namespace irata::sim::hdl {

const IrataDecl &irata() {
  static const IrataDecl irata_;
  return irata_;
}

std::ostream &operator<<(std::ostream &os, TickPhase phase) {
  switch (phase) {
  case TickPhase::Control:
    os << "Control";
    break;
  case TickPhase::Write:
    os << "Write";
    break;
  case TickPhase::Read:
    os << "Read";
    break;
  case TickPhase::Process:
    os << "Process";
    break;
  case TickPhase::Clear:
    os << "Clear";
    break;
  default:
    os << "Unknown";
    break;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const ComponentDecl &component) {
  os << "Component(" << component.path() << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ControlDecl &control) {
  os << "Control(" << control.path() << ", phase=" << control.phase() << ")";
  return os;
}

} // namespace irata::sim::hdl
