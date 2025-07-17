#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/control.hpp>

using ::testing::Optional;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components {

TEST(ControlTest, AutoClear) {
  Control control("control");
  // No clear control value because auto_clear is true.
  EXPECT_EQ(control.clear(), std::nullopt);
  // Default value is false.
  EXPECT_FALSE(control.value());
  // Tick without setting - stays false.
  control.tick();
  EXPECT_FALSE(control.value());
  // Set to true.
  control.set_value(true);
  // Immediate value is true.
  EXPECT_TRUE(control.value());
  // Tick - autoresets to false.
  control.tick();
  EXPECT_FALSE(control.value());
}

TEST(ControlTest, ManualClear) {
  Control control("control", nullptr, false);
  // By default clear control is false and value is false.
  EXPECT_FALSE(control.value());
  EXPECT_THAT(control.clear(), Optional(false));
  // Set value to true.
  control.set_value(true);
  // Immediate value is true.
  EXPECT_TRUE(control.value());
  // Tick - does not autoreset.
  control.tick();
  EXPECT_TRUE(control.value());
  // Set clear to true.
  control.set_clear(true);
  // Clear is true.
  EXPECT_THAT(control.clear(), Optional(true));
  // Tick - manually resets to false.
  control.tick();
  EXPECT_FALSE(control.value());
  // Clear autoresets to false.
  EXPECT_THAT(control.clear(), Optional(false));
}

TEST(ControlTest, GetControls) {
  Component root("root");
  Control control1("control1", &root);
  Control control2("control2", &root);
  EXPECT_THAT(root.controls(), UnorderedElementsAre(&control1, &control2));
}

} // namespace irata::sim::components
