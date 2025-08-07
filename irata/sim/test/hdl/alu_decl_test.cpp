#include <gmock/gmock.h>
#include <gtest/gtest.h>
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
  const AluDecl alu = {irata, bus};

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
  EXPECT_EQ(alu.carry().parent(), &alu);
  EXPECT_EQ(alu.carry().name(), "carry");
  EXPECT_EQ(alu.zero().parent(), &alu);
  EXPECT_EQ(alu.zero().name(), "zero");
  EXPECT_EQ(alu.negative().parent(), &alu);
  EXPECT_EQ(alu.negative().name(), "negative");
  EXPECT_EQ(alu.overflow().parent(), &alu);
  EXPECT_EQ(alu.overflow().name(), "overflow");
  EXPECT_EQ(alu.half_carry().parent(), &alu);
  EXPECT_EQ(alu.half_carry().name(), "half_carry");
}

TEST_F(AluDeclTest, Modules) {
  EXPECT_THAT(alu.modules(),
              UnorderedElementsAre(
                  Pointee(AllOf(ModuleHasOpcode(0x01), ModuleHasName("cmp"))),
                  Pointee(AllOf(ModuleHasOpcode(0x02), ModuleHasName("adc"))),
                  Pointee(AllOf(ModuleHasOpcode(0x03), ModuleHasName("sbc")))));
  EXPECT_THAT(
      alu.modules(),
      Each(Pointee(AllOf(ComponentDeclHasParent(&alu),
                         ComponentDeclHasType(ComponentType::AluModule)))));
}

TEST_F(AluDeclTest, MaxOpcode) { EXPECT_EQ(alu.max_opcode(), 0x03); }

TEST_F(AluDeclTest, NumOpcodeControls) {
  EXPECT_EQ(alu.num_opcode_controls(), 2);
}

TEST_F(AluDeclTest, OpcodeControls) {
  EXPECT_THAT(alu.opcode_controls(),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0")),
                                   Pointee(ComponentDeclHasName("opcode_1"))));
  EXPECT_THAT(alu.opcode_controls(),
              Each(Pointee(ComponentDeclHasParent(&alu))));
}

TEST_F(AluDeclTest, OpcodeControlsForOpcode) {
  EXPECT_THAT(alu.opcode_controls_for_opcode(0x00), IsEmpty());
  EXPECT_THAT(alu.opcode_controls_for_opcode(0x01),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0"))));
  EXPECT_THAT(alu.opcode_controls_for_opcode(0x02),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_1"))));
  EXPECT_THAT(alu.opcode_controls_for_opcode(0x03),
              UnorderedElementsAre(Pointee(ComponentDeclHasName("opcode_0")),
                                   Pointee(ComponentDeclHasName("opcode_1"))));
}

TEST_F(AluDeclTest, OpcodeControlsForOpcode_Invalid) {
  EXPECT_THROW(alu.opcode_controls_for_opcode(0x04), std::invalid_argument);
}

} // namespace irata::sim::hdl
