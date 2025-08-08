#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/and.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class AndTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  Status carry = Status("carry", &irata);
  ALU alu = ALU(irata, data_bus, carry);
  const And &and_ = dynamic_cast<const And &>(*alu.module(hdl::AluOpcode::And));
};

} // namespace

TEST_F(AndTest, Properties) {
  EXPECT_EQ(and_.name(), "and");
  EXPECT_EQ(and_.opcode(), hdl::AluOpcode::And);
  EXPECT_EQ(and_.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(and_.parent(), &alu);
}

TEST_F(AndTest, And) {
  EXPECT_EQ(and_.apply(false, Byte(0b0011), Byte(0b0101)), (Module::Result{
                                                               .value = 0b0001,
                                                           }));
}

} // namespace irata::sim::components::alu
