#include <gtest/gtest.h>
#include <irata/sim/hdl/hdl.hpp>

namespace irata::sim::hdl {

TEST(HdlTest, Irata) { irata(); }

TEST(HdlTest, ControlPhases) {
  static_assert(WriteControlDecl::phase_v == TickPhase::Write);
  static_assert(ReadControlDecl::phase_v == TickPhase::Read);
  static_assert(ProcessControlDecl::phase_v == TickPhase::Process);
  static_assert(ClearControlDecl::phase_v == TickPhase::Clear);

  const ComponentDecl root("root", nullptr);
  const ByteBusDecl bus("bus", root);
  EXPECT_EQ(WriteControlDecl("write", root, bus).phase(), TickPhase::Write);
  EXPECT_EQ(ReadControlDecl("read", root, bus).phase(), TickPhase::Read);
  EXPECT_EQ(ProcessControlDecl("process", root).phase(), TickPhase::Process);
  EXPECT_EQ(ClearControlDecl("clear", root).phase(), TickPhase::Clear);
}

} // namespace irata::sim::hdl
