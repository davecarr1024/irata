#include <gtest/gtest.h>
#include <irata/sim/hdl/hdl.hpp>

namespace irata::sim::hdl {

namespace {

class HdlTest : public ::testing::Test {
protected:
  const ComponentDecl root_ = ComponentDecl("root_", nullptr);
  const ByteBusDecl bus_ = ByteBusDecl("bus_", root_);
};

} // namespace

TEST_F(HdlTest, Irata) { EXPECT_NO_THROW(irata()); }

TEST_F(HdlTest, Name) { EXPECT_EQ(root_.name(), "root_"); }

TEST_F(HdlTest, Parent) {
  const ComponentWithParentDecl child("child", root_);
  const ComponentWithParentDecl grandchild("grandchild", child);
  EXPECT_EQ(child.parent(), &root_);
  EXPECT_EQ(grandchild.parent(), &child);
}

TEST_F(HdlTest, Path) {
  const ComponentWithParentDecl child("child", root_);
  const ComponentWithParentDecl grandchild("grandchild", child);
  EXPECT_EQ(root_.path(), "/");
  EXPECT_EQ(child.path(), "/child");
  EXPECT_EQ(grandchild.path(), "/child/grandchild");
}

TEST_F(HdlTest, root_) {
  const ComponentWithParentDecl child("child", root_);
  const ComponentWithParentDecl grandchild("grandchild", child);
  EXPECT_EQ(root_.root(), &root_);
  EXPECT_EQ(child.root(), &root_);
  EXPECT_EQ(grandchild.root(), &root_);
}

TEST_F(HdlTest, WriteControlDecl) {
  static_assert(WriteControlDecl::phase_v == TickPhase::Write);
  static_assert(WriteControlDecl::auto_clear_v == true);
  const WriteControlDecl write("write", root_, bus_);
  EXPECT_EQ(&write.bus(), &bus_);
  EXPECT_EQ(write.phase(), TickPhase::Write);
  EXPECT_TRUE(write.auto_clear());
}

TEST_F(HdlTest, ReadControlDecl) {
  static_assert(ReadControlDecl::phase_v == TickPhase::Read);
  static_assert(ReadControlDecl::auto_clear_v == true);
  const ReadControlDecl read("read", root_, bus_);
  EXPECT_EQ(&read.bus(), &bus_);
  EXPECT_EQ(read.phase(), TickPhase::Read);
  EXPECT_TRUE(read.auto_clear());
}

TEST_F(HdlTest, ProcessControlDecl) {
  static_assert(ProcessControlDecl::phase_v == TickPhase::Process);
  static_assert(ProcessControlDecl::auto_clear_v == true);
  const ProcessControlDecl process("process", root_);
  EXPECT_EQ(process.phase(), TickPhase::Process);
  EXPECT_TRUE(process.auto_clear());
}

TEST_F(HdlTest, StickyProcessControlDecl) {
  static_assert(StickyProcessControlDecl::phase_v == TickPhase::Process);
  static_assert(StickyProcessControlDecl::auto_clear_v == false);
  const StickyProcessControlDecl sticky_process("sticky_process", root_);
  EXPECT_EQ(sticky_process.phase(), TickPhase::Process);
  EXPECT_FALSE(sticky_process.auto_clear());
  EXPECT_EQ(sticky_process.reset().phase(), TickPhase::Process);
  EXPECT_TRUE(sticky_process.reset().auto_clear());
  EXPECT_EQ(sticky_process.reset().path(), "/sticky_process/reset");
}

TEST_F(HdlTest, StickyClearControlDecl) {
  static_assert(StickyClearControlDecl::phase_v == TickPhase::Clear);
  static_assert(StickyClearControlDecl::auto_clear_v == false);
  const StickyClearControlDecl sticky_clear("sticky_clear", root_);
  EXPECT_EQ(sticky_clear.phase(), TickPhase::Clear);
  EXPECT_FALSE(sticky_clear.auto_clear());
  EXPECT_EQ(sticky_clear.reset().phase(), TickPhase::Process);
  EXPECT_TRUE(sticky_clear.reset().auto_clear());
  EXPECT_EQ(sticky_clear.reset().path(), "/sticky_clear/reset");
}

} // namespace irata::sim::hdl
