#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

namespace irata::sim {

class ByteResult;

class Byte {
public:
  static Byte from_unsigned(uint8_t value);
  static Byte from_signed(int8_t value);
  Byte() = default;
  Byte(const Byte &) = default;
  virtual ~Byte() = default;

  bool operator==(const Byte &rhs) const;
  bool operator!=(const Byte &rhs) const;

  bool bit(uint8_t index) const;
  std::vector<bool> bits() const;

  uint8_t unsigned_value() const;
  int8_t signed_value() const;

  ByteResult add(Byte rhs, bool carry_in = false) const;
  ByteResult subtract(Byte rhs, bool borrow_in = false) const;
  ByteResult and_(Byte rhs) const;
  ByteResult or_(Byte rhs) const;
  ByteResult xor_(Byte rhs) const;
  ByteResult shift_left() const;
  ByteResult shift_right() const;
  ByteResult rotate_left(bool carry_in = false) const;
  ByteResult rotate_right(bool carry_in = false) const;

private:
  explicit Byte(uint8_t value);

  const uint8_t value_ = 0;
};

std::ostream &operator<<(std::ostream &os, const Byte &byte);

} // namespace irata::sim
