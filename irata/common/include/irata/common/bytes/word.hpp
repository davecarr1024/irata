#pragma once

#include <cstdint>
#include <irata/common/bytes/byte.hpp>
#include <ostream>
#include <utility>

namespace irata::common::bytes {

// A 16-bit word.
class Word {
public:
  // Constructs a word with the given value.
  explicit Word(uint16_t value = 0);
  Word(const Word &) = default;
  Word &operator=(const Word &) = default;
  virtual ~Word() = default;

  // Constructs a word from the given high and low bytes.
  static Word from_bytes(Byte high, Byte low);
  // Returns the high and low bytes of this word.
  std::pair<Byte, Byte> to_bytes() const;

  // Returns the value of this word.
  uint16_t value() const;

  bool operator==(const Word &other) const;
  bool operator!=(const Word &other) const;
  bool operator<(const Word &other) const;
  bool operator<=(const Word &other) const;
  bool operator>(const Word &other) const;
  bool operator>=(const Word &other) const;
  Word operator+(const Word &other) const;
  Word operator-(const Word &other) const;
  Word operator+(size_t other) const;
  Word operator-(size_t other) const;

private:
  uint16_t value_;
};

std::ostream &operator<<(std::ostream &os, const Word &word);

} // namespace irata::common::bytes
