#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

namespace irata::sim {

class ByteResult;

// A single byte (8 bits).
class Byte {
public:
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

  // Returns the value of the bit at the given index (0-7).
  bool bit(uint8_t index) const;
  // Returns the value of all bits as a vector of bools.
  std::vector<bool> bits() const;

  // Returns the unsigned value of this byte.
  uint8_t unsigned_value() const;
  // Returns the signed value of this byte.
  int8_t signed_value() const;

  // Returns the result of adding this byte to the given byte.
  ByteResult add(Byte rhs, bool carry_in = false) const;

  // Returns the result of subtracting the given byte from this byte.
  ByteResult subtract(Byte rhs, bool borrow_in = false) const;

  // Returns the result of ANDing this byte with the given byte.
  ByteResult and_(Byte rhs) const;

  // Returns the result of ORing this byte with the given byte.
  ByteResult or_(Byte rhs) const;

  // Returns the result of XORing this byte with the given byte.
  ByteResult xor_(Byte rhs) const;

  // Returns the result of shifting this byte left by one bit.
  ByteResult shift_left() const;

  // Returns the result of shifting this byte right by one bit.
  ByteResult shift_right() const;

  // Returns the result of rotating this byte left by one bit.
  ByteResult rotate_left(bool carry_in = false) const;

  // Returns the result of rotating this byte right by one bit.
  ByteResult rotate_right(bool carry_in = false) const;

private:
  explicit Byte(uint8_t value);

  uint8_t value_ = 0;
};

std::ostream &operator<<(std::ostream &os, const Byte &byte);

} // namespace irata::sim
