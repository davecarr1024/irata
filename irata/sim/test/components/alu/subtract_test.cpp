#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/subtract.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class SubtractTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  Status carry = Status("carry", &irata);
  ALU alu = ALU(irata, data_bus, carry);
  const Subtract &subtract =
      dynamic_cast<const Subtract &>(*alu.module(hdl::AluOpcode::Subtract));
};

} // namespace

TEST_F(SubtractTest, Properties) {
  EXPECT_EQ(subtract.name(), "subtract");
  EXPECT_EQ(subtract.opcode(), hdl::AluOpcode::Subtract);
  EXPECT_EQ(subtract.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(subtract.parent(), &alu);
}

TEST_F(SubtractTest, Subtract) {
  EXPECT_EQ(subtract.apply(false, Byte(0x03), Byte(0x02)), (Module::Result{
                                                               .value = 0x01,
                                                           }));
}

TEST_F(SubtractTest, SubtractWithCarry) {
  EXPECT_EQ(subtract.apply(true, Byte(0x03), Byte(0x02)), (Module::Result{
                                                              .value = 0x00,
                                                              .zero = true,
                                                          }));
}

TEST_F(SubtractTest, SubtractOverflow) {
  EXPECT_EQ(subtract.apply(false, Byte(0x00), Byte(0x01)), (Module::Result{
                                                               .value = 0xFF,
                                                               .carry = true,
                                                               .negative = true,
                                                               .overflow = true,
                                                           }));
}

} // namespace irata::sim::components::alu
