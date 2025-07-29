#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/hdl/tick_phase.hpp>
#include <sstream>

namespace irata::sim::hdl {

TEST(TickPhaseTest, ToString) {
  const auto to_string = [](TickPhase phase) {
    std::ostringstream os;
    os << phase;
    return os.str();
  };
  EXPECT_EQ(to_string(TickPhase::Control), "Control");
  EXPECT_EQ(to_string(TickPhase::Write), "Write");
  EXPECT_EQ(to_string(TickPhase::Read), "Read");
  EXPECT_EQ(to_string(TickPhase::Process), "Process");
  EXPECT_EQ(to_string(TickPhase::Clear), "Clear");
}

TEST(TickPhaseTest, Ordering) {
  EXPECT_LT(TickPhase::Control, TickPhase::Write);
  EXPECT_LT(TickPhase::Write, TickPhase::Read);
  EXPECT_LT(TickPhase::Read, TickPhase::Process);
  EXPECT_LT(TickPhase::Process, TickPhase::Clear);
  EXPECT_GT(TickPhase::Clear, TickPhase::Process);
  EXPECT_GT(TickPhase::Process, TickPhase::Read);
  EXPECT_GT(TickPhase::Read, TickPhase::Write);
  EXPECT_GT(TickPhase::Write, TickPhase::Control);
}

} // namespace irata::sim::hdl