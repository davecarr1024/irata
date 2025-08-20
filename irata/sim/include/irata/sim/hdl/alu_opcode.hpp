#pragma once

#include <cstdint>
#include <ostream>

namespace irata::sim::hdl {

enum class AluOpcode : uint8_t {
  Nop = 0x00,
  Add = 0x01,
  Subtract = 0x02,
  And = 0x03,
  Or = 0x04,
  Xor = 0x05,
  RotateLeft = 0x06,
  RotateRight = 0x07,
  ShiftLeft = 0x08,
  ShiftRight = 0x09,
  AddressAdd = 0x0A,
};

std::ostream &operator<<(std::ostream &os, AluOpcode opcode);

} // namespace irata::sim::hdl
