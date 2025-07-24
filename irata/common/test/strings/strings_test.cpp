#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/common/strings/strings.hpp>

using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace irata::common::strings {

TEST(StringsTest, Trim) {
  EXPECT_EQ(trim(""), "");
  EXPECT_EQ(trim("  "), "");
  EXPECT_EQ(trim("hello"), "hello");
  EXPECT_EQ(trim("  hello  "), "hello");
  EXPECT_EQ(trim("  hello  ", " h"), "ello");
}

TEST(StringsTest, Split) {
  EXPECT_THAT(split(""), IsEmpty());
  EXPECT_THAT(split("hello"), ElementsAre("hello"));
  EXPECT_THAT(split(" hello "), ElementsAre("hello"));
  EXPECT_THAT(split("hello world"), ElementsAre("hello", "world"));
  EXPECT_THAT(split("hello_world", "_"), ElementsAre("hello", "world"));
  EXPECT_THAT(split("__hello_world__", "_"), ElementsAre("hello", "world"));
}

TEST(StringsTest, Join) {
  EXPECT_EQ(join<std::string>({}, ""), "");
  EXPECT_EQ(join<std::string>({"hello"}, ""), "hello");
  EXPECT_EQ(join<std::string>({"hello", "world"}, " "), "hello world");
}

TEST(StringsTest, ToUpper) {
  EXPECT_EQ(to_upper(""), "");
  EXPECT_EQ(to_upper("hello"), "HELLO");
  EXPECT_EQ(to_upper("HELLO"), "HELLO");
}

TEST(StringsTest, ToLower) {
  EXPECT_EQ(to_lower(""), "");
  EXPECT_EQ(to_lower("HELLO"), "hello");
  EXPECT_EQ(to_lower("hello"), "hello");
}

} // namespace irata::common::strings
