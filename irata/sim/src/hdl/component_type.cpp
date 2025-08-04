#include <irata/sim/hdl/component_type.hpp>

namespace irata::sim::hdl {

std::ostream &operator<<(std::ostream &os, ComponentType type) {
  switch (type) {
  case ComponentType::Irata:
    return os << "Irata";
  case ComponentType::Cpu:
    return os << "Cpu";
  case ComponentType::Memory:
    return os << "Memory";
  case ComponentType::Register:
    return os << "Register";
  case ComponentType::WordRegister:
    return os << "WordRegister";
  case ComponentType::ByteBus:
    return os << "ByteBus";
  case ComponentType::WordBus:
    return os << "WordBus";
  case ComponentType::Control:
    return os << "Control";
  case ComponentType::Status:
    return os << "Status";
  case ComponentType::Controller:
    return os << "Controller";
  default:
    return os << "Unknown";
  }
}

} // namespace irata::sim::hdl
