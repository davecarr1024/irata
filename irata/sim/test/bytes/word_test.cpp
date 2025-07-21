#include <gtest/gtest.h>
#include <irata/sim/bytes/word.hpp>

namespace irata::sim {

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

TEST(WordTest, ToString) {
  const auto to_string = [](Word word) {
    std::ostringstream os;
    os << word;
    return os.str();
  };
  EXPECT_EQ(to_string(Word(0x1234)), "Word(0x1234)");
  EXPECT_EQ(to_string(Word(0x0000)), "Word(0x0000)");
}

TEST(WordTest, Equal) {
  EXPECT_EQ(Word(0x1234), Word(0x1234));
  EXPECT_NE(Word(0x1234), Word(0x5678));
}

TEST(WordTest, Add) { EXPECT_EQ(Word(0x1234) + Word(0x5678), Word(0x68AC)); }

TEST(WordTest, Subtract) {
  EXPECT_EQ(Word(0x5678) - Word(0x1234), Word(0x4444));
}

TEST(WordTest, LessThan) { EXPECT_LT(Word(0x1234), Word(0x5678)); }

TEST(WordTest, LessThanOrEqual) { EXPECT_LE(Word(0x1234), Word(0x5678)); }

TEST(WordTest, GreaterThan) { EXPECT_GT(Word(0x5678), Word(0x1234)); }

TEST(WordTest, GreaterThanOrEqual) { EXPECT_GE(Word(0x5678), Word(0x1234)); }

TEST(WordTest, AddSizet) {
  EXPECT_EQ(Word(0x1234) + size_t(0x5678), Word(0x68AC));
}

TEST(WordTest, SubtractSizet) {
  EXPECT_EQ(Word(0x5678) - size_t(0x1234), Word(0x4444));
}

} // namespace irata::sim