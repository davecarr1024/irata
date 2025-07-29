#pragma once

#include <ostream>

namespace irata::sim::hdl {

enum class ComponentType {
  Unknown,
  Irata,
  Cpu,
  Memory,
  Controller,
  Register,
  WordRegister,
  ByteBus,
  WordBus,
  Control,
  Status,
};

std::ostream &operator<<(std::ostream &os, ComponentType type);

} // namespace irata::sim::hdl
