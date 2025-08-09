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

TEST(ControlTest, CanBeReadDuringPhase) {
  for (const auto &[auto_clear, control_phase, phase, expected] :
       std::vector<std::tuple<bool, hdl::TickPhase,
                              std::optional<hdl::TickPhase>, bool>>{
           {
               true,
               hdl::TickPhase::Write,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Control,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Write,
               true,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Read,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Process,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Clear,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Control,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Write,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Read,
               true,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Process,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Clear,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Control,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Write,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Read,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Process,
               true,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Clear,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               std::nullopt,
               true,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Control,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Write,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Read,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Process,
               true,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Clear,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               std::nullopt,
               true,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Control,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Write,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Read,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Process,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Clear,
               true,
           },
       }) {
    Control control("control", control_phase, nullptr, auto_clear);
    EXPECT_EQ(control.can_be_read_during_phase(phase), expected);
  }
}

TEST(ControlTest, CanBeSetDuringPhase) {
  for (const auto &[auto_clear, control_phase, phase, expected] :
       std::vector<std::tuple<bool, hdl::TickPhase,
                              std::optional<hdl::TickPhase>, bool>>{
           {
               true,
               hdl::TickPhase::Write,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Control,
               true,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Write,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Read,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Process,
               false,
           },
           {
               true,
               hdl::TickPhase::Write,
               hdl::TickPhase::Clear,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Control,
               true,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Write,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Read,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Process,
               false,
           },
           {
               true,
               hdl::TickPhase::Read,
               hdl::TickPhase::Clear,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               std::nullopt,
               true,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Control,
               true,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Write,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Read,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Process,
               false,
           },
           {
               true,
               hdl::TickPhase::Process,
               hdl::TickPhase::Clear,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               std::nullopt,
               true,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Control,
               true,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Write,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Read,
               false,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Process,
               true,
           },
           {
               false,
               hdl::TickPhase::Process,
               hdl::TickPhase::Clear,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               std::nullopt,
               true,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Control,
               true,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Write,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Read,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Process,
               false,
           },
           {
               false,
               hdl::TickPhase::Clear,
               hdl::TickPhase::Clear,
               true,
           },
       }) {
    Control control("control", control_phase, nullptr, auto_clear);
    std::string phase_str;
    if (phase == std::nullopt) {
      phase_str = "nullopt";
    } else {
      std::ostringstream os;
      os << *phase;
      phase_str = os.str();
    }
    EXPECT_EQ(control.can_be_set_during_phase(phase), expected)
        << "auto_clear=" << auto_clear << " control_phase=" << control_phase
        << " phase=" << phase_str;
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
