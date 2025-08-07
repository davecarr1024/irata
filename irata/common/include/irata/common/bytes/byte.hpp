#pragma once

#include <cstdint>
#include <ostream>
#include <type_traits>
#include <vector>

namespace irata::common::bytes {

class ByteResult;

// A single byte (8 bits).
class Byte {
public:
  // Constructs a byte with the given unsigned value.
  Byte(uint8_t value);

  operator uint8_t() const;

  // Constructs a byte with the given unsigned value.
  static Byte from_unsigned(uint8_t value);

  // Constructs a byte with the given signed value.
  static Byte from_signed(int8_t value);

  // Allow copying and assigning.
  Byte() = default;
  Byte(const Byte &) = default;
  Byte &operator=(const Byte &) = default;
  virtual ~Byte() = default;

  bool operator==(const Byte &rhs) const;
  bool operator!=(const Byte &rhs) const;
  bool operator<(const Byte &rhs) const;

  template <typename T, typename std::enable_if<
                            std::is_integral<T>::value>::type * = nullptr>
  bool operator==(T rhs) const {
    return rhs == value_;
  }
  template <typename T, typename std::enable_if<
                            std::is_integral<T>::value>::type * = nullptr>
  bool operator!=(T rhs) const {
    return rhs != value_;
  }

  // Returns the value of the bit at the given index (0-7).
  bool bit(uint8_t index) const;
  // Returns the value of all bits as a vector of bools.
  std::vector<bool> bits() const;

  // Returns the unsigned value of this byte.
  uint8_t value() const;
  // Returns the unsigned value of this byte.
  uint8_t unsigned_value() const;
  // Returns the signed value of this byte.
  int8_t signed_value() const;

private:
  uint8_t value_ = 0;
};

std::ostream &operator<<(std::ostream &os, const Byte &byte);

} // namespace irata::common::bytes
