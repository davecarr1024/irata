#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/shift_left.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class ShiftLeftTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  ALU alu = ALU(irata, data_bus);
  const ShiftLeft &shift_left =
      dynamic_cast<const ShiftLeft &>(*alu.module(hdl::AluOpcode::ShiftLeft));
};

} // namespace

TEST_F(ShiftLeftTest, Properties) {
  EXPECT_EQ(shift_left.name(), "shift_left");
  EXPECT_EQ(shift_left.opcode(), hdl::AluOpcode::ShiftLeft);
  EXPECT_EQ(shift_left.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(shift_left.parent(), &alu);
}

TEST_F(ShiftLeftTest, ShiftLeft) {
  EXPECT_EQ(shift_left.apply(false, Byte(0b1), Byte()), (Module::Result{
                                                            .value = 0b10,
                                                        }));
}

TEST_F(ShiftLeftTest, ShiftLeftCarry) {
  EXPECT_EQ(shift_left.apply(false, Byte(0b10000000), Byte()),
            (Module::Result{
                .value = 0b0,
                .carry = true,
                .zero = true,
            }));
}

} // namespace irata::sim::components::alu
