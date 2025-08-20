#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/xor.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class XorTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  Status carry = Status("carry", &irata);
  ALU alu = ALU(irata, data_bus, carry);
  Xor &xor_ = dynamic_cast<Xor &>(*alu.module(hdl::AluOpcode::Xor));
};

} // namespace

TEST_F(XorTest, Properties) {
  EXPECT_EQ(xor_.name(), "xor");
  EXPECT_EQ(xor_.opcode(), hdl::AluOpcode::Xor);
  EXPECT_EQ(xor_.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(xor_.parent(), &alu);
}

TEST_F(XorTest, Xor) {
  EXPECT_EQ(xor_.apply(false, Byte(0b0011), Byte(0b0101)), (Module::Result{
                                                               .value = 0b0110,
                                                           }));
}

} // namespace irata::sim::components::alu
