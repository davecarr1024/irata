#include <iomanip>
#include <irata/sim/bytes/word.hpp>
#include <utility>

namespace irata::sim {

Word::Word(uint16_t value) : value_(value) {}

Word Word::from_bytes(Byte high, Byte low) {
  return Word((high.unsigned_value() << 8) | low.unsigned_value());
}

std::pair<Byte, Byte> Word::to_bytes() const {
  return {Byte::from_unsigned(value_ >> 8), Byte::from_unsigned(value_ & 0xFF)};
}

uint16_t Word::value() const { return value_; }

std::ostream &operator<<(std::ostream &os, const Word &word) {
  return os << "Word(0x" << std::uppercase << std::hex << std::setw(4)
            << std::setfill('0') << static_cast<int>(word.value()) << ")";
}

bool Word::operator==(const Word &other) const {
  return value_ == other.value_;
}

bool Word::operator!=(const Word &other) const { return !(*this == other); }

bool Word::operator<(const Word &other) const { return value_ < other.value_; }

bool Word::operator<=(const Word &other) const {
  return value_ <= other.value_;
}

bool Word::operator>(const Word &other) const { return value_ > other.value_; }

bool Word::operator>=(const Word &other) const {
  return value_ >= other.value_;
}

Word Word::operator+(const Word &other) const {
  return Word(value_ + other.value_);
}

Word Word::operator-(const Word &other) const {
  return Word(value_ - other.value_);
}

} // namespace irata::sim
