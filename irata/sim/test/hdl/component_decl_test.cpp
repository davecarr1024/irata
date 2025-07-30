#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

namespace {

class ComponentDeclTest : public ::testing::Test {
protected:
  const ComponentWithTypeDecl<ComponentType::Irata> irata =
      ComponentWithTypeDecl<ComponentType::Irata>("irata");
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
  static_assert(ComponentWithTypeDecl<ComponentType::Irata>::type_v ==
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

TEST_F(ComponentDeclTest, ToString) {
  const auto to_string = [](const ComponentDecl &component) {
    std::ostringstream os;
    os << component;
    return os.str();
  };
  EXPECT_EQ(to_string(irata), "Irata(/)");
  EXPECT_EQ(to_string(cpu), "Cpu(/cpu)");
}

TEST_F(ComponentDeclTest, VerifyNull) {
  EXPECT_THROW(irata.verify(nullptr), std::invalid_argument);
}

TEST_F(ComponentDeclTest, VerifyTypeMismatch) {
  const components::FakeComponent component(ComponentType::Cpu, "cpu");
  EXPECT_THROW(irata.verify(&component), std::invalid_argument);
}

TEST_F(ComponentDeclTest, Verify) {
  const components::FakeComponent component(ComponentType::Irata, "irata");
  EXPECT_NO_THROW(irata.verify(&component));
}

} // namespace irata::sim::hdl
