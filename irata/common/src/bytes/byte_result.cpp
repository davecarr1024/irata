#include <irata/common/bytes/byte_result.hpp>

namespace irata::common::bytes {

bool ByteResult::operator==(const ByteResult &rhs) const {
  return std::tie(result, carry, zero, negative, overflow, half_carry) ==
         std::tie(rhs.result, rhs.carry, rhs.zero, rhs.negative, rhs.overflow,
                  rhs.half_carry);
}

bool ByteResult::operator!=(const ByteResult &rhs) const {
  return !(*this == rhs);
}

std::ostream &operator<<(std::ostream &os, const ByteResult &result) {
  os << "<" << result.result;
  if (result.carry)
    os << " C";
  if (result.zero)
    os << " Z";
  if (result.negative)
    os << " N";
  if (result.overflow)
    os << " V";
  if (result.half_carry)
    os << " H";
  return os << ">";
}

} // namespace irata::common::bytes
