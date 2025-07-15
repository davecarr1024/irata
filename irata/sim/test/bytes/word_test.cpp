#include <gtest/gtest.h>
#include <irata/sim/bytes/word.hpp>

using namespace irata::sim;

TEST(WordTest, DefaultConstructor) {
  const Word word;
  EXPECT_EQ(word.value(), 0);
}

TEST(WordTest, ForValue) {
  const Word word(0x1234);
  EXPECT_EQ(word.value(), 0x1234);
}

TEST(WordTest, FromBytes) {
  const Word word =
      Word::from_bytes(Byte::from_unsigned(0x12), Byte::from_unsigned(0x34));
  EXPECT_EQ(word.value(), 0x1234);
}

TEST(WordTest, ToBytes) {
  const Word word(0x1234);
  const auto [high, low] = word.to_bytes();
  EXPECT_EQ(high, Byte::from_unsigned(0x12));
  EXPECT_EQ(low, Byte::from_unsigned(0x34));
}