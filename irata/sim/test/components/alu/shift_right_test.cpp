#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/shift_right.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class ShiftRightTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  ALU alu = ALU(irata, data_bus);
  const ShiftRight &shift_right =
      dynamic_cast<const ShiftRight &>(*alu.module(hdl::AluOpcode::ShiftRight));
};

} // namespace

TEST_F(ShiftRightTest, Properties) {
  EXPECT_EQ(shift_right.name(), "shift_right");
  EXPECT_EQ(shift_right.opcode(), hdl::AluOpcode::ShiftRight);
  EXPECT_EQ(shift_right.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(shift_right.parent(), &alu);
}

TEST_F(ShiftRightTest, ShiftRight) {
  EXPECT_EQ(shift_right.apply(false, Byte(0b10), Byte()), (Module::Result{
                                                              .value = 0b01,
                                                          }));
}

TEST_F(ShiftRightTest, ShiftRightCarry) {
  EXPECT_EQ(shift_right.apply(false, Byte(0b1), Byte()), (Module::Result{
                                                             .value = 0b0,
                                                             .carry = true,
                                                             .zero = true,
                                                         }));
}

} // namespace irata::sim::components::alu
