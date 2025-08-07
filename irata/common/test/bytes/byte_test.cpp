#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/common/bytes/byte.hpp>
#include <stdexcept>

using ::testing::ElementsAre;

namespace irata::common::bytes {

TEST(ByteTest, DefaultConstructor) {
  const Byte byte;
  EXPECT_EQ(byte.unsigned_value(), 0);
  EXPECT_EQ(byte.signed_value(), 0);
}

TEST(ByteTest, ImplicitConversion) {
  Byte byte = 0x12;
  uint8_t value = byte;
  EXPECT_EQ(value, 0x12);
}

TEST(ByteTest, UnsignedConstructor) {
  const Byte byte(0x12);
  EXPECT_EQ(byte.value(), 0x12);
}

TEST(ByteTest, FromUnsigned) {
  const Byte byte = Byte::from_unsigned(0xAB);
  EXPECT_EQ(byte.unsigned_value(), 0xAB);
  EXPECT_EQ(byte.signed_value(), -85);
}

TEST(ByteTest, FromSigned) {
  const Byte byte = Byte::from_signed(-85);
  EXPECT_EQ(byte.unsigned_value(), 0xAB);
  EXPECT_EQ(byte.signed_value(), -85);
}

TEST(ByteTest, Equality) {
  EXPECT_EQ(Byte::from_unsigned(0xAB), Byte::from_unsigned(0xAB));
  EXPECT_NE(Byte::from_unsigned(0xAB), Byte::from_unsigned(0xCD));
  EXPECT_EQ(Byte::from_signed(-85), Byte::from_signed(-85));
  EXPECT_NE(Byte::from_signed(-85), Byte::from_signed(85));
  EXPECT_EQ(Byte::from_signed(-85), Byte::from_unsigned(0xAB));
  EXPECT_EQ(Byte::from_unsigned(0xAB), Byte::from_signed(-85));
  EXPECT_NE(Byte::from_signed(85), Byte::from_unsigned(0xAB));
  EXPECT_NE(Byte::from_unsigned(0xAB), Byte::from_signed(85));

  EXPECT_EQ(Byte(0x12), 0x12);
  EXPECT_NE(Byte(0x12), 0x34);
}

TEST(ByteTest, Bit) {
  const Byte byte = Byte::from_unsigned(0b10101010);
  EXPECT_EQ(byte.bit(0), false);
  EXPECT_EQ(byte.bit(1), true);
  EXPECT_EQ(byte.bit(2), false);
  EXPECT_EQ(byte.bit(3), true);
  EXPECT_EQ(byte.bit(4), false);
  EXPECT_EQ(byte.bit(5), true);
  EXPECT_EQ(byte.bit(6), false);
  EXPECT_EQ(byte.bit(7), true);
  EXPECT_THROW(byte.bit(8), std::out_of_range);
}

TEST(ByteTest, Bits) {
  const Byte byte = Byte::from_unsigned(0b10101010);
  EXPECT_THAT(byte.bits(),
              ElementsAre(false, true, false, true, false, true, false, true));
}

TEST(ByteTest, ToString) {
  const auto to_str = [](const Byte &byte) {
    std::ostringstream os;
    os << byte;
    return os.str();
  };
  EXPECT_EQ(to_str(Byte::from_unsigned(0x00)), "0x00");
  EXPECT_EQ(to_str(Byte::from_unsigned(0xAB)), "0xAB");
  EXPECT_EQ(to_str(Byte::from_signed(-85)), "0xAB");
  EXPECT_EQ(to_str(Byte::from_signed(85)), "0x55");
}

} // namespace irata::common::bytes