#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

using ::testing::AllOf;
using ::testing::Contains;
using ::testing::Each;
using ::testing::IsEmpty;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::alu {

namespace {

class ALUTest : public ::testing::Test {
protected:
  FakeComponent root = {hdl::ComponentType::Irata, "root"};
  ByteBus data_bus = ByteBus("data_bus", &root);
  Register source = Register("source", &data_bus, &root);
  Register dest = Register("dest", &data_bus, &root);
  Status carry = Status("carry", &root);
  ALU alu = ALU(root, data_bus, carry);

  template <typename Matcher>
  auto ComponentHasName(const Matcher &matcher) const {
    return Property("name", &Component::name, matcher);
  }

  template <typename Matcher>
  auto ComponentHasParent(const Matcher &matcher) const {
    return Property("parent", &Component::parent, matcher);
  }

  template <typename Matcher>
  auto ControlHasValue(const Matcher &matcher) const {
    return Property("value", &Control::value, matcher);
  }
};

} // namespace

TEST_F(ALUTest, Properties) {
  EXPECT_EQ(alu.type(), hdl::ComponentType::Alu);
  EXPECT_EQ(alu.name(), "alu");
  EXPECT_EQ(alu.parent(), &root);
  EXPECT_EQ(alu.path(), "/alu");
  EXPECT_EQ(alu.lhs_register().parent(), &alu);
  EXPECT_EQ(alu.lhs_register().name(), "lhs");
  EXPECT_EQ(alu.rhs_register().parent(), &alu);
  EXPECT_EQ(alu.rhs_register().name(), "rhs");
  EXPECT_EQ(alu.result_register().parent(), &alu);
  EXPECT_EQ(alu.result_register().name(), "result");
  EXPECT_EQ(&alu.carry_in_status(), &carry);
  EXPECT_EQ(alu.carry_in_status().parent(), &root);
  EXPECT_EQ(alu.carry_in_status().name(), "carry");
  EXPECT_EQ(alu.carry_in_status().path(), "/carry");
  EXPECT_EQ(alu.carry_out_status().parent(), &alu);
  EXPECT_EQ(alu.carry_out_status().name(), "carry");
  EXPECT_EQ(alu.carry_out_status().path(), "/alu/carry");
  EXPECT_EQ(alu.zero_status().parent(), &alu);
  EXPECT_EQ(alu.zero_status().name(), "zero");
  EXPECT_EQ(alu.negative_status().parent(), &alu);
  EXPECT_EQ(alu.negative_status().name(), "negative");
  EXPECT_EQ(alu.overflow_status().parent(), &alu);
  EXPECT_EQ(alu.overflow_status().name(), "overflow");
  EXPECT_THAT(alu.opcode_controls(), Each(Pointee(ComponentHasParent(&alu))));
  EXPECT_THAT(alu.modules(), Each(Pointee(ComponentHasParent(&alu))));
}

TEST_F(ALUTest, Opcode) {
  alu.set_opcode(hdl::AluOpcode::Nop);
  EXPECT_EQ(alu.opcode(), hdl::AluOpcode::Nop);
  EXPECT_THAT(
      alu.opcode_controls(),
      UnorderedElementsAre(
          Pointee(AllOf(ComponentHasName("opcode_0"), ControlHasValue(false))),
          Pointee(AllOf(ComponentHasName("opcode_1"), ControlHasValue(false))),
          Pointee(AllOf(ComponentHasName("opcode_2"), ControlHasValue(false))),
          Pointee(
              AllOf(ComponentHasName("opcode_3"), ControlHasValue(false)))));
  alu.set_opcode(hdl::AluOpcode::Add);
  EXPECT_EQ(alu.opcode(), hdl::AluOpcode::Add);
  EXPECT_THAT(
      alu.opcode_controls(),
      UnorderedElementsAre(
          Pointee(AllOf(ComponentHasName("opcode_0"), ControlHasValue(true))),
          Pointee(AllOf(ComponentHasName("opcode_1"), ControlHasValue(false))),
          Pointee(AllOf(ComponentHasName("opcode_2"), ControlHasValue(false))),
          Pointee(
              AllOf(ComponentHasName("opcode_3"), ControlHasValue(false)))));
  alu.set_opcode(hdl::AluOpcode::Subtract);
  EXPECT_EQ(alu.opcode(), hdl::AluOpcode::Subtract);
  EXPECT_THAT(
      alu.opcode_controls(),
      UnorderedElementsAre(
          Pointee(AllOf(ComponentHasName("opcode_0"), ControlHasValue(false))),
          Pointee(AllOf(ComponentHasName("opcode_1"), ControlHasValue(true))),
          Pointee(AllOf(ComponentHasName("opcode_2"), ControlHasValue(false))),
          Pointee(
              AllOf(ComponentHasName("opcode_3"), ControlHasValue(false)))));
}

TEST_F(ALUTest, EncodeAndDecodeOpcode) {
  for (const auto &opcode : std::vector<hdl::AluOpcode>{
           hdl::AluOpcode::Nop,
           hdl::AluOpcode::Add,
           hdl::AluOpcode::Subtract,
           hdl::AluOpcode::And,
           hdl::AluOpcode::Or,
           hdl::AluOpcode::Xor,
           hdl::AluOpcode::RotateLeft,
           hdl::AluOpcode::RotateRight,
           hdl::AluOpcode::ShiftLeft,
           hdl::AluOpcode::ShiftRight,
       }) {
    alu.set_opcode(opcode);
    EXPECT_EQ(alu.opcode(), opcode);
  }
}

TEST_F(ALUTest, ControlsForOpcode) {
  EXPECT_THAT(alu.opcode_controls_for_opcode(hdl::AluOpcode::Nop), IsEmpty());
  EXPECT_THAT(alu.opcode_controls_for_opcode(hdl::AluOpcode::Add),
              UnorderedElementsAre(Pointee(ComponentHasName("opcode_0"))));
  EXPECT_THAT(alu.opcode_controls_for_opcode(hdl::AluOpcode::Subtract),
              UnorderedElementsAre(Pointee(ComponentHasName("opcode_1"))));
}

TEST_F(ALUTest, Modules) {
  EXPECT_THAT(alu.modules(),
              AllOf(Contains(Pointee(ComponentHasName("add"))),
                    Contains(Pointee(ComponentHasName("subtract")))));
  EXPECT_EQ(alu.module(hdl::AluOpcode::Nop), nullptr);
  EXPECT_THAT(alu.module(hdl::AluOpcode::Add),
              Pointee(ComponentHasName("add")));
  EXPECT_THAT(alu.module(hdl::AluOpcode::Subtract),
              Pointee(ComponentHasName("subtract")));
  alu.set_opcode(hdl::AluOpcode::Nop);
  EXPECT_EQ(alu.module(), nullptr);
  alu.set_opcode(hdl::AluOpcode::Add);
  EXPECT_THAT(alu.module(), Pointee(ComponentHasName("add")));
}

TEST_F(ALUTest, CarryIn) {
  carry.set_value(false);
  EXPECT_FALSE(alu.carry_in());
  carry.set_value(true);
  EXPECT_TRUE(alu.carry_in());
}

TEST_F(ALUTest, Lhs) {
  alu.set_lhs(0x00);
  EXPECT_EQ(alu.lhs(), 0x00);
  alu.set_lhs(0xFF);
  EXPECT_EQ(alu.lhs(), 0xFF);
}

TEST_F(ALUTest, Rhs) {
  alu.set_rhs(0x00);
  EXPECT_EQ(alu.rhs(), 0x00);
  alu.set_rhs(0xFF);
  EXPECT_EQ(alu.rhs(), 0xFF);
}

TEST_F(ALUTest, Result) {
  alu.set_result(0x00);
  EXPECT_EQ(alu.result(), 0x00);
  alu.set_result(0xFF);
  EXPECT_EQ(alu.result(), 0xFF);
}

TEST_F(ALUTest, CarryOut) {
  alu.set_carry_out(false);
  EXPECT_FALSE(alu.carry_out());
  alu.set_carry_out(true);
  EXPECT_TRUE(alu.carry_out());
}

TEST_F(ALUTest, Zero) {
  alu.set_zero(false);
  EXPECT_FALSE(alu.zero());
  alu.set_zero(true);
  EXPECT_TRUE(alu.zero());
}

TEST_F(ALUTest, Negative) {
  alu.set_negative(false);
  EXPECT_FALSE(alu.negative());
  alu.set_negative(true);
  EXPECT_TRUE(alu.negative());
}

TEST_F(ALUTest, Overflow) {
  alu.set_overflow(false);
  EXPECT_FALSE(alu.overflow());
  alu.set_overflow(true);
  EXPECT_TRUE(alu.overflow());
}

TEST_F(ALUTest, Add) {
  // Write 0x01 to lhs
  source.set_value(0x01);
  source.set_write(true);
  alu.lhs_register().set_read(true);
  alu.tick();
  EXPECT_EQ(alu.lhs(), 0x01);

  // Write 0x02 to rhs
  source.set_value(0x02);
  source.set_write(true);
  alu.rhs_register().set_read(true);
  alu.tick();

  // Set opcode to add and run alu
  alu.set_opcode(hdl::AluOpcode::Add);
  carry.set_value(false);
  alu.tick();

  // Copy result to dest
  alu.result_register().set_write(true);
  dest.set_read(true);
  alu.tick();

  // Check result
  EXPECT_EQ(dest.value(), 0x03);
}

TEST_F(ALUTest, AddressAdd) {
  alu.set_lhs(0x12);
  alu.set_rhs(0x34);
  alu.set_opcode(hdl::AluOpcode::AddressAdd);
  alu.tick();
  EXPECT_EQ(alu.result(), 0x46);
  EXPECT_FALSE(alu.address_add_carry().value());
}

TEST_F(ALUTest, AddressAddWithCarry) {
  alu.set_lhs(0xFF);
  alu.set_rhs(0x02);
  alu.set_opcode(hdl::AluOpcode::AddressAdd);
  alu.tick();
  EXPECT_EQ(alu.result(), 0x01);
  EXPECT_TRUE(alu.address_add_carry().value());
}

} // namespace irata::sim::components::alu
