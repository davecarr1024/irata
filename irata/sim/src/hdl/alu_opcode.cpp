#include <irata/sim/hdl/alu_opcode.hpp>
#include <stdexcept>
#include <string>

namespace irata::sim::hdl {

std::ostream &operator<<(std::ostream &os, AluOpcode opcode) {
  switch (opcode) {
  case AluOpcode::Add:
    return os << "Add";
  case AluOpcode::Subtract:
    return os << "Subtract";
  case AluOpcode::And:
    return os << "And";
  case AluOpcode::Or:
    return os << "Or";
  case AluOpcode::Xor:
    return os << "Xor";
  case AluOpcode::RotateLeft:
    return os << "RotateLeft";
  case AluOpcode::RotateRight:
    return os << "RotateRight";
  case AluOpcode::ShiftLeft:
    return os << "ShiftLeft";
  case AluOpcode::ShiftRight:
    return os << "ShiftRight";
  case AluOpcode::Nop:
    return os << "Nop";
  }
}

} // namespace irata::sim::hdl