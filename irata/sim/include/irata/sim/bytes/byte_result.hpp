#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <ostream>

namespace irata::sim {

struct ByteResult {
  Byte result;
  bool carry = false;
  bool zero = false;
  bool negative = false;
  bool overflow = false;
  bool half_carry = false;

  bool operator==(const ByteResult &rhs) const;
  bool operator!=(const ByteResult &rhs) const;
};

std::ostream &operator<<(std::ostream &os, const ByteResult &result);

} // namespace irata::sim