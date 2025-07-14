#include <gtest/gtest.h>
#include <irata/sim/components/component.h>

using namespace irata::sim;

TEST(ComponentTest, Ctor) {
  const Component test_component("test");
  EXPECT_EQ(test_component.name(), "test");
}