#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/status.hpp>

using namespace irata::sim;
using ::testing::UnorderedElementsAre;

TEST(StatusTest, Value) {
  Status status("status");
  EXPECT_FALSE(status.value());
  status.set_value(true);
  EXPECT_TRUE(status.value());
  status.set_value(false);
  EXPECT_FALSE(status.value());
}

TEST(StatusTest, GetStatuses) {
  Component root("root");
  Status status1("status1");
  Status status2("status2");
  root.add_child(&status1);
  root.add_child(&status2);
  EXPECT_THAT(root.statuses(), UnorderedElementsAre(&status1, &status2));
}