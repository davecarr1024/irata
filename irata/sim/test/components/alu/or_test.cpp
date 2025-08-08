#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/or.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class OrTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  Status carry = Status("carry", &irata);
  ALU alu = ALU(irata, data_bus, carry);
  const Or &or_ = dynamic_cast<const Or &>(*alu.module(hdl::AluOpcode::Or));
};

} // namespace

TEST_F(OrTest, Properties) {
  EXPECT_EQ(or_.name(), "or");
  EXPECT_EQ(or_.opcode(), hdl::AluOpcode::Or);
  EXPECT_EQ(or_.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(or_.parent(), &alu);
}

TEST_F(OrTest, Or) {
  EXPECT_EQ(or_.apply(false, Byte(0b0011), Byte(0b0101)), (Module::Result{
                                                              .value = 0b0111,
                                                          }));
}

} // namespace irata::sim::components::alu
