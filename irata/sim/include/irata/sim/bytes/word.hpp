#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <utility>

namespace irata::sim {

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

private:
  uint16_t value_;
};

} // namespace irata::sim
