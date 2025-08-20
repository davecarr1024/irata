#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/hdl/alu_decl.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>

using ::testing::AllOf;
using ::testing::Each;
using ::testing::IsEmpty;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::hdl {

namespace {

class AluDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl irata = {ComponentType::Irata, "irata"};
  const ByteBusDecl bus = {"bus", irata};
  const StatusDecl carry_in = {"carry", irata};
  const AluDecl alu = {irata, bus, carry_in};
  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::ByteBus data_bus_component =
      components::ByteBus("data_bus", &irata_component);
  components::Status carry_component =
      components::Status("carry", &irata_component);

  template <typename Matcher>
  auto ModuleHasOpcode(const Matcher &matcher) const {
    return Property("opcode", &AluDecl::ModuleDecl::opcode, matcher);
  }

  template <typename Matcher> auto ModuleHasName(const Matcher &matcher) const {
    return Property("name", &AluDecl::ModuleDecl::name, matcher);
  }

  template <typename Matcher>
  auto ComponentDeclHasName(const Matcher &matcher) const {
    return Property("name", &ComponentDecl::name, matcher);
  }

  template <typename Matcher>
  auto ComponentDeclHasParent(const Matcher &matcher) const {
    return Property("parent", &ComponentDecl::parent, matcher);
  }

  template <typename Matcher>
  auto ComponentDeclHasType(const Matcher &matcher) const {
    return Property("type", &ComponentDecl::type, matcher);
  }
};

} // namespace

TEST_F(AluDeclTest, Properties) {
  EXPECT_EQ(alu.type(), ComponentType::Alu);
  EXPECT_EQ(alu.name(), "alu");
  EXPECT_EQ(alu.parent(), &irata);
  EXPECT_EQ(alu.lhs().parent(), &alu);
  EXPECT_EQ(alu.lhs().name(), "lhs");
  EXPECT_EQ(alu.rhs().parent(), &alu);
  EXPECT_EQ(alu.rhs().name(), "rhs");
  EXPECT_EQ(alu.result().parent(), &alu);
  EXPECT_EQ(alu.result().name(), "result");
  EXPECT_EQ(&alu.carry_in(), &carry_in);
  EXPECT_EQ(alu.carry_in().parent(), &irata);
  EXPECT_EQ(alu.carry_in().name(), "carry");
  EXPECT_EQ(alu.carry_in().path(), "/carry");
  EXPECT_EQ(alu.carry_out().parent(), &alu);
  EXPECT_EQ(alu.carry_out().name(), "carry");
  EXPECT_EQ(alu.carry_out().path(), "/alu/carry");
  EXPECT_EQ(alu.zero().parent(), &alu);
  EXPECT_EQ(alu.zero().name(), "zero");
  EXPECT_EQ(alu.negative().parent(), &alu);
  EXPECT_EQ(alu.negative().name(), "negative");
  EXPECT_EQ(alu.overflow().parent(), &alu);
  EXPECT_EQ(alu.overflow().name(), "overflow");
}

TEST_F(AluDeclTest, Modules) {
  EXPECT_THAT(
      alu.modules(),
      UnorderedElementsAre(
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::Add), ModuleHasName("add"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::Subtract),
                        ModuleHasName("subtract"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::And), ModuleHasName("and"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::Or), ModuleHasName("or"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::Xor), ModuleHasName("xor"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::RotateLeft),
                        ModuleHasName("rotate_left"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::RotateRight),
                        ModuleHasName("rotate_right"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::ShiftLeft),
                        ModuleHasName("shift_left"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::ShiftRight),
                        ModuleHasName("shift_right"))),
          Pointee(AllOf(ModuleHasOpcode(AluOpcode::AddressAdd),
                        ModuleHasName("address_add")))));
  EXPECT_THAT(
      alu.modules(),
      Each(Pointee(AllOf(ComponentDeclHasParent(&alu),
                         ComponentDeclHasType(ComponentType::AluModule)))));
}

TEST_F(AluDeclTest, MaxOpcode) { EXPECT_EQ(alu.max_opcode(), 0x0A); }

TEST_F(AluDeclTest, NumOpcodeControls) {
  EXPECT_EQ(alu.num_opcode_controls(), 4);
}

TEST_F(AluDeclTest, OpcodeControls) {
  EXPECT_THAT(alu.opcode_controls(),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0")),
                                   Pointee(ComponentDeclHasName("opcode_1")),
                                   Pointee(ComponentDeclHasName("opcode_2")),
                                   Pointee(ComponentDeclHasName("opcode_3"))));
  EXPECT_THAT(alu.opcode_controls(),
              Each(Pointee(ComponentDeclHasParent(&alu))));
}

TEST_F(AluDeclTest, OpcodeControlsForOpcode) {
  EXPECT_THAT(alu.opcode_controls_for_opcode(AluOpcode::Nop), IsEmpty());
  EXPECT_THAT(alu.opcode_controls_for_opcode(AluOpcode::Add),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0"))));
  EXPECT_THAT(alu.opcode_controls_for_opcode(AluOpcode::Subtract),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_1"))));
  EXPECT_THAT(alu.opcode_controls_for_opcode(AluOpcode::And),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0")),
                                   Pointee(ComponentDeclHasName("opcode_1"))));
}

TEST_F(AluDeclTest, Verify) {
  auto alu_component = components::alu::ALU(irata_component, data_bus_component,
                                            carry_component);
  EXPECT_NO_THROW(alu.verify(&alu_component));
}

TEST_F(AluDeclTest, GetModule) {
  const auto &add = alu.module(AluOpcode::Add);
  EXPECT_EQ(add.name(), "add");
  EXPECT_EQ(add.parent(), &alu);
}

TEST_F(AluDeclTest, GetModuleInvalid) {
  EXPECT_THROW(alu.module(AluOpcode::Nop), std::invalid_argument);
}

TEST_F(AluDeclTest, GetAddressAddCarry) {
  const auto &address_add = alu.module(AluOpcode::AddressAdd);
  EXPECT_EQ(address_add.name(), "address_add");
  EXPECT_EQ(address_add.parent(), &alu);
  const auto &address_add_carry = alu.address_add_carry();
  EXPECT_EQ(address_add_carry.parent(), &address_add);
  EXPECT_EQ(address_add_carry.name(), "carry");
}

} // namespace irata::sim::hdl
