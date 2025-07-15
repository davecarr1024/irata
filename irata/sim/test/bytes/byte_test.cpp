#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/byte_result.hpp>
#include <stdexcept>

using namespace irata::sim;
using ::testing::ElementsAre;

TEST(ByteTest, DefaultConstructor) {
  const Byte byte;
  EXPECT_EQ(byte.unsigned_value(), 0);
  EXPECT_EQ(byte.signed_value(), 0);
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

TEST(ByteTest, AddWithCarryOutAndZeroResult) {
  Byte lhs = Byte::from_unsigned(0xFF);
  Byte rhs = Byte::from_unsigned(0x01);
  ByteResult res = lhs.add(rhs, false);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x00),
                     .carry = true,
                     .zero = true,
                     .negative = false,
                     .overflow = false,
                     .half_carry = true,
                 }));
}

TEST(ByteTest, AddWithCarryIn) {
  Byte lhs = Byte::from_unsigned(0x01);
  Byte rhs = Byte::from_unsigned(0x01);
  ByteResult res = lhs.add(rhs, true);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x03),
                     .carry = false,
                     .zero = false,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, SubtractWithBorrowOut) {
  Byte lhs = Byte::from_unsigned(0x00);
  Byte rhs = Byte::from_unsigned(0x01);
  ByteResult res = lhs.subtract(rhs, false);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0xFF),
                     .carry = true,
                     .zero = false,
                     .negative = true,
                     .overflow = false,
                     .half_carry = true,
                 }));
}

TEST(ByteTest, SubtractWithBorrowIn) {
  Byte lhs = Byte::from_unsigned(0x02);
  Byte rhs = Byte::from_unsigned(0x01);
  ByteResult res = lhs.subtract(rhs, true);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x00),
                     .carry = false,
                     .zero = true,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, AndOperation) {
  Byte lhs = Byte::from_unsigned(0xAA);
  Byte rhs = Byte::from_unsigned(0x0F);
  ByteResult res = lhs.and_(rhs);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x0A),
                     .carry = false,
                     .zero = false,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, OrOperation) {
  Byte lhs = Byte::from_unsigned(0xA0);
  Byte rhs = Byte::from_unsigned(0x0F);
  ByteResult res = lhs.or_(rhs);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0xAF),
                     .carry = false,
                     .zero = false,
                     .negative = true,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, XorOperation) {
  Byte lhs = Byte::from_unsigned(0xFF);
  Byte rhs = Byte::from_unsigned(0x0F);
  ByteResult res = lhs.xor_(rhs);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0xF0),
                     .carry = false,
                     .zero = false,
                     .negative = true,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, ShiftLeftWithCarry) {
  Byte byte = Byte::from_unsigned(0x81);
  ByteResult res = byte.shift_left();
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x02),
                     .carry = true,
                     .zero = false,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, ShiftRightWithCarry) {
  Byte byte = Byte::from_unsigned(0x03);
  ByteResult res = byte.shift_right();
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x01),
                     .carry = true,
                     .zero = false,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, RotateLeftWithCarryIn) {
  Byte byte = Byte::from_unsigned(0x80);
  ByteResult res = byte.rotate_left(true);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x01),
                     .carry = true,
                     .zero = false,
                     .negative = false,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, RotateRightWithCarryIn) {
  Byte byte = Byte::from_unsigned(0x01);
  ByteResult res = byte.rotate_right(true);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x80),
                     .carry = true,
                     .zero = false,
                     .negative = true,
                     .overflow = false,
                     .half_carry = false,
                 }));
}

TEST(ByteTest, AddSignedOverflow) {
  Byte lhs = Byte::from_signed(127); // 0x7F
  Byte rhs = Byte::from_signed(1);   // 0x01
  ByteResult res = lhs.add(rhs);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0x80),
                     .carry = false,
                     .zero = false,
                     .negative = true,
                     .overflow = true,
                     .half_carry = true,
                 }));
}

TEST(ByteTest, SubtractNegativeWithoutBorrowOut) {
  Byte lhs = Byte::from_unsigned(0x10);
  Byte rhs = Byte::from_unsigned(0x20);
  ByteResult res = lhs.subtract(rhs, false);
  EXPECT_EQ(res, (ByteResult{
                     .result = Byte::from_unsigned(0xF0),
                     .carry = true,
                     .zero = false,
                     .negative = true,
                     .overflow = false,
                     .half_carry = false,
                 }));
}
