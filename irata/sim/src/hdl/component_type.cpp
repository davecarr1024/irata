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
  case ComponentType::Alu:
    return os << "Alu";
  case ComponentType::AluModule:
    return os << "AluModule";
  case ComponentType::Unknown:
    return os << "Unknown";
  default:
    throw std::logic_error("Unknown component type");
  }
}

} // namespace irata::sim::hdl
