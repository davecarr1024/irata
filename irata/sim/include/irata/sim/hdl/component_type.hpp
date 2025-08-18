#pragma once

#include <ostream>

namespace irata::sim::hdl {

enum class ComponentType {
  Unknown,
  Alu,
  AluModule,
  Irata,
  Cpu,
  Memory,
  Controller,
  Register,
  IncrementableRegister,
  WordRegister,
  IncrementableWordRegister,
  ByteBus,
  WordBus,
  Control,
  Status,
  StatusRegister,
  MemoryAddress,
};

std::ostream &operator<<(std::ostream &os, ComponentType type);

} // namespace irata::sim::hdl
