#include <gtest/gtest.h>
#include <irata/sim/bytes/byte_result.hpp>

using namespace irata::sim;

TEST(ByteResultTest, DefaultConstructor) {
  EXPECT_EQ(ByteResult{}, (ByteResult{.result = Byte::from_unsigned(0),
                                      .carry = false,
                                      .zero = false,
                                      .negative = false,
                                      .overflow = false,
                                      .half_carry = false}));
}

TEST(ByteResultTest, Equality) {
  EXPECT_EQ((ByteResult{.result = Byte::from_unsigned(1), .carry = true}),
            (ByteResult{.result = Byte::from_unsigned(1), .carry = true}));
  EXPECT_NE((ByteResult{.result = Byte::from_unsigned(1), .carry = true}),
            (ByteResult{.result = Byte::from_unsigned(1), .carry = false}));
  EXPECT_NE((ByteResult{.result = Byte::from_unsigned(1), .carry = true}),
            (ByteResult{.result = Byte::from_unsigned(2), .carry = true}));
}

TEST(ByteResultTest, ToStr) {
  const auto to_str = [](const ByteResult &result) {
    std::ostringstream os;
    os << result;
    return os.str();
  };
  EXPECT_EQ(to_str(ByteResult{.result = Byte::from_unsigned(0)}),
            "<Byte(0x00)>");
  EXPECT_EQ(to_str(ByteResult{.result = Byte::from_unsigned(1),
                              .carry = true,
                              .zero = true,
                              .negative = true,
                              .overflow = true,
                              .half_carry = true}),
            "<Byte(0x01) C Z N V H>");
}