#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/rotate_right.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class RotateRightTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  ALU alu = ALU(irata, data_bus);
  const RotateRight &rotate_right = dynamic_cast<const RotateRight &>(
      *alu.module(hdl::AluOpcode::RotateRight));
};

} // namespace

TEST_F(RotateRightTest, Properties) {
  EXPECT_EQ(rotate_right.name(), "rotate_right");
  EXPECT_EQ(rotate_right.opcode(), hdl::AluOpcode::RotateRight);
  EXPECT_EQ(rotate_right.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(rotate_right.parent(), &alu);
}

TEST_F(RotateRightTest, RotateRight) {
  EXPECT_EQ(rotate_right.apply(false, Byte(0b10), Byte()), (Module::Result{
                                                               .value = 0b01,
                                                           }));
}

TEST_F(RotateRightTest, RotateRightCarryIn) {
  EXPECT_EQ(rotate_right.apply(true, Byte(0b10), Byte()),
            (Module::Result{
                .value = 0b10000001,
            }));
}

TEST_F(RotateRightTest, RotateRightCarryOut) {
  EXPECT_EQ(rotate_right.apply(false, Byte(0b1), Byte()), (Module::Result{
                                                              .value = 0b0,
                                                              .carry = true,
                                                              .zero = true,
                                                          }));
}

} // namespace irata::sim::components::alu
