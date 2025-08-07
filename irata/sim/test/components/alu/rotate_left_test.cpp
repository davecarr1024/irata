#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/rotate_left.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class RotateLeftTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  ALU alu = ALU(irata, data_bus);
  const RotateLeft &rotate_left =
      dynamic_cast<const RotateLeft &>(*alu.module(hdl::AluOpcode::RotateLeft));
};

} // namespace

TEST_F(RotateLeftTest, Properties) {
  EXPECT_EQ(rotate_left.name(), "rotate_left");
  EXPECT_EQ(rotate_left.opcode(), hdl::AluOpcode::RotateLeft);
  EXPECT_EQ(rotate_left.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(rotate_left.parent(), &alu);
}

TEST_F(RotateLeftTest, RotateLeft) {
  EXPECT_EQ(rotate_left.apply(false, Byte(0b1), Byte()), (Module::Result{
                                                             .value = 0b10,
                                                         }));
}

TEST_F(RotateLeftTest, RotateLeftCarryIn) {
  EXPECT_EQ(rotate_left.apply(true, Byte(0b1), Byte()), (Module::Result{
                                                            .value = 0b11,
                                                        }));
}

TEST_F(RotateLeftTest, RotateLeftCarryOut) {
  EXPECT_EQ(rotate_left.apply(false, Byte(0b10000000), Byte()),
            (Module::Result{
                .value = 0b0,
                .carry = true,
                .zero = true,
            }));
}

} // namespace irata::sim::components::alu
