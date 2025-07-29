#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/control.hpp>

using ::testing::Optional;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components {

TEST(ControlTest, ValidConfigurations) {
  for (const auto &[auto_clear, phase] :
       std::vector<std::pair<bool, hdl::TickPhase>>{
           {true, hdl::TickPhase::Write},
           {true, hdl::TickPhase::Read},
           {true, hdl::TickPhase::Process},
           {false, hdl::TickPhase::Process},
           {false, hdl::TickPhase::Clear},
       }) {
    Control control("control", phase, nullptr, auto_clear);
    EXPECT_EQ(control.phase(), phase);
    EXPECT_EQ(control.auto_clear(), auto_clear);
  }
}

TEST(ControlTest, InvalidConfigurations) {
  for (const auto &[auto_clear, phase] :
       std::vector<std::pair<bool, hdl::TickPhase>>{
           {true, hdl::TickPhase::Control},
           {true, hdl::TickPhase::Clear},
           {false, hdl::TickPhase::Control},
           {false, hdl::TickPhase::Write},
           {false, hdl::TickPhase::Read},
       }) {
    EXPECT_THROW(Control("control", phase, nullptr, auto_clear),
                 std::invalid_argument);
  }
}

TEST(ControlTest, AutoClear) {
  Control control("control", hdl::TickPhase::Process);
  EXPECT_TRUE(control.auto_clear());
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
  Control control("control", hdl::TickPhase::Process, nullptr, false);
  EXPECT_FALSE(control.auto_clear());
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
  Control control1("control1", hdl::TickPhase::Process, &root);
  Control control2("control2", hdl::TickPhase::Process, &root);
  EXPECT_THAT(root.controls(), UnorderedElementsAre(&control1, &control2));
}

} // namespace irata::sim::components
