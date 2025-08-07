#include <iomanip>
#include <irata/common/bytes/byte.hpp>
#include <stdexcept>

namespace irata::common::bytes {

Byte Byte::from_unsigned(uint8_t value) { return Byte(value); }

Byte Byte::from_signed(int8_t value) {
  return Byte(static_cast<uint8_t>(value));
}

Byte::Byte(uint8_t value) : value_(value) {}

Byte::operator uint8_t() const { return unsigned_value(); }

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

} // namespace irata::common::bytes
