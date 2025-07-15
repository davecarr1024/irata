#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <ostream>

namespace irata::sim {

// The result of a byte operation.
struct ByteResult {
  // The result of the operation.
  Byte result;
  // Whether the operation resulted in a carry.
  // A carry is a carry out of the most significant bit.
  bool carry = false;
  // Whether the operation resulted in a zero.
  // A zero result is a result with all bits clear.
  bool zero = false;
  // Whether the operation resulted in a negative.
  // A negative result is a result with the most significant bit set.
  bool negative = false;
  // Whether the operation resulted in an overflow.
  // An overflow is a carry out of the most significant bit.
  bool overflow = false;
  // Whether the operation resulted in a half carry.
  // A half carry is a carry out of the lower nibble.
  bool half_carry = false;

  bool operator==(const ByteResult &rhs) const;
  bool operator!=(const ByteResult &rhs) const;
};

std::ostream &operator<<(std::ostream &os, const ByteResult &result);

} // namespace irata::sim