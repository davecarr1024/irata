#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/fake_component.hpp>
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

TEST(FakeComponentDeclTest, VerifyNull) {
  FakeComponentDecl irata(ComponentType::Irata, "irata");
  EXPECT_THROW(irata.verify(nullptr), std::invalid_argument);
}

TEST(FakeComponentDeclTest, VerifyWrongType) {
  FakeComponentDecl irata_decl(ComponentType::Irata, "irata");
  components::FakeComponent irata_component(ComponentType::Cpu, "irata");
  EXPECT_THROW(irata_decl.verify(&irata_component), std::invalid_argument);
}

TEST(FakeComponentDeclTest, VerifyChildNotFound) {
  FakeComponentDecl irata_decl(ComponentType::Irata, "irata");
  FakeComponentDecl cpu_decl(ComponentType::Cpu, "cpu", &irata_decl);
  components::FakeComponent irata_component(ComponentType::Irata, "irata");
  EXPECT_THROW(irata_decl.verify(&irata_component), std::invalid_argument);
}

TEST(FakeComponentDeclTest, VerifyChildWrongType) {
  FakeComponentDecl irata_decl(ComponentType::Irata, "irata");
  FakeComponentDecl cpu_decl(ComponentType::Cpu, "cpu", &irata_decl);
  components::FakeComponent irata_component(ComponentType::Irata, "irata");
  components::FakeComponent cpu_component(ComponentType::Memory, "cpu",
                                          &irata_component);
  EXPECT_THROW(irata_decl.verify(&irata_component), std::invalid_argument);
}

TEST(FakeComponentDeclTest, VerifyWithChildren) {
  FakeComponentDecl irata_decl(ComponentType::Irata, "irata");
  FakeComponentDecl cpu_decl(ComponentType::Cpu, "cpu", &irata_decl);
  components::FakeComponent irata_component(ComponentType::Irata, "irata");
  components::FakeComponent cpu_component(ComponentType::Cpu, "cpu",
                                          &irata_component);
  EXPECT_NO_THROW(irata_decl.verify(&irata_component));
}

} // namespace irata::sim::hdl
