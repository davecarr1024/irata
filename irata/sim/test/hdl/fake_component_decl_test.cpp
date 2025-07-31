#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/hdl/fake_component_decl.hpp>

namespace irata::sim::hdl {

TEST(FakeComponentDeclTest, Empty) {
  const FakeComponentDecl irata(ComponentType::Irata, "irata");
  EXPECT_EQ(irata.name(), "irata");
  EXPECT_EQ(irata.type(), ComponentType::Irata);
  EXPECT_EQ(irata.parent(), nullptr);
  EXPECT_EQ(irata.path(), "/");
}

TEST(FakeComponentDeclTest, WithChildren) {
  FakeComponentDecl irata(ComponentType::Irata, "irata");
  const FakeComponentDecl cpu(ComponentType::Cpu, "cpu", &irata);
  EXPECT_EQ(cpu.name(), "cpu");
  EXPECT_EQ(cpu.type(), ComponentType::Cpu);
  EXPECT_EQ(cpu.parent(), &irata);
  EXPECT_EQ(cpu.path(), "/cpu");
}

} // namespace irata::sim::hdl
