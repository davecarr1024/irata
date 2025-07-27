#include <iomanip>
#include <irata/common/bytes/byte.hpp>
#include <irata/common/bytes/byte_result.hpp>
#include <stdexcept>

namespace irata::common::bytes {

Byte Byte::from_unsigned(uint8_t value) { return Byte(value); }

Byte Byte::from_signed(int8_t value) {
  return Byte(static_cast<uint8_t>(value));
}

Byte::Byte(uint8_t value) : value_(value) {}

bool Byte::operator==(const Byte &rhs) const { return value_ == rhs.value_; }

bool Byte::operator!=(const Byte &rhs) const { return value_ != rhs.value_; }

bool Byte::operator<(const Byte &rhs) const { return value_ < rhs.value_; }

bool Byte::bit(uint8_t index) const {
  if (index >= 8) {
    throw std::out_of_range("index must be between 0 and 7");
  }
  return (value_ >> index) & 1;
}

std::vector<bool> Byte::bits() const {
  std::vector<bool> result;
  for (uint8_t i = 0; i < 8; ++i) {
    result.push_back(bit(i));
  }
  return result;
}

uint8_t Byte::value() const { return value_; }

uint8_t Byte::unsigned_value() const { return value_; }

int8_t Byte::signed_value() const { return static_cast<int8_t>(value_); }

std::ostream &operator<<(std::ostream &os, const Byte &byte) {
  return os << "0x" << std::uppercase << std::hex << std::setw(2)
            << std::setfill('0') << static_cast<int>(byte.unsigned_value());
}

ByteResult Byte::add(Byte rhs, bool carry_in) const {
  uint16_t lhs_val = this->unsigned_value();
  uint16_t rhs_val = rhs.unsigned_value();
  uint16_t carry_val = carry_in ? 1 : 0;
  uint16_t result = lhs_val + rhs_val + carry_val;

  uint8_t lhs_u = this->unsigned_value();
  uint8_t rhs_u = rhs.unsigned_value();
  uint8_t result_u = static_cast<uint8_t>(result);

  ByteResult out{
      .result = Byte::from_unsigned(result_u),
      .carry = result > 0xFF,
      .zero = result_u == 0,
      .negative = (result_u & 0x80) != 0,
      .overflow = (~(lhs_u ^ rhs_u) & (lhs_u ^ result_u) & 0x80) != 0,
      .half_carry = ((lhs_u & 0xF) + (rhs_u & 0xF) + carry_val) > 0xF,
  };

  return out;
}

ByteResult Byte::subtract(Byte rhs, bool borrow_in) const {
  uint16_t lhs_val = this->unsigned_value();
  uint16_t rhs_val = rhs.unsigned_value();
  uint16_t borrow_val = borrow_in ? 1 : 0;
  uint16_t result = lhs_val - rhs_val - borrow_val;

  uint8_t lhs_u = this->unsigned_value();
  uint8_t rhs_u = rhs.unsigned_value();
  uint8_t result_u = static_cast<uint8_t>(result);

  ByteResult out{
      .result = Byte::from_unsigned(result_u),
      .carry = lhs_val < (rhs_val + borrow_val),
      .zero = result_u == 0,
      .negative = (result_u & 0x80) != 0,
      .overflow = ((lhs_u ^ rhs_u) & (lhs_u ^ result_u) & 0x80) != 0,
      .half_carry =
          (((lhs_u & 0xF) - (rhs_u & 0xF) - borrow_val) & 0x10) == 0x10,
  };

  return out;
}

ByteResult Byte::and_(Byte rhs) const {
  uint8_t result = unsigned_value() & rhs.unsigned_value();
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = false,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::or_(Byte rhs) const {
  uint8_t result = unsigned_value() | rhs.unsigned_value();
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = false,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::xor_(Byte rhs) const {
  uint8_t result = unsigned_value() ^ rhs.unsigned_value();
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = false,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::shift_left() const {
  uint8_t val = unsigned_value();
  uint8_t result = val << 1;
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = (val & 0x80) != 0,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::shift_right() const {
  uint8_t val = unsigned_value();
  uint8_t result = val >> 1;
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = (val & 0x01) != 0,
      .zero = result == 0,
      .negative = false, // logical shift right doesn't preserve sign
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::rotate_left(bool carry_in) const {
  uint8_t val = unsigned_value();
  uint8_t carry_bit = carry_in ? 1 : 0;
  bool new_carry = (val & 0x80) != 0;
  uint8_t result = static_cast<uint8_t>((val << 1) | carry_bit);
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = new_carry,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

ByteResult Byte::rotate_right(bool carry_in) const {
  uint8_t val = unsigned_value();
  uint8_t carry_bit = carry_in ? 0x80 : 0;
  bool new_carry = (val & 0x01) != 0;
  uint8_t result = static_cast<uint8_t>((val >> 1) | carry_bit);
  return ByteResult{
      .result = Byte::from_unsigned(result),
      .carry = new_carry,
      .zero = result == 0,
      .negative = (result & 0x80) != 0,
      .overflow = false,
      .half_carry = false,
  };
}

} // namespace irata::common::bytes
