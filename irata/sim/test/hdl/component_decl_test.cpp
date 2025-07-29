#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

namespace {

class ComponentDeclTest : public ::testing::Test {
protected:
  const TypedComponentDecl<ComponentType::Irata> irata =
      TypedComponentDecl<ComponentType::Irata>("irata");
  const ComponentWithParentDecl<ComponentType::Cpu> cpu =
      ComponentWithParentDecl<ComponentType::Cpu>("cpu", irata);
};

} // namespace

TEST_F(ComponentDeclTest, Name) {
  EXPECT_EQ(irata.name(), "irata");
  EXPECT_EQ(cpu.name(), "cpu");
}

TEST_F(ComponentDeclTest, Type) {
  EXPECT_EQ(irata.type(), ComponentType::Irata);
  static_assert(TypedComponentDecl<ComponentType::Irata>::type_v ==
                ComponentType::Irata);
  EXPECT_EQ(cpu.type(), ComponentType::Cpu);
  static_assert(ComponentWithParentDecl<ComponentType::Cpu>::type_v ==
                ComponentType::Cpu);
}

TEST_F(ComponentDeclTest, Parent) {
  EXPECT_EQ(irata.parent(), nullptr);
  EXPECT_EQ(cpu.parent(), &irata);
}

TEST_F(ComponentDeclTest, Path) {
  EXPECT_EQ(irata.path(), "/");
  EXPECT_EQ(cpu.path(), "/cpu");
}

TEST_F(ComponentDeclTest, Root) {
  EXPECT_EQ(irata.root(), &irata);
  EXPECT_EQ(cpu.root(), &irata);
}

} // namespace irata::sim::hdl